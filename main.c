//����Դ����Դ����--������24Сʱ��ɾ��
#include "stm32f10x.h"
#include "interface.h"
#include "LCD1602.h"
#include "IRCtrol.h"
#include "motor.h"
#include "uart.h"

//ȫ�ֱ�������
unsigned int speed_count=0;//ռ�ձȼ����� 50��һ����
char front_left_speed_duty=SPEED_DUTY;
char front_right_speed_duty=SPEED_DUTY;
char behind_left_speed_duty=SPEED_DUTY;
char behind_right_speed_duty=SPEED_DUTY;

unsigned char tick_5ms = 0;//5ms����������Ϊ�������Ļ�������
unsigned char tick_1ms = 0;//1ms����������Ϊ����Ļ���������
unsigned char tick_200ms = 0;//ˢ����ʾ

char ctrl_comm = COMM_STOP;//����ָ��
unsigned char continue_time=0;
unsigned char bt_rec_flag=0;//�������Ʊ�־λ

void LEDInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE); 						 
//=============================================================================
//LED -> PC13
//=============================================================================			 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
}


int main(void)
{
	int i = 0;
	delay_init();
	GPIOCLKInit();
	TIM2_Init();
	MotorInit();
	//ServoInit();
	USART3Conf(9600);
	
	LEDInit();
	while (1)
	{
		//PutChar('A');
		switch (MoveFlag)
		{
			case 'G':
				CarGo();break;
			case 'B':
				 CarBack();break;
			case 'L':
				CarLeft();break;
			case 'R':
				CarRight();break;
			case 'S':
				CarStop();break;
			default:
				break;
		}
		Delayms(500);
		
		if (i % 2 == 0)
			GPIO_ResetBits(GPIOC, GPIO_Pin_13); 
		else
			GPIO_SetBits(GPIOC, GPIO_Pin_13); 
		Delayms(500);
		i++;
	}
	
	while (1)
	{
		//CarGo();
		PutChar('A');
		Delayms(500);
		
		if (i % 2 == 0)
			LED_SET;
		else
			LED_RESET;
		
		i++;
	}

 while(1)
 {	 
	if(tick_5ms >= 5)
		{
			tick_5ms = 0;
			tick_200ms++;
			if(tick_200ms >= 40)
			{
				tick_200ms = 0;
				LEDToggle(LED_PIN);
			}
			continue_time--;//200ms �޽���ָ���ͣ��
			if(continue_time == 0)
			{
				continue_time = 1;
				CarStop();
			}
			//do something
			if(bt_rec_flag == 1)//���յ������ź�
			{
				bt_rec_flag = 0;
				switch(ctrl_comm)
				{
					case COMM_UP:    CarGo();break;
					case COMM_DOWN:  CarBack();break;
					case COMM_LEFT:  CarLeft();break;
					case COMM_RIGHT: CarRight();break;
					case COMM_STOP:  CarStop();break;
					default : break;
				}
				LCD1602WriteCommand(ctrl_comm);
			}
		}
		
 }
}


