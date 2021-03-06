#include "pmm.h"
#include "util/kutil.h"
#include "util/bitmap.h"
#include "drivers/uart/serial.h"
#include "boot/kconf.h"
#include "paging.h"

uint memory::pmm::free_memory_size;
uint memory::pmm::used_memory_size;
uint memory::pmm::total_memory_size;
uint memory::pmm::reserved_memory_size;
util::bitmap page_bitmap_map;
bool initialized = false;

const char *memory_types(uint16 type)
{
    switch (type)
    {
        case 1: return (const char *)"Conventional memory";
        case 2: return (const char *)"Reserved memory";
        case 3: return (const char *)"Reclaimable ACPI memory";
        case 4: return (const char *)"ACPI NVS memory";
        case 5: return (const char *)"Faulty memory";
        case 10: return (const char *)"Kernel memory";
        case 4096: return (const char *)"Reclaimable bootloader memory";
        default: return (const char *)"Unidentified memory";
    }
    return nullptr;
}

void init_bitmap(uint64 bitmap_size, void* addr)
{
	page_bitmap_map.size = bitmap_size;
    page_bitmap_map.buffer = (byte*)addr;
    for (uint64 i = 0; i < bitmap_size; i++)
        *(byte *)(page_bitmap_map.buffer + i) = 0;
} 

uint memory::pmm::get_total_memory_size(stivale_memory_map* memory_map, uint64 map_size, uint64 desc_size) 
{
    static uint64 memory_size_bytes = 0;
    if (memory_size_bytes > 0) return memory_size_bytes;

    uint64 map_entries = map_size / desc_size;

    for (uint i = 0; i < map_entries; i++) {
        stivale_mmap_entry* desc = (stivale_mmap_entry *)((address)memory_map->memory_map_addr + (i * desc_size));
        
        if (desc->type == 1)
            memory_size_bytes = desc->base + desc->length;
    }

    memory_size_bytes /= 0x100000; //////////
    memory_size_bytes *= 0x100000; //// account for inaccuracies

    return memory_size_bytes;
}

extern "C" void reserve_pages(void* vaddress, uint64 page_count);

void memory::pmm::initialize(stivale_memory_map* memory_map, uint64 map_size, uint64 desc_size)
{
    if (initialized) return;

    initialized = true;

    uint map_entries = map_size / desc_size;

    void* largest_free_memory_segment = NULL;
    uint largest_free_memory_segment_size = 0;

    for (uint i = 0; i < map_entries; i++) {
        stivale_mmap_entry* desc = (stivale_mmap_entry *)((address)memory_map->memory_map_addr + (i * desc_size));
        if (desc->type == 1) { // usable memory
            if ((desc->length / 0x1000) * 4096 > largest_free_memory_segment_size)
            {
                largest_free_memory_segment = (void *)desc->base;
                largest_free_memory_segment_size = (desc->length  / 0x1000) * 4096;
            }
        }
        
        io::serial::serial_msg("@ 0x");
        io::serial::serial_msg(util::itoa(desc->base, 16));
        io::serial::serial_msg(": ");
        io::serial::serial_msg(memory_types(desc->type));
        io::serial::serial_msg(", for ");
        io::serial::serial_msg(util::itoa((desc->length  / 0x1000), 10));
        io::serial::serial_msg(" pages\n");
    }

    uint memory_size = get_total_memory_size(memory_map, map_size, desc_size);
    total_memory_size = memory_size;
    free_memory_size = memory_size;
    uint bitmap_size = memory_size / 4096 / 8 + 1;     

    io::serial::serial_msg("Memory size (MiB): ");
    io::serial::serial_msg(util::itoa(memory_size / 0x100000, 10));
    io::serial::serial_msg("\nBitmap size (B): ");
    io::serial::serial_msg(util::itoa(bitmap_size, 10));
    io::serial::serial_msg("\n");

    init_bitmap(bitmap_size, largest_free_memory_segment);

    for (uint i = 0; i < map_entries; i++) {
        stivale_mmap_entry* desc = (stivale_mmap_entry *)((address)memory_map->memory_map_addr + (i * desc_size));
        if (desc->type != 1 && desc->base < memory_size) // not conventional memory
            reserve_pages((void *)desc->base, (desc->length  / 0x1000));
    }

    lock_pages(page_bitmap_map.buffer, page_bitmap_map.size / 4096 + 1);
    
    reserve_pages((void *)0x0, 0x100000 / 0x1000);
    reserve_pages((void *)&sys::config::__kernel_start, sys::config::__kernel_pages);
}

void unreserve_page(void* vaddress) {

    uint64 index = (uint64)vaddress / 4096;

    if (page_bitmap_map[index] == false) 
		return;

    page_bitmap_map.set(index, false);
    memory::pmm::free_memory_size += 4096;
    memory::pmm::reserved_memory_size -= 4096;
}

void unreserve_pages(void* vaddress, uint64 pageCount) {

    for (uint t = 0; t < pageCount; t++)
        unreserve_page((void*)((uint64)vaddress + (t * 4096)));
}

extern "C" void reserve_page(void* vaddress) 
{
    uint64 index = (uint64)vaddress / 4096;

    if (page_bitmap_map[index] == true) 
		return;

    page_bitmap_map.set(index, true);
    memory::pmm::free_memory_size -= 4096;
    memory::pmm::reserved_memory_size += 4096;
}

extern "C" void reserve_pages(void* vaddress, uint64 pageCount) {

    for (uint t = 0; t < pageCount; t++)
        reserve_page((void*)((address)vaddress + (t * 4096)));
}

extern "C" void* memory::pmm::request_page() {

    for (uint index = 0; index < page_bitmap_map.size * 8; index++) {

        if (page_bitmap_map[index] == true) 
			continue;
/*
        io::serial::serial_msg("requested page, located @paddr-0x");
        io::serial::serial_msg(util::itoa(index * 4096, 16));
        io::serial::serial_msg("\n");
*/
        memory::pmm::lock_page((void*)(index * 4096));

        return (void*)(index * 4096);
    }

    return nullptr;
}

extern "C" void* memory::pmm::request_pages(uint64 page_count)
{
    if (page_count <= 1)
        return nullptr;

    void* start_ptr = request_page();
    page_count--;

    paging::map_memory(start_ptr, start_ptr, false);

    for (uint i = 1; i <= page_count; i++)
    {
        void* page_ptr = request_page();
        paging::map_memory((void *)((uint64)start_ptr + (i * 0x1000)), page_ptr, false);
    }

    return start_ptr;
}

extern "C" void memory::pmm::free_page(void* vaddress) {

    uint index = (uint64)vaddress / 4096;
    if (page_bitmap_map[index] == false) return;
    page_bitmap_map.set(index, false);
    free_memory_size += 4096;
    used_memory_size -= 4096;
}

extern "C" void memory::pmm::free_pages(void* vaddress, uint64 page_count) {
 
    for (uint64 t = 0; t < page_count; t++)
        memory::pmm::free_page((void*)((address)vaddress + (t * 4096)));
}

extern "C" void memory::pmm::lock_page(void* vaddress) {
    uint index = (uint64)vaddress / 4096;
    if (page_bitmap_map[index] == true) return;
    page_bitmap_map.set(index, true);
    free_memory_size -= 4096;
    used_memory_size += 4096;
} 

extern "C" void memory::pmm::lock_pages(void* vaddress, uint64 page_count) {

    for (uint t = 0; t < page_count; t++)
        memory::pmm::lock_page((void*)((uint64)vaddress + (t * 4096)));
}
