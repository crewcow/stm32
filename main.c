#include "stm32f4xx.h"
#include "stdio.h"
#include "main.h"

#define RED_ON()				GPIO_SetBits(GPIOD, GPIO_Pin_14)
#define BLUE_ON()				GPIO_SetBits(GPIOD, GPIO_Pin_15)
#define RED_OFF()				GPIO_ResetBits(GPIOD, GPIO_Pin_14)
#define BLUE_OFF()			GPIO_ResetBits(GPIOD, GPIO_Pin_15)

#define RS(a)		GPIO_WriteBit(GPIOC, GPIO_Pin_13, (BitAction)a)
#define RW(a)		GPIO_WriteBit(GPIOC, GPIO_Pin_14, (BitAction)a)
#define EN(a)		GPIO_WriteBit(GPIOC, GPIO_Pin_15, (BitAction)a)
#define DB7(a)	GPIO_WriteBit(GPIOE, GPIO_Pin_0, (BitAction)a)
#define DB6(a)	GPIO_WriteBit(GPIOE, GPIO_Pin_1, (BitAction)a)
#define DB5(a)	GPIO_WriteBit(GPIOE, GPIO_Pin_2, (BitAction)a)
#define DB4(a)	GPIO_WriteBit(GPIOE, GPIO_Pin_3, (BitAction)a)
#define DB3(a)	GPIO_WriteBit(GPIOE, GPIO_Pin_4, (BitAction)a)
#define DB2(a)	GPIO_WriteBit(GPIOE, GPIO_Pin_5, (BitAction)a)
#define DB1(a)	GPIO_WriteBit(GPIOE, GPIO_Pin_6, (BitAction)a)
#define DB0(a)	GPIO_WriteBit(GPIOE, GPIO_Pin_7, (BitAction)a)

uint32_t Systick_delay=0;
uint16_t data_adc2[50000];
uint16_t i,k = 0;
uint16_t count_25k = 0;
uint8_t stage = 0; 
uint16_t level = 4000;
uint8_t flag = 0; 
char str[5];

void LEDs_ini(void)
{
	GPIO_InitTypeDef 	PORT_init_struct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	PORT_init_struct.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;
	PORT_init_struct.GPIO_Mode = GPIO_Mode_OUT;
	PORT_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
	PORT_init_struct.GPIO_OType = GPIO_OType_PP;
	PORT_init_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_Init(GPIOD, &PORT_init_struct);
}
void SysTick_Handler(void)
{
	if (Systick_delay > 0)
	{
		Systick_delay--;
	}
	if (flag == 1)
	{
		ADC_SoftwareStartConv(ADC1);
	}
}
void delay_ms(uint16_t data)
{
		Systick_delay = data*50;
		while(Systick_delay){}
}
void LCD_delay(void)
	{
		uint16_t i;
		for (i=0;i<1000;i++){}
	}
void write_data(uint8_t data)
	{
		if (((data>>7)&0x01) == 0x01){DB7(1);} else {DB7(0);}
		if (((data>>6)&0x01) == 0x01){DB6(1);} else {DB6(0);}
		if (((data>>5)&0x01) == 0x01){DB5(1);} else {DB5(0);}
		if (((data>>4)&0x01) == 0x01){DB4(1);} else {DB4(0);}
		
		if (((data>>3)&0x01) == 0x01){DB3(1);} else {DB3(0);}
		if (((data>>2)&0x01) == 0x01){DB2(1);} else {DB2(0);}
		if (((data>>1)&0x01) == 0x01){DB1(1);} else {DB1(0);}
		if (((data>>0)&0x01) == 0x01){DB0(1);} else {DB0(0);}
	}
void LCD_SendCommand(uint8_t Command)
	{
		RS(0);
		write_data(Command);
		EN(1);
		LCD_delay();
		EN(0);
	}
void LCD_SendData(uint8_t Command)
	{
		RS(1);
		write_data(Command);
		EN(1);
		LCD_delay();
		EN(0);
	}
void ClearLCD(void)
	{
		LCD_SendCommand(0x01);
		delay_ms(2);
		LCD_SendCommand(0x02);
		delay_ms(2);
	}
void LCD_SendString(uint8_t Pos_Num, char *strTemp)
	{
		uint8_t i=0;
		if (Pos_Num==0)
		{
			LCD_SendCommand(0x80);
		} else
		if (Pos_Num==1)
		{
			LCD_SendCommand(0x88);
		} else
		if (Pos_Num==2)
		{
			LCD_SendCommand(0xC0);
		} else	
		if (Pos_Num==3)
		{
			LCD_SendCommand(0xC8);
		}
		delay_ms(1);
		while(strTemp[i] != 0)
		{
			LCD_SendData(strTemp[i]);
			LCD_delay();
			i++;
		}
	}
void LCD_ini(void)
	{	
		GPIO_InitTypeDef GPIO_Init_LCD;
		GPIO_InitTypeDef GPIO_Init_LCD_C;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
		GPIO_Init_LCD.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
		GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
		GPIO_Init_LCD.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init_LCD.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_Init_LCD.GPIO_OType = GPIO_OType_PP;
		GPIO_Init_LCD.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOE, &GPIO_Init_LCD);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		GPIO_Init_LCD_C.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
		GPIO_Init_LCD_C.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init_LCD_C.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_Init_LCD_C.GPIO_OType = GPIO_OType_PP;
		GPIO_Init_LCD_C.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOC, &GPIO_Init_LCD_C);
		delay_ms(17);
		RW(0);
		LCD_SendCommand(0x30);
		delay_ms(5);
		LCD_SendCommand(0x30);
		delay_ms(1);
		LCD_SendCommand(0x30);
		delay_ms(1);
		LCD_SendCommand(0x38);
		delay_ms(1);
		LCD_SendCommand(0x0E);
		delay_ms(1);
		LCD_SendCommand(0x01);
		delay_ms(1);
		LCD_SendCommand(0x06);
		delay_ms(1);
		LCD_SendCommand(0x02);
		delay_ms(1);
	}
void ADC_IRQHandler(void)
	{
		if (ADC_GetITStatus(ADC1, ADC_IT_EOC) == SET)
		{
			if (stage == 1)
			{
				ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
				if (i<50000)
				{
					data_adc2[i] = ADC_GetConversionValue(ADC1);
					i++;
					count_25k++;
				}else
				{
					data_adc2[i] = ADC_GetConversionValue(ADC1);
					i=0;
					count_25k++;
				}
				if (count_25k == 24999) 
					{
						flag = 0;
						BLUE_ON();
					}
			}
			
			if (stage == 0)
			{
				ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
				if (i<50000)
				{
					data_adc2[i] = ADC_GetConversionValue(ADC1);
					if (data_adc2[i] > level)
						{
							k=i;
							stage = 1;
							count_25k = 0;
							RED_ON();
						}
					i++;
				}else
				{
					data_adc2[i] = ADC_GetConversionValue(ADC1);
					if (data_adc2[i] > level) 
						{
							k=i;
							stage = 1;
							count_25k = 0;
							RED_ON();
						}
					i=0;
				}
			}
		}
	}
void ADC_enable(void)
	{
		flag = 1;
		stage = 0;
		RED_OFF();
		BLUE_OFF();
	}
uint16_t buf_value(uint16_t j)
	{
		return(data_adc2[j]);
	}
void LCD_value(uint16_t A, uint16_t B, uint16_t C, uint16_t D)
	{
		sprintf(str,"%d",A);
		LCD_SendString(0,str);
			delay_ms(2);
		sprintf(str,"%d",B);
		LCD_SendString(1,str);
			delay_ms(2);
		sprintf(str,"%d",C);
		LCD_SendString(2,str);
			delay_ms(2);
		sprintf(str,"%d",D);
		LCD_SendString(3,str);
			delay_ms(2);
	}
int main(void) 
{ 	
	SysTick_Config(SystemCoreClock/50000);
	
	LEDs_ini();
	ADC_ini();
	LCD_ini();
	delay_ms(1000);
	ADC_enable();
	LCD_value(500, 60000, 123, 9);

	while(1)
	{
	}	
} 
