#include "console.h"

typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned int uintptr_t;

extern short CODE_SEG;
extern int gdt_descriptor;
extern int DATA_SEG;
extern void loadGDT(void);

typedef struct {
	uint16_t    isr_low;      // The lower 16 bits of the ISR's address
	uint16_t   kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t     reserved;     // Set to zero
	uint8_t     attributes;   // Type and attributes; see the IDT page
	uint16_t    isr_high;     // The higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry_t;

__attribute__((aligned(0x10)))
static idt_entry_t idt[256]; // Create an array of IDT entries; aligned for performance
		
typedef struct {
	uint16_t	limit;
	uint32_t	base;
} __attribute__((packed)) idtr_t;

static idtr_t idtr;

static inline void lcr3(unsigned int val)
{
  asm volatile("movl %0,%%cr3" : : "r" (val));
}

extern void isr_wrapper_no_err_code(void);
extern void isr_wrapper_push_err_code(void);

void isr(void) {
	printk("in isr\n");
}

// 8MB mapping
int pagetable[2][1024] __attribute__((aligned(4096)));
int pagedirectory[2] __attribute__((aligned(4096)));

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_low        = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs      = 0x08; //CODE_SEG; // this value can be whatever offset your kernel code selector is in your GDT
    descriptor->attributes     = flags;
    descriptor->isr_high       = (uint32_t)isr >> 16;
    descriptor->reserved       = 0;
}

static inline void halt(void)
{
    asm volatile("hlt" : : );
}


void idt_init() {
    idtr.base = &idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * 256 - 1;

    for (uint16_t vector = 0; vector < 256; vector++) {
        idt_set_descriptor(vector, isr_wrapper_no_err_code, 0x8E);
    }
    
    idt_set_descriptor(0x08, isr_wrapper_push_err_code, 0x8E);
    idt_set_descriptor(0x0a, isr_wrapper_push_err_code, 0x8E);
    idt_set_descriptor(0x0b, isr_wrapper_push_err_code, 0x8E);
    idt_set_descriptor(0x0c, isr_wrapper_push_err_code, 0x8E);
    idt_set_descriptor(0x0d, isr_wrapper_push_err_code, 0x8E);
    idt_set_descriptor(0x0e, isr_wrapper_push_err_code, 0x8E);
    idt_set_descriptor(0x11, isr_wrapper_push_err_code, 0x8E);

    __asm__ volatile ("lidt %0" : : "m"(idtr)); // load the new IDT
}

int main(void)
{
    int i;
        int j;
        int addr=0;

    // Create your page table here
        for(i=0;i<2;i++){
                for(j=0;j<1024;j++){
                        pagetable[i][j]=addr|0b11;
                        addr +=4096;
                }
                pagedirectory[i]=(unsigned int )(&pagetable[i])|0b11;
        }

    

    lcr3((unsigned int)(&pagedirectory));
       loadGDT();
	idt_init();

	uartinit();
	printk("hi");
    __asm__ volatile ("sti");
    int *bad = (int *)0xDEADBEEF;
int val = *bad; // page fault isr return will try to dereference again, causing infinite page fault
    while (1) {
    halt();
    }
}


