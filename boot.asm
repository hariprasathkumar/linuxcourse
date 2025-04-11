global start
global CODE_SEG
global isr_asm
global DATA_SEG
global loadGDT
global isr_wrapper_push_err_code
global isr_wrapper_no_err_code
global enablePaging
extern main
extern isr

section .text
bits 32
start:
    mov esp, stack_top

    call main

    hlt


loadGDT:
    pushad
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    ; Set our own GDT, can't rely GDT register being valid after bootloader
    ; transfers control to our entry point
    lgdt [gdt_descriptor]         ; Load GDT Register with GDT record
    mov eax, DATA_SEG
    mov ds, eax         ; Reload all the data descriptors with Data selector (2nd argument)
    mov es, eax
    mov gs, eax
    mov fs, eax
    mov ss, eax

    jmp CODE_SEG:.setcs
                        ; Do the FAR JMP to next instruction to set CS with Code selector, and
                        ;    set the EIP (instruction pointer) to offset of setcs
.setcs:
    popad
    ret

align 4
isr_wrapper_push_err_code:
    add esp, 4
    pushad
    call isr
    popad
    iret

align 4
isr_wrapper_no_err_code:
     pushad
     call isr
     popad
     iret

section .bss

align 4096

stack_bottom:
    resb 4096 ; Reserve this many bytes
stack_top:
	
	
section .rodata
gdt_start:
    dq 0x0000000000000000
gdt_code:
    dq 0x00CF9A000000FFFF
gdt_data:
    dq 0x00CF92000000FFFF
gdt_end:

; GDT descriptor
gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; size (16 bit), always one less of its true size
    dd gdt_start ; address (32 bit)

; define some constants for later use
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

