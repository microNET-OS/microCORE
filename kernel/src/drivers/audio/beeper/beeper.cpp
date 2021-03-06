#include "beeper.h"

void sys::audio::pcspk::init()
{
    io::outb(0x61, io::inb(0x61) | 0x1);
}

void sys::audio::pcspk::play_sound(uint32 frequency) {
    byte tmp;
    
    io::pit::set_c2_frequency(frequency);

    tmp = io::inb(0x61);
    if (tmp != (tmp | 3)) {
        io::outb(0x61, tmp | 3);
    }
}
 
void sys::audio::pcspk::stop_sound() {
    byte tmp = io::inb(0x61) & 0xFC;
 
    io::outb(0x61, tmp);
}
 
void sys::audio::pcspk::beep() {
    play_sound(1000);
    sys::chrono::sleep(10);
    stop_sound();
    io::pit::set_c2_frequency(1);
}

void sys::audio::pcspk::beep(uint32 frequency, uint32 duration) {
    play_sound(frequency);
    sys::chrono::sleep(duration);
    stop_sound();
    io::pit::set_c2_frequency(1);
}