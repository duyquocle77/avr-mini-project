
#pragma used+
sfrb TWBR=0;
sfrb TWSR=1;
sfrb TWAR=2;
sfrb TWDR=3;
sfrb ADCL=4;
sfrb ADCH=5;
sfrw ADCW=4;      
sfrb ADCSRA=6;
sfrb ADMUX=7;
sfrb ACSR=8;
sfrb UBRRL=9;
sfrb UCSRB=0xa;
sfrb UCSRA=0xb;
sfrb UDR=0xc;
sfrb SPCR=0xd;
sfrb SPSR=0xe;
sfrb SPDR=0xf;
sfrb PIND=0x10;
sfrb DDRD=0x11;
sfrb PORTD=0x12;
sfrb PINC=0x13;
sfrb DDRC=0x14;
sfrb PORTC=0x15;
sfrb PINB=0x16;
sfrb DDRB=0x17;
sfrb PORTB=0x18;
sfrb PINA=0x19;
sfrb DDRA=0x1a;
sfrb PORTA=0x1b;
sfrb EECR=0x1c;
sfrb EEDR=0x1d;
sfrb EEARL=0x1e;
sfrb EEARH=0x1f;
sfrw EEAR=0x1e;   
sfrb UBRRH=0x20;
sfrb UCSRC=0X20;
sfrb WDTCR=0x21;
sfrb ASSR=0x22;
sfrb OCR2=0x23;
sfrb TCNT2=0x24;
sfrb TCCR2=0x25;
sfrb ICR1L=0x26;
sfrb ICR1H=0x27;
sfrb OCR1BL=0x28;
sfrb OCR1BH=0x29;
sfrw OCR1B=0x28;  
sfrb OCR1AL=0x2a;
sfrb OCR1AH=0x2b;
sfrw OCR1A=0x2a;  
sfrb TCNT1L=0x2c;
sfrb TCNT1H=0x2d;
sfrw TCNT1=0x2c;  
sfrb TCCR1B=0x2e;
sfrb TCCR1A=0x2f;
sfrb SFIOR=0x30;
sfrb OSCCAL=0x31;
sfrb OCDR=0x31;
sfrb TCNT0=0x32;
sfrb TCCR0=0x33;
sfrb MCUCSR=0x34;
sfrb MCUCR=0x35;
sfrb TWCR=0x36;
sfrb SPMCR=0x37;
sfrb TIFR=0x38;
sfrb TIMSK=0x39;
sfrb GIFR=0x3a;
sfrb GICR=0x3b;
sfrb OCR0=0X3c;
sfrb SPL=0x3d;
sfrb SPH=0x3e;
sfrb SREG=0x3f;
#pragma used-

#asm
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x40
	.EQU __sm_mask=0xB0
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0xA0
	.EQU __sm_ext_standby=0xB0
	.EQU __sm_adc_noise_red=0x10
	.SET power_ctrl_reg=mcucr
	#endif
#endasm

typedef char *va_list;

#pragma used+

char getchar(void);
void putchar(char c);
void puts(char *str);
void putsf(char flash *str);
int printf(char flash *fmtstr,...);
int sprintf(char *str, char flash *fmtstr,...);
int vprintf(char flash * fmtstr, va_list argptr);
int vsprintf(char *str, char flash * fmtstr, va_list argptr);

char *gets(char *str,unsigned int len);
int snprintf(char *str, unsigned int size, char flash *fmtstr,...);
int vsnprintf(char *str, unsigned int size, char flash * fmtstr, va_list argptr);

int scanf(char flash *fmtstr,...);
int sscanf(char *str, char flash *fmtstr,...);

#pragma used-

#pragma library stdio.lib

#pragma used+

unsigned char cabs(signed char x);
unsigned int abs(int x);
unsigned long labs(long x);
float fabs(float x);
int atoi(char *str);
long int atol(char *str);
float atof(char *str);
void itoa(int n,char *str);
void ltoa(long int n,char *str);
void ftoa(float n,unsigned char decimals,char *str);
void ftoe(float n,unsigned char decimals,char *str);
void srand(int seed);
int rand(void);
void *malloc(unsigned int size);
void *calloc(unsigned int num, unsigned int size);
void *realloc(void *ptr, unsigned int size); 
void free(void *ptr);

#pragma used-
#pragma library stdlib.lib

#pragma used+

void delay_us(unsigned int n);
void delay_ms(unsigned int n);

#pragma used-

void _lcd_write_data(unsigned char data);

unsigned char lcd_read_byte(unsigned char addr);

void lcd_write_byte(unsigned char addr, unsigned char data);

void lcd_gotoxy(unsigned char x, unsigned char y);

void lcd_clear(void);
void lcd_putchar(char c);

void lcd_puts(char *str);

void lcd_putsf(char flash *str);

void lcd_putse(char eeprom *str);

void lcd_init(unsigned char lcd_columns);

#pragma library alcd.lib

static inline void init_GPIO();
static inline void init_ADC();
static inline void init_Timer0_PWM();
static inline void init_Timer1_PWM();
static inline void init_ExtInt();
static inline void init_UART();

unsigned int read_adc(unsigned char adc_input);
void airbag_employ();
void read_Sensor();
void print_lcd(unsigned int flag);

volatile unsigned char flag = 0;	
unsigned char employ = 0;	    
unsigned int crash_value, accelerate_value;
char buffer[16];

void main(void)
{ 
unsigned int i;

lcd_init(16);
init_GPIO();
init_ADC();
init_ExtInt();
init_UART();	

#asm("sei")  

lcd_gotoxy(2, 0);
lcd_puts("supplemental");
lcd_gotoxy(0, 1);
lcd_puts("restraint system");
delay_ms(1000);
for (i = 0; i <= 6; i++)
{
PORTB.0 = 0;
delay_ms(100);
PORTB.0 = 1;
delay_ms(100);
}

lcd_clear();
lcd_gotoxy(0, 0);
lcd_puts("Seatbelt: OFF");    

while (1)
{ 

read_Sensor(); 

if(flag == 1)
{

PORTB.1 = 1;

if((crash_value >= 500) && (accelerate_value >= 500))
{
airbag_employ();

lcd_gotoxy(0, 1);
lcd_puts("Airbag  Employed");

while(employ == 1)
{
if(flag == 0)
{
PORTB.1 = 0;
delay_ms(1000);
PORTB.1 = 1;
delay_ms(1000); 

}
else if(flag == 1)
{ 
PORTB.1 = 1;

}

}
}

else
{
read_Sensor();
}
}

else
{ 
read_Sensor();

PORTB.1 = 0;
delay_ms(1000);
PORTB.1 = 1;  
delay_ms(1000);

if((crash_value >= 500) && (accelerate_value >= 500))
{
read_Sensor();
while(PIND.2 == 0)
{
read_Sensor();

}
}

else
{
read_Sensor();
} 
}                  
} 
}

interrupt [2] void ext_int0_isr(void)
{

flag = 1 - flag;
print_lcd(flag);

PORTB.2 = 0;
delay_ms(20);
PORTB.2 = 1;
}

static inline void init_GPIO()
{

PORTA = (1<<2);
DDRA  = (1<<2);

PORTB = (1<<0       )|(1<<1       )|(1<<2       );
DDRB  = 0xFF;

PORTC = 0x00;
DDRC  = 0xFF;

PORTD = 0x00;
DDRD  = (1<<4       )|(1<<5       );
}

static inline void init_ADC()
{

ADMUX = 0x40  & 0xff; 

ADCSRA = (1<<7       )|(1<<5       )|(1<<0       )|(1<<2       );

SFIOR &= 0x00;
}

static inline void init_Timer0_PWM()
{

TCCR0 = (1<<6       )|(1<<3       );
TCNT0 = 0x00;
OCR0  = 127;

TIMSK=0x00;
}

static inline void init_Timer1_PWM()
{

TCCR1A = (1<<7       )|(1<<5       )|(1<<1       )|(1<<0       );
TCCR1B = (1<<3       )|(1<<0       );
TCNT1H = 0x00;
TCNT1L = 0x00;
ICR1H  = 0x00;
ICR1L  = 0x00;

OCR1A = 10;
OCR1B = 10;

TIMSK=0x00;
}

static inline void init_ExtInt()
{

GICR  |= (1<<6       );

MCUCR  = (1 << 1       );
MCUCSR = 0x00;
GIFR   = (1<<6       ); 
}

static inline void init_UART()
{

UCSRA=0x00;
UCSRB=0x18;
UCSRC=0x86;
UBRRH=0x00;
UBRRL=0x33;
}

unsigned int read_adc(unsigned char adc_input)
{
ADMUX=adc_input | (0x40  & 0xff);

delay_us(10);

ADCSRA|=0x40;

while ((ADCSRA & 0x10)==0);
ADCSRA|=0x10;
return ADCW;
}

void airbag_employ()
{
PORTB.0 = 0;    
PORTA.2 = 0;
init_Timer0_PWM();	
init_Timer1_PWM();  
employ = 1;
}

void read_Sensor()
{
crash_value = read_adc(0);
accelerate_value = read_adc(1);

sprintf(buffer,"ss1:%4dss2:%4d\n\r", crash_value, accelerate_value);
printf("%s\n\r",buffer); 
lcd_gotoxy(0,1);
lcd_puts(buffer);    
}

void print_lcd(unsigned int flag)
{
if(flag == 1)
{

lcd_gotoxy(0, 0);
lcd_puts("Seatbelt:  ON");
}
else if(flag == 0)
{

lcd_gotoxy(0, 0);
lcd_puts("Seatbelt: OFF");
}
}
