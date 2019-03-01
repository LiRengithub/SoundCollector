#ifndef _USART3_H
#define _USART3_H

#include "system.h"
#include "stdio.h"

#define USART3_MAX_QUEUE_BUF_SIZE 100	//USART3���ڻ�������С����λ���ֽ�

extern uint8_t USART3_RECEIEVE_QUEUE_BUF[USART3_MAX_QUEUE_BUF_SIZE];//USART3���ڻ�����
extern uint8_t USART3_QUEUE_REAR;//���ڻ���������β�����λ��
extern uint8_t USART3_QUEUE_FRONT;//���ڻ����������ף�����λ��
extern uint8_t USART3_QUEUE_FULL(void);
extern uint8_t USART3_QUEUE_EMPTY(void);
extern uint8_t USART3_PUSH_QUEUE(uint8_t ch);
extern uint8_t USART3_POP_QUEUE(void);




extern void USART3_Init(u32 baud);
//int fputc(int ch, FILE *p);
extern void USART3_WriteByte(uint8_t ch);
extern void USART3_WriteBytes(uint8_t n, uint8_t *p);


#endif
