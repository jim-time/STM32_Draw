#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "LCD12864.h"

//ALIENTEK ̽����STM32F407������ ʵ��1
//�����ʵ�� -�⺯���汾
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK

int main(void)
{ 
	double i=0;
	long long int j=0;
	delay_init(168);		  //��ʼ����ʱ����
	LCD12864_PORT_init();
	LCD_init();
	LCD_TClr();
	LCD_TLine(0,0,"Happy birthday");
	LCD_TLine(1,2,"to you!");
	LCD_TLine(2,0,"My dear country.");
	while(1)
	{             
		LCD_TData(3,0,i);
		for(j=0;j<100000;j++)
		{
			j=j;
		}
		i++;
		if(i>=10000) i=0;
	}
}
