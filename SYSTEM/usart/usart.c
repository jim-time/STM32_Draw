#include "usart.h"
#include "sys.h"
#include "main.h"
//#include "FIFO.h"

//#define DMA_ENABLE 1
#if DMA_ENABLE
static uint8_t _USART3_DMA_RX_BUF[_USART3_DMA_RX_BUF_LEN];
static uint8_t _USART3_RX_BUFFER[_USART3_RX_BUF_SIZE_IN_FRAMES*_USART3_DMA_RX_BUF_LEN];
//static FIFO_t _USART3_RX_FIFO;
#endif
 
#if EN_USART3_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART3_RX_BUF[USART3_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	���յ�0x0d
//bit14~0��	���յ�����Ч�ֽ���Ŀ
u16 USART3_RX_STA=0;       //����״̬���	

#if 1
int fputc(int ch, FILE *f);    //fputc�ض���

#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
int _sys_exit(int x) 
{ 
	return x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	USART3->DR = (u8) ch;   
	while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
   
	return ch;
}
#endif

void usart3_init(u32 bound)
{
  //GPIO Initialization
	GPIO_InitTypeDef GPIOInitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
//	DMA_InitTypeDef	DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);//Enable GPIOA Clock
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);//PB.10->USART.TX
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);//PB.11->USART.RX
	//USART3 PB.10,PB.11
	GPIOInitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
	GPIOInitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIOInitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIOInitStructure.GPIO_OType=GPIO_OType_PP;
	GPIOInitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIOInitStructure);                  //Initialize PB.10|PB.11
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//Enable USART3 Clock
	
	USART_InitStructure.USART_BaudRate=bound;// default:9600
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity=USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_Init(USART3,&USART_InitStructure);//��ʼ������
	USART_Cmd(USART3,ENABLE);               //ʹ�ܴ���
	USART_ClearFlag(USART3,USART_FLAG_TC);

#if DMA_ENABLE
	USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);// Enable USART1 DMA Transmission
	
	DMA_DeInit(DMA1_Stream1);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	while(DMA_GetCmdStatus(DMA1_Stream1)!=DISABLE){};
	/*Configurate DMA Stream*/
	DMA_InitStructure.DMA_Channel=DMA_Channel_4;							//Select Channel
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t)(&USART3->DR);		//DMA Peripheral Address
	DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)&_USART3_DMA_RX_BUF[0]; //DMA Memory0 Address
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralToMemory;				    //MemoryToPeripheral Mode
	DMA_InitStructure.DMA_BufferSize=sizeof(_USART3_DMA_RX_BUF)/2;
#endif

#if EN_USART3_RX
   USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//����RXNE�ж�
   //USART3 NVIC Initialization
   NVIC_InitStructure.NVIC_IRQChannel=USART3_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�1
   NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;       //��Ӧ���ȼ�1
   NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
   NVIC_Init(&NVIC_InitStructure);
#endif 
}
//д��LCD̫��ʱ�䣬Ӱ��USART�Ľ���
void USART3_IRQHandler()
{
	uint8_t Res;
	//extern uint8_t ResUsart;
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET) //�����жϣ������յ������ݱ�����0x0d��β��
	{
	    USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		Res=USART_ReceiveData(USART3);//(USART3->DR); //��ȡ���յ�������
		//ResUsart=Res; 
		if(!(USART3_RX_STA&0x8000))   //����δ���
		{
			//���յ���0x0d,���س���carriage return
			  if(Res==0x0d) USART3_RX_STA|=0x8000;//���մ������¿�ʼ
              else 	
			//û���յ�0x0d
		    {
                USART3_RX_BUF[USART3_RX_STA&0x7fff]=Res;
				USART3_RX_STA++;
				if(USART3_RX_STA>(USART3_REC_LEN-1)) USART3_RX_STA=0;//�������ݴ������½���		  
		    }
		}
	}

}
#endif
