#include "uart.h"
#include "interface.h"

//UART function
//UART3 TxD GPIOB10   RxD GPIOB11



#define Max_BUFF_Len 50
unsigned char Uart3_Buffer[Max_BUFF_Len];
unsigned int Uart3_Rx=0;
u8 MoveFlag;

void USART3Conf(u32 baudRate)
{
	NVIC_InitTypeDef 	NVIC_InitStruct;//定义一个设置中断的结构体	
	USART_InitTypeDef USART_InitSturct;//定义串口1的初始化结构体
	GPIO_InitTypeDef GPIO_InitStruct;//定义串口对应管脚的结构体

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 , ENABLE);//打开串口管脚时钟
	//USART3_Tx_Pin Configure 
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;//输出引脚
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;//设置最高速度50MHz
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//推挽复用输出
	GPIO_Init(GPIOB , &GPIO_InitStruct);//将初始化好的结构体装入寄存器

//USART3_Rx_Pin Configure
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO模式悬浮输入
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;//输入引脚
  GPIO_Init(GPIOB, &GPIO_InitStruct);//将初始化好的结构体装入寄存器

//USART3 Configure	
	USART_InitSturct.USART_BaudRate = baudRate;//波特率9600
	USART_InitSturct.USART_WordLength = USART_WordLength_8b;//数据宽度8位
	USART_InitSturct.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitSturct.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitSturct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitSturct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//使能发送与接收
	USART_Init(USART3 , &USART_InitSturct);//将初始化好的结构体装入寄存器	
	//USART1_INT Configure
	USART_ITConfig(USART3 , USART_IT_RXNE , ENABLE);//使能接收中断
//	USART_ITConfig(USART3 , USART_IT_TXE , ENABLE);
	USART_Cmd(USART3 , ENABLE);//打开串口
	USART_ClearFlag(USART3 , USART_FLAG_TC);//解决第一个数据发送失败的问题
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//打开该中断
	NVIC_Init(&NVIC_InitStruct);
	
}

void PutChar(u8 Data)
{
	USART_SendData(USART3 , Data);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);//等待发送完毕
}
void PutStr(char *str)//发送一个字符串
{
	while(*str != '\0')
	{
		USART_SendData(USART3 , *str++);
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);//等待发送完毕
	}
}

void PutNChar(u8 *buf , u16 size)
{
  u8 i;
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET); //防止第一字节丢失
	for(i=0;i<size;i++)
	{
		 USART_SendData(USART3 , buf[i]);
		 while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);//等待发送完毕
	}
}


void USART3_IRQHandler()
{
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET) //中断产生 
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE); //清除中断标志
			 
		Uart3_Buffer[Uart3_Rx] = USART_ReceiveData(USART3);     //接收串口1数据到buff缓冲区
		Uart3_Rx++; 
     		 
		if(Uart3_Buffer[Uart3_Rx-1] == 0x0a || Uart3_Rx == Max_BUFF_Len)    //如果接收到尾标识是换行符（或者等于最大接受数就清空重新接收）
		{
			if(Uart3_Buffer[0] == 'A')                      //检测到头标识是我们需要的 
			{
				//printf("%s\r\n",Uart3_Buffer);        //这里我做打印数据处理
				EXTI_InitTypeDef EXTI_InitStructure;
				EXTI_InitStructure.EXTI_LineCmd = DISABLE;
				EXTI_Init(&EXTI_InitStructure);


				switch (Uart3_Buffer[1])
				{
					case 'G':
					case 'B':
					case 'L':
					case 'R':
					case 'S':
						MoveFlag = Uart3_Buffer[1];
						break;
				}
				Uart3_Rx=0;   

				EXTI_InitStructure.EXTI_LineCmd = ENABLE;
				EXTI_Init(&EXTI_InitStructure);				
			} 
			else
			{
				Uart3_Rx=0;                                   //不是我们需要的数据或者达到最大接收数则开始重新接收
			}
		}
	}
}



