/*
 * rotary_encoder.c
 *
 * Created: 21.11.2018 14:52:56
 * Author : Marius
 */ 
#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "usitwislave.h"

#define 	SLAVE_ADR_ATTINY_1       0x20		//slave address wird noch um 1 bit nach links geschoben wgen r/w adresse ist also 0x10

int8_t last=0;           
int8_t enc_value;
//const int8_t rotary_table[16] PROGMEM = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};	vierfache präzision
//const int8_t rotary_table[16] PROGMEM = {0,0,-1,0,0,0,0,1,1,0,0,0,0,-1,0,0};		doppelte präzisio
const int8_t rotary_table[16] PROGMEM = {0,0,0,0,0,0,0,-1,0,0,0,1,0,0,0,0}; 
	
volatile char timer_flag;
volatile int8_t intern_val=0;
volatile char counter=0;
volatile int8_t intern_val_tmp=0;

void init_rotary(void){
		
		DDRB &= ~(1<<DDB4);	//Pin 5 Input
		DDRB &= ~(1<<DDB3);	//Pin 3 Input
		PORTB |= (1<<PB4); //Pullup Pin 5
		PORTB |= (1<<PB3); //Pullup Pin 3
		
		DDRB  |= (1<<DDB1);  //Pin 5 Output
		PORTB |= (1<<PB1);
		
	
}

int8_t encode_read( void )         // Encoder auslesen
{
	int8_t val;
	cli();
	val = enc_value;
	enc_value = 0;
	sei();
	return val;
}
void init_timer(void){
	TCCR0A = (1<<WGM01);
	TCCR0B = (1<<CS02);
	OCR0A = 63;
	TIMSK = (1<<OCIE0A);
	
	
}

void init_timer_2(void){
	
	TCCR1 = (1<<CTC1)|(1<<CS13)|(1<<CS12)|(1<<CS10);
	OCR1A = 180;
	timer_flag=0;
	//TIMSK |= (1<<OCIE1A);
	
}


int main(void)
{
	
	typedef enum
	{
		CHECK_VALUE      =		0x00,
		SINGLE_PRECISION =		0x01,
		DOUBLE_PRECISION =		0x02
		
	} rotary_state_t;
	
	
	
	cli();
		init_rotary();
		init_timer();
		init_timer_2();
		usiTwiSlaveInit(SLAVE_ADR_ATTINY_1);	
		sei();
	


	
	
	
	int8_t tmp;
	int8_t tmp_internval;
	int8_t value=0;
	int8_t max_value=255; 
	int8_t min_value=0;
	int8_t init_value;
	
	
	rotary_state_t rotary_state=CHECK_VALUE;
	
	
    txbuffer[3]=max_value;
	txbuffer[2]=min_value;
	
	
	
	
    while (1) 
    {	
		
		
				switch(rotary_state)
				{
					case CHECK_VALUE :
					
					tmp=rxbuffer[4];
					init_value=rxbuffer[5];
					
					if (value == tmp && init_value==0)
					{
						max_value=rxbuffer[3];
						min_value=rxbuffer[2];
						
						rotary_state=SINGLE_PRECISION;
						break;
					}
					
					if (value == tmp && init_value==1)
					{
						intern_val=0;
						init_value=0;
						max_value=rxbuffer[3];
						min_value=rxbuffer[2];
						
						rotary_state=SINGLE_PRECISION;
						break;
					}
					
					if ((value != tmp) )
					{
						value=tmp;
						max_value=rxbuffer[3];
						min_value=rxbuffer[2];
						intern_val=0;
						rotary_state=SINGLE_PRECISION;
						break;
					}
					
					
					case SINGLE_PRECISION :
					
					tmp_internval=encode_read();
					intern_val+=tmp_internval;
					
					if (((intern_val+value)>=min_value) && ((intern_val+value)<=max_value))
					{
						
						txbuffer[4]=(intern_val+value);
						rotary_state=CHECK_VALUE;
						
					}
					
					if ((intern_val+value)<(min_value+value))
					{
						
						
						intern_val=min_value;
						txbuffer[4]=(min_value);
						
						rotary_state=CHECK_VALUE;
						
					}
					
						if ((intern_val+value)>(max_value+value))
						{
							
							intern_val=max_value;
							txbuffer[4]=(max_value);
							rotary_state=CHECK_VALUE;
							
						}
					
					break;
				}
				
				/*	
					if (((intern_val+value)<max_value)&&((intern_val+value)>min_value))
					{
						txbuffer[0]=(intern_val+value);
						break;
					}
					if ((intern_val+value)>=max_value)
					{
						txbuffer[0]=(max_value);
						intern_val-=tmp_internval;
						break;
					}
					
					if ((intern_val+value)<=min_value)
					{
						txbuffer[0]=(min_value);
						intern_val+=tmp_internval;
						break;
					}
*/
	
/*
	 _delay_ms(3);
	 tmp_internval=encode_read();
	 
	 value+=tmp_internval;
	 
	 txbuffer[4]=value;
		*/
    
}
	}


ISR( TIMER0_COMPA_vect )
{
	
	static int8_t last=0;           // alten Wert speichern

	last = (last << 2)  & 0x0F;
	if (!(PINB & (1<<PB3))) {last |=2;}
	if (!(PINB & (1<<PB4))) {last |=1;}
		if ((last==7 || last==11) && timer_flag==0)
		{
			PORTB &= ~(1<<PB1);
			TIMSK |= (1<<OCIE1A);
			timer_flag=1;
			enc_value += pgm_read_byte(&rotary_table[last]);
			
		}
		else
		{
			enc_value += pgm_read_byte(&rotary_table[last]);
		}
	
	
}

ISR( TIMER1_COMPA_vect )
{	
	PORTB ^= (1<<PB1);
	
	if (intern_val==intern_val_tmp)				
	{
		
		counter++;
		PORTB &= ~(1<<PB1);
	}
	
	else							//(intern_val_tmp!=intern_val)
	{
		intern_val_tmp=intern_val;		
		counter=0;
		PORTB &= ~(1<<PB1);
	} 
	
	if (counter==5)
	{	
		counter=0;
		PORTB |= (1<<PB1);
		timer_flag=0;
		TIMSK &=~(1<<OCIE1A);
		
		
	}
	

	
}