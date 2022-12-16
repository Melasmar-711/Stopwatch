/*
 * exc.c
 *
 *  Created on: Sep 15, 2021
 *      Author: Mahmoud EL-Asmar
 */


#include<avr/io.h>
#include <avr/interrupt.h>
#include<util/delay.h>

unsigned char seconds_tens=0;
unsigned char seconds_ones=0;
unsigned char minutes_ones=0;
unsigned char minutes_tens=0;
unsigned char hours_ones=0;
unsigned char hours_tens=0;

void Timer1_Init_CTC_Mode(void)
{
	SREG  &=~(1<<7);
	TCNT1 = 0;		//setting initial value of timer one
	OCR1A = 1000;	//setting compare value
	TIMSK |=(1<<OCIE1A);	//using compare mode
	TCCR1A|=(1<<FOC1A); 	//clearing the output compare match flag
	TCCR1B=(1<<WGM12)|(1<<CS12)|(1<<CS10); //using CTC with pre-scaler of 1024
	SREG  |=(1<<7);			//enable the i bit
}
ISR(TIMER1_COMPA_vect)
{
	if(seconds_ones<9)
	{
		//controlling seconds right led incrementing whenever it's not equal to 9
		seconds_ones++;
	}
	else if(seconds_ones==9&&seconds_tens<5)
	{
		//controlling seconds left led incrementing whenever it's not equal to 5 and when right led is equal to 9
		seconds_ones=0;
		seconds_tens++;
	}
	else if(seconds_tens==5&&minutes_ones<9)
	{
		//controlling minutes right led incrementing when it's less than 9 and when seconds left led is =5
		seconds_ones=0;
		seconds_tens=0;
		minutes_ones++;
	}
	else if(seconds_tens==5&&minutes_ones==9&&minutes_tens<5)
	{
		//controlling minutes left led when less than 5 and the second left led is 5 and the minutes right led 9
		seconds_ones=0;
		seconds_tens=0;
		minutes_ones=0;
		minutes_tens++;
	}
	else if((minutes_ones==9)&&(minutes_tens==5)&&(seconds_ones==9)&&(seconds_tens==5)&&(hours_ones<9))
	{
		//controlling hours right led when less than 9 and minutes are 59 and the seconds are 59
		seconds_ones=0;
		seconds_tens=0;
		minutes_ones=0;
		minutes_tens=0;
		hours_ones++;
	}
	if (hours_ones==9)
	{
		seconds_ones=0;
		seconds_tens=0;
		minutes_ones=0;
		minutes_tens=0;
		hours_ones=0;
		hours_tens++;
	}

}

void interupt0_enable(void)
{
	DDRD&=(~(1<<PD2));			// defining pin as input pin
	PORTD|=(1<<PD2); 			//activating internal pull up resistor
	MCUCR|=(1<<ISC01);			//interrupt request at falling edge
	GICR|=(1<<INT0); 			//Enable interrupt 0
	SREG|=(1<<7);
}
ISR(INT0_vect)
{
	//interrupt one service routine to reset the stop watch
	seconds_ones=0;
	seconds_tens=0;
	minutes_ones=0;
	minutes_tens=0;
	hours_ones=0;
	hours_tens=0;
}

void interupt1_enable(void)
{

	DDRD&=(~(1<<PD3));			//defining pin as input pin
	MCUCR|=(1<<ISC11)|(1<<ISC10);	// rising edge interrupt request
	GICR|=(1<<INT1); // enable interrupt one here to avoid the noise in the beginning
	SREG|=(1<<7);
}

ISR(INT1_vect)
{

	TCCR1B&=~(0b00000111);//disable timer one clock

}
void interupt2_enable(void)
{
	DDRB&=(~(1<<PB2));			//defining pi as input pin
	PORTB|=(1<<PB2);			//activating internal pull up resistor
	MCUCSR&=~(1<<ISC2);			// falling edge
	GICR|=(1<<INT2);	 		//enable interrupt 2
	SREG|=(1<<7);
}

ISR(INT2_vect)
{
	//interrupt 2 enabling the timer clock again to continue counting
	TCCR1B|=(0b00000101);//enable timer one with pre-scaler 1024
}


int main()
{
	DDRC|=0b00001111;
	SREG  |= (1<<7);			//Enable i-bit
	DDRA=0b00111111;
	PORTC=0x00;
	PORTA=0xff;
	interupt0_enable();			//activate interrupt 0
	interupt1_enable();			//activate interrupt 1
	interupt2_enable();			//activate interrupt 2
	Timer1_Init_CTC_Mode();		//activate timer one

	while(1)
	{
		PORTA=0b00000001; 	//activate seconds right led
		PORTC=seconds_ones;
		_delay_ms(1);
		PORTA=0b00000010;	// activate seconds left led
		PORTC=seconds_tens;
		_delay_ms(1);
		PORTA=0b00000100;	// activate minutes right led
		PORTC=minutes_ones;
		_delay_ms(1);
		PORTA=0b00001000;	// activate minutes left led
		PORTC=minutes_tens;
		_delay_ms(1);
		PORTA=0b00010000;	//activate hours right led
		PORTC=hours_ones;
		_delay_ms(1);
		PORTA=0b00100000;	// activate hours left led
		PORTC=hours_tens;
		_delay_ms(1);

	}
}
