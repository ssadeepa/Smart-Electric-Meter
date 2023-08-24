#define F_CPU 16000000UL
#define sampling 300
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define BAUD 9600
#define UBBR_VALUE ((F_CPU/16/BAUD)-1)

#include "lcd.h"
#include <math.h>

void initHardware(void);
void configFreeRunningADC1(uint16_t channel);
//void configFreeRunningADC2(void);
void display(int value1, int value2);
void ADC3val(void);
void ADC2val(void);
int prevADC2Value=0;
int prevADC1Value=0;

int volt,current;
int k=0;

volatile int adcValue1 = 0;
volatile int adcValue2 = 0;
volatile int adcValueChanged = 0; // Flag to indicate if the ADC value has changed
int x=0;
int y=0;
double max1=0;
double max3=0;
int d=0;

int Button(void){return (PIND & 0B00000001);}

/*int processADCValues(int *array, int size) {
	int sum_squares = 0;
	for (int i = 0; i < size; i++) {
		sum_squares += (int)array[i] * array[i];
	}

	int mean_square = (int)sum_squares /size ;
	int rms = sqrt(mean_square);
	return rms;
}*/

void initUSART(void)
{
	UBRR0H = (unsigned char)(UBBR_VALUE>>8);
	UBRR0L = (unsigned char)(UBBR_VALUE);
	UCSR0B = (1<<TXEN0);
	UCSR0C = (3<<UCSZ00);
	sei();
}

void transmitUSART(unsigned char data)
{
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 =data;
}


int main(void)
{
	initHardware();
	Lcd_init();
	LcdDataBusDirnReg = 0xFF;
	//configFreeRunningADC2();
	configFreeRunningADC1(0x02);
	configFreeRunningADC1(0x03);
	initUSART();
	
	while (1)
	{
		// Display the updated ADC value only if it has changed
		if (adcValueChanged)
		{
			display(volt,current);
			if(Button()){
				PORTD |= 0B00000010;
			}
			else{
				PORTD &= 0B11111101;
			}
			
			adcValueChanged=0;
			
		}

		
	}
}

void initHardware(void)
{
	DDRD &= 0B11111110;
	DDRD |= 0B00000010;
	Lcd_PortConfig();
}

void configFreeRunningADC1(uint16_t channel)
{
	ADMUX = (ADMUX & 0x0F) | channel; // Channel 03 (ADC3)
	ADCSRB = 0B00000000;
	ADCSRA = 0B11101111; // Enable ADC and set auto-triggering, use prescaler 128
	ADCSRA |= (1 << ADATE); // Enable auto-triggering (free-running mode)
	ADCSRA |= (1 << ADIE); // Enable ADC interrupt
	ADCSRA |= (1 << ADSC); // Start the first ADC conversion
	sei();
}

void display(int adc3Value, int adc2Value)
{
	int index = 0;
	
	if (k == 0) {
		setCursorLine(&index);
		Lcd_CmdWrite(0x80 + 0);
		Lcd_DataWrite('V');
		Lcd_DataWrite('r');
		Lcd_DataWrite('m');
		Lcd_DataWrite('s');
		Lcd_DataWrite('=');
		
		Lcd_CmdWrite(0xc0 + 0);
		Lcd_DataWrite('I');
		Lcd_DataWrite('r');
		Lcd_DataWrite('m');
		Lcd_DataWrite('s');
		Lcd_DataWrite('=');
		
		
	}

	index = 6;
	setCursorLine(&index);
	Lcd_CmdWrite(0x80 + 6);
	// Update the new adc2Value
	int value = adc2Value;
	for (int i = 1000; i > 0; i /= 10) {
		int digit = value / i;
		Lcd_DataWrite(digit + '0');
		transmitUSART(digit + '0');
		value %= i;
	}

	// Keep cursor at position 22
	index = 22;
	setCursorLine(&index);
	Lcd_CmdWrite(0xC0 + 6);
	// Update the new adc3Value
	value = adc3Value;
	for (int i = 1000; i > 0; i /= 10) {
		int digit = value / i;
		Lcd_DataWrite(digit + '0');
		transmitUSART(digit + '0');
		value %= i;
	}

	k = 1;
}

ISR(ADC_vect)
{
	if ((ADMUX & 0x0F) == 0x03)
	{
		ADC3val();
		configFreeRunningADC1(0x02); // Configure ADC2 after ADC3 conversion
	}
	else if ((ADMUX & 0x0F) == 0x02)
	{
		ADC2val();
		configFreeRunningADC1(0x03); // Configure ADC3 after ADC2 conversion
	}
	
}

void ADC3val()
{
	adcValue1 = ADC; // Read ADC3 value
	if((x>sampling)){
		x=0;
		if ((max1> prevADC1Value+2)||(max1< prevADC1Value-2))
		{
			adcValueChanged = 1; // Update the previous ADC3 value
			current = max1/sqrt(2);
		}
		
		prevADC1Value = max1;
		max1=0;
	}
	else{
		
		if(adcValue1>max1){
			max1=adcValue1;
		}
		x++;
		
	}
	
	ADMUX = (ADMUX & 0xF0) | 0x02; // Switch to ADC2 for the next conversion
}


void ADC2val()
{
	adcValue2 = ADC; // Read ADC2 value
	if(y>=sampling ){
		y=0;
		if((max3 >prevADC2Value+1)||((max3 <prevADC2Value-1))){
			adcValueChanged = 1;
			volt=max3/sqrt(2);
		}
		prevADC2Value =max3;
		max3 =0;
	}
	else{
		if(adcValue2>max3){
			max3=adcValue2;
		}
		y++;
	}
	ADCSRA |= (1 << ADSC); // Start the next ADC conversion
}