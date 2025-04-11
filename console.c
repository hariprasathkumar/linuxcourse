#define COM1    0x3f8
#define VIDEOMEMORY 0xB8000  

static int uart;    // is there a uart?
unsigned short* videocurr=(unsigned short*)VIDEOMEMORY;
unsigned short videox=0;
unsigned short videoy=0;

void consoleputc(int c);

void microdelay(unsigned long us) {

}

static inline unsigned char inb(unsigned short port)
{
    unsigned char data;

    asm volatile("in %1,%0" : "=a" (data) : "d" (port));
    return data;
}

static inline void outb(unsigned short port, unsigned char data)
{
    asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

void uartinit(void)
{

  // Turn off the FIFO
  outb(COM1+2, 0);

  // 9600 baud, 8 data bits, 1 stop bit, parity off.
  outb(COM1+3, 0x80);    // Unlock divisor
  outb(COM1+0, 115200/115200);
  outb(COM1+1, 0);
  outb(COM1+3, 0x03);    // Lock divisor, 8 data bits.
  outb(COM1+4, 0);
  outb(COM1+1, 0x01);    // Enable receive interrupts.

  // If status is 0xFF, no serial port.
  if(inb(COM1+5) == 0xFF)
      return;

  uart = 1;

  // Acknowledge pre-existing interrupt conditions;
  // enable interrupts.
  inb(COM1+2);
  inb(COM1+0);
}

void
uartputc(int c)
{
  int i;

  if(!uart)
      return;
  
  for(i = 0; i < 128 && !(inb(COM1+5) & 0x20); i++)
      microdelay(10);

  outb(COM1+0, c);
}


// Print to the console
void printk(char *str)
{
    int i, c;

    for(i = 0; (c = str[i]) != 0; i++){
        uartputc(c);
		consoleputc(c);
    }
}

// clear screen function
void clrscr(void)
{	
	// clear screen attributes
	unsigned short ch = 0x0F20;
	// temporary variable
	int i;
	// points to start of display
	videocurr=(unsigned short *)VIDEOMEMORY;
	for (i = 0; i < 80*25; i++)
	{
	   videocurr[i] = ch;
	}
	//modify position of x and y
	videox=0;
	videoy=0;
}

// print character of screen
void consoleputc(int c)
{
	// extracts character 
	char data=(char)c;
	// add foreground and background information
	unsigned short ch=(0x0F<<8)|data;
	// calculate position
	unsigned short pos;
	// handle \n
	if('\n'==c){
		videox=0;
		videoy++;
	}else{
		//else print character
		pos=videoy*80+videox;
		*(videocurr+pos)=ch;
		videox++;
	}
	//handle overflow condition
	if(videox>=80){
		videox=0;
		videoy++;
	}
	
	if(videoy>= 25){
		videoy=0;
		videox=0;
		clrscr();
	}

}

// initialize console
void console_init(void)
{
	// intialize screen
	clrscr();
	// modify x and y screen pointer
	videox=0;
	videoy=0;	
}




