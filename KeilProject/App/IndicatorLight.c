/**
  ***************************************************************
  * @file    IndicatorLight.c
  * @author  Siyou luo
  * @contact siyouluo11@gmail.com
  * @version V1.0
  * @date    27-JAN-2019
  * @brief   ָʾ����������������ʾϵͳ�����Լ�bug����
  ******************************************************************************
  * @resources
  * GPIOC_Pin_13
  * ϵͳ���ϵĻ���ɫled��
  ******************************************************************************
  */
#include "IndicatorLight.h"
void LED_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;//����ṹ�����
	
	RCC_APB2PeriphClockCmd(LED_PORT_RCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=LED_PIN;  //ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //�����������ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //���ô�������
	GPIO_Init(LED_PORT,&GPIO_InitStructure); 	   /* ��ʼ��GPIO */
	
	GPIO_SetBits(LED_PORT,LED_PIN);   //��LED�˿����ߣ�Ϩ������LED
}

