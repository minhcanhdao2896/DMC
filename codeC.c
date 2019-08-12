#include "r_macro.h" / System macro and standard type definition /
#include "r_spi_if.h" / SPI driver interface /
#include "lcd.h" / LCD driver interface /
#include "timer.h" / Timer driver interface /
#include "stdio.h" / Standard IO library: sprintf */
#include "string.h"

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct Clock
{
uint8_t centisecond[20];
uint8_t second[20];
uint8_t minute[20];

}data;

time_t p_time;
data savedata;

char *string_shown_on_lcd[10];
char *string_record[10];
int flag = 0;
int flag_pause =0;
int current=0;
int record_time = 0;
int lcd_position = 8; // position of lcd
unsigned int record_position = 0; //position of #
unsigned int record_number = 0; // count in struct
int scroll_up=0;
int scroll_down=0;
int record_count=0;

void r_main_userinit(void);
void start_status(time_t time, char str);
void pause_stopwatch(time_t time, char str);
void restart_status();
void record_elapsedtime(int lcd_position,data data);
void Delay(unsigned long );
void conf_SW();
void scroll_down(data data);
void scroll_up(data data);

void conf_SW() //Cofigure Button
{
PM7_bit.no4 = 1;
PM7_bit.no5 = 1;
PM7_bit.no6 = 1;
PM6_bit.no2 = 0;
P7_bit.no4 = 1;
P7_bit.no5 = 1;
P7_bit.no6 = 1;
P6_bit.no2 = 1;
}
void Delay(unsigned long M)
{
unsigned long i=M1778;
while(i!=0) i--;
}
void start_status(time_t *time, char *str)
{
if(g_time_update_flag >= TIMER_COUNT_INTERVAL)
{
g_time_update_flag = 0;
g_time.centisecond = g_time.centisecond + 10;
if(g_time.centisecond == 100)
{
g_time.second++;
g_time.centisecond =0;
}
if(g_time.second == 60)
{
g_time.minute++;
g_time.second =0;
}
p_time = g_time;
*time = p_time;
DisplayLCD(LCD_LINE1, str);
sprintf(string_shown_on_lcd, "%d:%d:%d", time->minute, time->second, time->centisecond);
DisplayLCD(LCD_LINE2, string_shown_on_lcd);
}
}
void restart_status(int *lcd_position)
{
R_IT_Stop();
g_time_update_flag = 0;
g_time.centisecond = 0;
g_time.second = 0;
g_time.minute = 0;
*lcd_position = 8;

R_IT_Start();
}
void pause_stopwatch(time_t *time, char *str)
{
R_IT_Stop();
g_time_update_flag = 0;
DisplayLCD(LCD_LINE1, str);
sprintf(string_shown_on_lcd, " %d:%d:%d", time->minute, time->second, time->centisecond);
DisplayLCD(LCD_LINE2, string_shown_on_lcd);
}
void record_elapsedtime(int *lcd_position,data *data)
{
int lcd_row = 16;
int number_row = 5;
*lcd_position = *lcd_position + 8;
record_number++;
record_position++;
if(*lcd_position > 56)
{
while(number_row > 0)
{
sprintf(string_record, "#%d %d:%d:%d", record_number - number_row, data->minute[record_number - number_row], data->second[record_number - number_row], data->centisecond[record_number - number_row]);
DisplayLCD(lcd_row, string_record);
lcd_row = lcd_row + 8;
number_row--;
}
*lcd_position = 56;
 current= number_record;
}

data->centisecond[record_number] = p_time.centisecond;
data->second[record_number] = p_time.second;
data->minute[record_number] = p_time.minute;
sprintf(string_record, "#%d %d:%d:%d", record_position,p_time.minute, p_time.second,p_time.centisecond);
DisplayLCD(*lcd_position, string_record);
}
void scroll_up(data *data)
{
	if(current < 7)
	{
	DisplayLCD(LCD_LINE1, (uint8_t *)"FIRST RECORD");
	delay(1000);
	DisplayLCD(LCD_LINE1, (uint8_t *)"  RUNNING   ");
	}	
	if(current>6)
	{
	int lcd_row = 16;
	int number_row = 6;
	while(number_row > 0)
	{
	sprintf(string_record, "#%d %d:%d:%d", current - number_row, data->minute[current - number_row], data->second[current - number_row], data->centisecond[current - number_row]);
	DisplayLCD(lcd_row, string_record);
	lcd_row = lcd_row + 8;
	//number_row++;
	if(lcd_row >= 56)
	{
	lcd_row = 56;
	}
	}
	current--;
	}
}

void scroll_down(data *data)
{
	//if(current < 7 ||current=number_record)
	if(current < 7)
	{
	DisplayLCD(LCD_LINE1, (uint8_t *)"LAST RECORD");
	delay(1000);
	DisplayLCD(LCD_LINE1, (uint8_t *)"  RUNNING   ");
	}	
	if((current>6)&&(current<20))
	{
	int lcd_row = 16;
	int number_row = 6;
	while(number_row > 0)
	{
	sprintf(string_record, "#%d %d:%d:%d", current - number_row, data->minute[current - number_row], data->second[current - number_row], data->centisecond[current - number_row]);
	DisplayLCD(lcd_row, string_record);
	lcd_row = lcd_row - 8;
	//number_row--;
	if(lcd_row >= 56)
	{
	lcd_row = 56;
	}
	}
	current++;
	}
}
void main(void)
{

/* Initialize user system */
r_main_userinit();
R_IT_Create();
/* Clear LCD display */
ClearLCD();

/* Print message to the first line of LCD */
DisplayLCD(LCD_LINE1, (uint8_t *)"Welcome");
DisplayLCD(LCD_LINE2, "00:00:00");
conf_SW();
//R_IT_Start();
/* Main loop - Infinite loop */
while (1) 
{
    WDTE = 0xAC;
    //pause_stopwatch(&p_time,);
    if(P7_bit.no5 == 0)
    {
        Delay(200);
        if(flag == 1)
        { if (record_number<20){
            record_time = 1;}
        }
        flag = 1;
    flag_pause =0;
    }
/////////////////////////////
    if(record_time == 1)
    {
        record_time = 0;
        record_elapsedtime(&lcd_position,&savedata);
    //flag =0;
    }
    if(flag == 1)
    {
        R_IT_Start();
        start_status(&p_time,(uint8_t*)"Running");
    //flag =0;
    }
        
/////////////////////////////////////
//R_IT_Stop();
   if(P7_bit.no4 ==0 && P7_bit.no6 ==1)
   {
       Delay(300);
   scroll_down=1;
   }
   if(scroll_down==1)
   {
   
   scroll_down=0;
   //record_number++;
   scroll_down(&savedata); 
   
   
   }
  //////////////////////////// 
   if(P7_bit.no6 ==0 && P7_bit.no4 ==1)
   {
       Delay(200);
   scroll_up=1;
   
   }
   if(scroll_up==1)
   {
   scroll_up=0;
   scroll_up(&savedata); 
   //curent--;
   //record_number--;
   }
   
    if(P7_bit.no6 ==0 && P7_bit.no4 ==0)
   {
     Delay(500);
        if(flag_pause==1)
    {
    
        P6_bit.no2 = 0;
    ClearLCD();
        DisplayLCD(LCD_LINE1, (uint8_t *)"Restart!");
        DisplayLCD(LCD_LINE2, "00:00:00");
    restart_status(&lcd_position); 
    flag_pause=0;
    flag=0;
    }
        if(flag==1)
    {
    flag_pause=1;
    pause_stopwatch(&p_time,(uint8_t*)"Pausing..."); 
    }
   }
     
}
}

void r_main_userinit(void)
{
uint16_t i;

/* Enable interrupt /
EI();
/ Output a logic LOW level to external reset pin*/
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

/* Output a logic LOW level to external reset pin, the reset is completed */
P13_bit.no0 = 0;

/* Initialize SPI channel used for LCD */
R_SPI_Init(SPI_LCD_CHANNEL);

/* Initialize Chip-Select pin for LCD-SPI: P145 (Port 14, pin 5) /
R_SPI_SslInit(
SPI_SSL_LCD, / SPI_SSL_LCD is the index defined in lcd.h */
(unsigned char )&P14, / Select Port register */
(unsigned char )&PM14, / Select Port mode register /
5, / Select pin index in the port /
0, / Configure CS pin active state, 0 means active LOW level /
0 / Configure CS pin active mode, 0 means active per transfer */
);

/* Initialize LCD driver */
InitialiseLCD();
}

/******************************************************************************
End of file
******************************************************************************/