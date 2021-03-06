/*****************************************************
Project : ACU Driver
Date    : 31/05/2022
Author  : Le Duy Quoc
Company : HCMC Nong Lam University

Chip type               : ATmega16
Program type            : Application
AVR Core Clock frequency: 8.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 256
*****************************************************/

#include <mega16.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <delay.h>
#include <alcd.h>
/******************************************************************************/
/*---------------------------------DEFINE-------------------------------------*/

#define ACCELERATE_THRESHOLD    500
#define CRASH_THRESHOLD         500
#define VACC					5.0f
#define ADC_VREF_TYPE           0x40 

#define AIRBAG_LED              PORTB.0
#define SEATBELT_LED            PORTB.1
#define SEATBELT                PIND.2
#define LED						PORTB.2
#define BUZZER					PORTA.2
/******************************************************************************/
/*----------------------------DECLARE FUNCTION--------------------------------*/
/*kHOI TAO CAC KHOI NGOAI VI*/
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
/******************************************************************************/
/*----------------------------DECLARE GLOBAL VAR------------------------------*/

volatile unsigned char flag = 0;	// trang thai seatbelt
unsigned char employ = 0;	    // trang thai tui khi
unsigned int crash_value, accelerate_value;
char buffer[16];

/******************************************************************************/
/*-------------------------------MAIN FUNCTION--------------------------------*/

void main(void)
{ 
	unsigned int i;
    // Alphanumeric LCD initialization
	// RS - PORTC Bit 0
	// RD - PORTC Bit 1
	// EN - PORTC Bit 2
	// D4 - PORTC Bit 4
	// D5 - PORTC Bit 5
	// D6 - PORTC Bit 6
	// D7 - PORTC Bit 7
	// Characters/line: 16

	lcd_init(16);
	init_GPIO();
	init_ADC();
	init_ExtInt();
	init_UART();	
	
	// Global enable interrupts
	#asm("sei")  
    
            lcd_gotoxy(2, 0);
            lcd_puts("supplemental");
            lcd_gotoxy(0, 1);
            lcd_puts("restraint system");
            delay_ms(1000);
            for (i = 0; i <= 6; i++)
            {
                AIRBAG_LED = 0;
                delay_ms(100);
                AIRBAG_LED = 1;
                delay_ms(100);
            }
            
            lcd_clear();
            lcd_gotoxy(0, 0);
            lcd_puts("Seatbelt: OFF");    

	while (1)
	      { 
            /* Doc gia tri cac cam bien */   
            read_Sensor(); 
            
            /* Khi seatbelt ON */
            if(flag == 1)
            {
                /*lcd_gotoxy(0, 0);
                lcd_puts("Seatbelt:  ON"); */
				
				SEATBELT_LED = 1;
                                                   
                
                /* Khi seatbelt ON va cac cam bien dat gia tri nguong kich hoat tui khi */
                if((crash_value >= CRASH_THRESHOLD) && (accelerate_value >= ACCELERATE_THRESHOLD))
                {
                    airbag_employ();

                    lcd_gotoxy(0, 1);
                    lcd_puts("Airbag  Employed");
                    
                    /*Sau khi tui khi no, gia tri cam bien thap hon nguong hoac thao seatbelt, acu van bao tui khi da no*/
                    while(employ == 1)
                    {
                        if(flag == 0)
                        {
                            SEATBELT_LED = 0;
                            delay_ms(1000);
                            SEATBELT_LED = 1;
                            delay_ms(1000); 
                            
                            /*lcd_gotoxy(0, 0);
                            lcd_puts("Seatbelt: OFF");  */
                        }
                        else if(flag == 1)
                        { 
                            SEATBELT_LED = 1;
                           /* lcd_gotoxy(0, 0);
                            lcd_puts("Seatbelt:  ON"); */
                        }
                        
                    }
                }
                
                /* Khi seatbelt ON, nhung cac cam bien van chua dat gia tri nguong*/ 
                else
                {
                    read_Sensor();
                }
            }
            
            /* Khi seatbelt OFF */ 
                                  
            else
            { 
                read_Sensor();
                /*lcd_gotoxy(0, 0);
                lcd_puts("Seatbelt: OFF");*/
                
                SEATBELT_LED = 0;
                delay_ms(1000);
                SEATBELT_LED = 1;  
                delay_ms(1000);
                
                /* Khi seatbelt OFF, cac cam bien dat gia tri nguong */
                if((crash_value >= CRASH_THRESHOLD) && (accelerate_value >= ACCELERATE_THRESHOLD))
                {
                    read_Sensor();
                    while(PIND.2 == 0)
                    {
                        read_Sensor();
                        /*lcd_gotoxy(0, 0);
                        lcd_puts("Seatbelt: OFF");*/
                    }
                }
                /* Khi seatbelt OFF, gia tri cac cam bien thap hon nguong */
                else
                {
                    read_Sensor();
                } 
            }                  
	      } 
}

/******************************************************************************/
/*-------------------------INTERRUPT SERVICE ROUTINE--------------------------*/

// External Interrupt 0 service routine
interrupt [EXT_INT0] void ext_int0_isr(void)
{
    //delay_ms(200);
    flag = 1 - flag;
    print_lcd(flag);
    
    LED = 0;
    delay_ms(20);
    LED = 1;
}

/******************************************************************************/
/*---------------------------DEFINITION FUNCTION------------------------------*/

static inline void init_GPIO()
{
	// Input/Output Ports initialization
	PORTA = (1<<2);
	DDRA  = (1<<2);
	
	PORTB = (1<<PORTB0)|(1<<PORTB1)|(1<<PORTB2);
	DDRB  = 0xFF;
	 
	PORTC = 0x00;
	DDRC  = 0xFF;
	 
	PORTD = 0x00;
	DDRD  = (1<<DDD4)|(1<<DDD5);
}

static inline void init_ADC()
{
	// ADC initialization

	/* ADC Voltage Reference: AVCC pin*/    
	ADMUX = ADC_VREF_TYPE & 0xff; 
    /*ADC Enable, Pre-scaler 8: 1.000.000 kHz*/
	ADCSRA = (1<<ADEN)|(1<<ADATE)|(1<<ADPS0)|(1<<ADPS2);
    /*Free Running mode*/
	SFIOR &= 0x00;
}

static inline void init_Timer0_PWM()
{
	/*
		duty cycle = OCR/256
		T=256*(Tclk_cpu/prescaler)
	*/
	
	// Timer/Counter 0 initialization
	/*Fast PWM mode, top=0xFF, OC0 output: non-inverted, no-prescaler */
	TCCR0 = (1<<WGM00)|(1<<WGM01);
	TCNT0 = 0x00;
	OCR0  = 127;
	
	// Timer(s)/Counter(s) Interrupt(s) initialization
	TIMSK=0x00;
}


static inline void init_Timer1_PWM()
{
	/*
		duty cycle = OCR/256
		T=256*(Tclk_cpu/prescaler)
	*/
	
	// Timer/Counter 1 initialization
	/*Fast PWM mode, top=0x3FF, OC1A-OC1B output: non-inverted, no-prescaler */
	TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<WGM11)|(1<<WGM10);
	TCCR1B = (1<<WGM12)|(1<<CS10);
	TCNT1H = 0x00;
	TCNT1L = 0x00;
	ICR1H  = 0x00;
	ICR1L  = 0x00;
	
	OCR1A = 10;
	OCR1B = 10;
	
	// Timer(s)/Counter(s) Interrupt(s) initialization
	TIMSK=0x00;
}

static inline void init_ExtInt()
{
	// External Interrupt(s) initialization
	/*INT0 Enable*/
	GICR  |= (1<<INT0);
	/*falling Mode*/ 
	MCUCR  = (1 << ISC01);
	MCUCSR = 0x00;
	GIFR   = (1<<INTF0); 
}

static inline void init_UART()
{
	// USART initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART Receiver: On
	// USART Transmitter: On
	// USART Mode: Asynchronous
	// USART Baud Rate: 9600
	UCSRA=0x00;
	UCSRB=0x18;
	UCSRC=0x86;
	UBRRH=0x00;
	UBRRL=0x33;
}

/******************************************************************************/
/*
float ADC_to_Voltage(unsigned int adc_value)
{
	// ADC 10-bit
	float voltage = 0.0;
	voltage = VACC * adc_value / 1024;
	
	return voltage; 
}*/

unsigned int read_adc(unsigned char adc_input)
{
	ADMUX=adc_input | (ADC_VREF_TYPE & 0xff);
	// Delay needed for the stabilization of the ADC input voltage
	delay_us(10);
	// Start the AD conversion
	ADCSRA|=0x40;
	// Wait for the AD conversion to complete
	while ((ADCSRA & 0x10)==0);
	ADCSRA|=0x10;
	return ADCW;
}

void airbag_employ()
{
	AIRBAG_LED = 0;    
    BUZZER = 0;
    init_Timer0_PWM();	// xung tui khi da no
    init_Timer1_PWM();  // xung tin hieu tai nan gui ve hop BCM
    employ = 1;
}

void read_Sensor()
{
    crash_value = read_adc(0);
    accelerate_value = read_adc(1);
    //printf("impact: %d     accelerate: %d\n\r",crash_value, accelerate_value);
    sprintf(buffer,"ss1:%4dss2:%4d\n\r", crash_value, accelerate_value);
    printf("%s\n\r",buffer); 
    lcd_gotoxy(0,1);
    lcd_puts(buffer);    
}

void print_lcd(unsigned int flag)
{
    if(flag == 1)
    {
        //lcd_clear();
        lcd_gotoxy(0, 0);
        lcd_puts("Seatbelt:  ON");
    }
    else if(flag == 0)
    {
        //lcd_clear();
        lcd_gotoxy(0, 0);
        lcd_puts("Seatbelt: OFF");
    }
}