global kernel_entry
global stack_begin
global stack_end

extern kernel_main
extern load_idt
extern load_gdt
extern restart_cold
extern puts
extern configure_pic
extern enter_userspace

kernel_entry:

	mov r15, rdi					;	preserve parameters

	mov esp, stack_end				;	reconfigure the stack
	xor ebp, ebp

	; ensure we are in long mode
	; and replace the UEFI-owned
	; existing GDT and IDT, ofc

	; interrupts

	cli								;	clear the interrupt flag

	; gdt
	call load_gdt

;	mov edi, 0x0000FF00
;	call set_status_color

	; idt
	call load_idt

;	mov edi, 0xFF00FF00
;	call set_status_color

	call configure_pic

	; interrupts
	sti								;	set the interrupt flag

;	mov edi, 0x00FFFF00
;	call set_status_color

	mov rdi, r15					;	bring back original rdi

	call kernel_main				;	call kernel

	jmp enter_userspace

set_status_color:
	push r14
	mov r14, [asm_framebuffer_ptr]
%rep 200
	mov [r14], edi
	add r14, 4
%endrep
	pop r14
	ret

section .bss

stack_begin:
	resb 16384 						; 	16 KiB
stack_end:

section .data

asm_framebuffer_ptr:
	resb 8

asm_framebuffer_x_res:
	resb 8

asm_framebuffer_y_res:
	resb 8
