#ifndef __USART_H
#define __USART_H

#include "stdio.h"	
#include "main.h"
#define USART3_REC_LEN  		200  	//�����������ֽ��� 200
#define EN_USART3_RX 			1		//ʹ�ܣ�1��/��ֹ��0������3����

#define _USART3_RX_BUF_SIZE 128u
#define _USART3_DMA_RX_BUF_LEN 30u
#define _USART3_RX_BUF_SIZE_IN_FRAMES (_USART3_RX_BUF_SIZE/RC_FRAME_LENGTH)
#define RC_FRAME_LENGTH 18u
	  	
extern u8  USART3_RX_BUF[USART3_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART3_RX_STA;        

void usart3_init(u32 bound);
void USART3_IRQHandler(void);

#endif
