#include "r_macro.h"
#include "r_spi_if.h"
#include "lcd.h"
#include "uart.h"
#include "sw.h"
#include "timer.h"
#include<stdlib.h>
#include "string.h"
void LCD_Reset(void);
volatile int result;
volatile uint8_t g_time_update_flag;
void config_Led();
char string[20];
char str_transmit_analog[12];
char * string_shown_on_lcd[10];
char number;
//char status_led;
unsigned int led_number;
unsigned int status_led;
unsigned int line;
char text;
void config_Led(){
PM6 = 0x00;
PM4 = 0x00;
PM15 = 0x00;
PM10 = 0x00;
P6 = 0xFC;
P4 = 0x3E;
P15 = 0x04;
P10 = 0x02;
//PM6_bit.no2 = 0;
// PM6_bit.no2 = 0;
}
void control_led(int a, int b)
{
switch(a)
{
case 3:
if(b == 1)
{
P6_bit.no2 = 0;
}else P6_bit.no2 = 1;
break;
case 4:
if(b == 1)
{
P4_bit.no2 = 0;
}else P4_bit.no2 = 1;
break;
case 5:
if(b == 1)
{
P6_bit.no3 = 0;
}else P6_bit.no3 = 1;
break;
case 6:
if(b == 1)
{
P4_bit.no3 = 0;
}else P4_bit.no3 = 1;
break;
case 7:
if(b == 1)
{
P6_bit.no4 = 0;
}else P6_bit.no4 = 1;
break;
case 8:
if(b == 1)
{
P4_bit.no4 = 0;
}else P4_bit.no4 = 1;
break;
case 9:
if(b == 1)
{
P6_bit.no5 = 0;
}else P6_bit.no5 = 1;
break;
case 10:
if(b == 1)
{
P4_bit.no5 = 0;
}else P5_bit.no5 = 1;
break;
case 11:
if(b == 1)
{
P6_bit.no6 = 0;
}else P6_bit.no6 = 1;
break;
case 12:
if(b == 1)
{
P15_bit.no2 = 0;
}else P15_bit.no2 = 1;
break;
case 13:
if(b == 1)
{
P6_bit.no7 = 0;
}else P6_bit.no7 = 1;
break;
case 14:
if(b == 1)
{
P10_bit.no1 = 0;
}else P10_bit.no1 = 1;
break;
case 15:
if(b == 1)
{
P4_bit.no1 = 0;
}else P4_bit.no1 = 1;
break;
default:
break;
}
}
int status_bit(void)
{
	int arr[13];
	int hexadecimal = 0;
	int remainder;
	int i = 0;
	char str1[20];
	if(P6_bit.no2 == 0)
	{
	arr[0] = 1;
	}else arr[0] = 0;

	if(P4_bit.no2 == 0)
	{
	arr[1] = 1;
	}else arr[1] = 0;

	if(P6_bit.no3 == 0)
	{
	arr[2] = 1;
	}else arr[2] = 0;

	if(P4_bit.no3 == 0)
	{
	arr[3] = 1;
	}else arr[3] = 0;

	if(P6_bit.no4 == 0)
	{
	arr[4] = 1;
	}else arr[4] = 0;

	if(P4_bit.no4 == 0)
	{
	arr[5] = 1;
	}else arr[5] = 0;

	if(P6_bit.no5 == 0)
	{
	arr[6] = 1;
	}else arr[6] = 0;

	if(P4_bit.no5 == 0)
	{
	arr[7] = 1;
	}else arr[7] = 0;

	if(P6_bit.no6 == 0)
	{
	arr[8] = 1;
	}else arr[8] = 0;

	if(P15_bit.no2 == 0)
	{
	arr[9] = 1;
	}else arr[9] = 0;

	if(P6_bit.no7 == 0)
	{
	arr[10] = 1;
	}else arr[10] = 0;

	if(P10_bit.no1 == 0)
	{
	arr[11] = 1;
	}else arr[11] = 0;

	if(P4_bit.no1 == 0)
	{
	arr[12] = 1;
	}else arr[12] = 0;

	hexadecimal = arr[0];
	for(i = 1; i < 13; i++)
		{
		hexadecimal += arr[i] * pow(2,i);
		}
	return hexadecimal;
}
void main(void)
{
char string_lcd;
char str1[12];
int i,j;
char *p;
int decimal;
Uart_Init();
R_IT_Create();
R_IT_Start();
INTC_Create();
config_Led();
EI();
LCD_Reset();
/* Initialize SPI channel used for LCD */
R_SPI_Init(SPI_LCD_CHANNEL);
InitialiseLCD();
/* Clear LCD display */
ClearLCD();
/* Display information on the debug LCD.*/
DisplayLCD(LCD_LINE1, (uint8_t *)"UART 9600bps");
DisplayLCD(LCD_LINE2, (uint8_t *)"Interface");

INTC10_Start();


ADCEN = 1; //Start supplying clocks
ADPC = 0x0A; // set mode Analog pin
ADM0 = 0b00110000;//Select A-D conversion time
ADM1 = ADM1 & 0b00011111; // SW trigger mode, Sequential conversion
ADCE = 0;
ADM2 = ADM2 & 0b00011111; //Supplied from VDD, VSS
ADCE = 1;
ADTYP = 0; //10-bit resolution
ADS = 0b00001000; // select ANI8 pin

while (1U)
{

/* Check UART1 receive status */
if( g_time_update_flag>=20)
{
        ADCS = 1;
        result = ADCR >> 6;
        result = (float)result/1023*999;
        ADCR = 0;
        ADCS = 0;
        g_time_update_flag = 0;
        //sprintf(string, "   %d", result); 
        //DisplayLCD(LCD_LINE5 , string);
        sprintf(str_transmit_analog, "$2456,A%d^",result);
        Uart_Transmit(&(str_transmit_analog[0]),11);
        //ClearLCD();
        DisplayLCD(LCD_LINE3 , str_transmit_analog);
      
        sprintf(str_transmit_analog, "$2456,L%04x^",7);
        Uart_Transmit(&(str_transmit_analog[0]),12);
		Uart_Transmit(&(str1[0]), 12);
        control_led(12,1);
      
}
if(status == UART_RECEIVE_DONE)
{
    status = 0;    
    DisplayLCD(LCD_LINE5 , rx_buff);
/* Replace the last element by NULL */
    rx_buff[UART_RX_BUFFER_LEN - 1] = '\0';
    p = strtok(rx_buff,"$,^");
	if(rx_buff[1] == 'L')
	{
		p = strtok(NULL,"$,^");
		led_number = atoi(p);
		p = strtok(NULL,"$,^");
		status_led = atoi(p);
		switch_led(led_number,status_led);
		//control_led(13,1);
		sprintf(string,"%d %d", led_number,status_led);
		DisplayLCD(LCD_LINE5, string);
		decimal = status_bit();
		sprintf(str1,"$2479,L%04x^",decimal);
		DisplayLCD(LCD_LINE7, str1);
		//Uart_ClearBuff(&rx_buff[0], UART_RX_BUFFER_LEN - 1);
	}
	
	if(rx_buff[1] == 'T')
	{
		p = strtok(NULL,"$,^");
		line = atoi(p);
		line = (line * 8) - 8;
		p = strtok(NULL,"$,^");
		text = p;
		sprintf(string,"%c",text);
		DisplayLCD(line, string);
	}
    Uart_Clear(&rx_buff[0], UART_RX_BUFFER_LEN - 1);
    /* Display string in receive buffer */
    //ClearLCD();
}
}
}
void LCD_Reset(void)
{
int i =0;
/* Output a logic LOW level to external reset pin*/
P13_bit.no0 = 0;
for (i = 0; i < 1000; i++)
{
NOP();
}

/* Generate a raising edge by ouput HIGH logic level to external reset pin */
P13_bit.no0 = 1;
for (i = 0; i < 1000; i++)
{
NOP();
}

/* Output a logic LOW level to external reset pin, the reset is completed /
P13_bit.no0 = 0;
}
/*****************************************************************************
End of file
******************************************************************************/