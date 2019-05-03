#ifndef _USART2_H
#define _USART2_H

#include "system.h"
#include "stdio.h"

extern void USART2_Init(u32 baud);
//extern int fputc(int ch, FILE *p);
#define USART2_MAX_QUEUE_BUF_SIZE 100	//USART2���ڻ�������С����λ���ֽ�

extern uint8_t USART2_RECEIEVE_QUEUE_BUF[USART2_MAX_QUEUE_BUF_SIZE];//USART2���ڻ�����
extern uint8_t USART2_QUEUE_REAR;//���ڻ���������β�����λ��
extern uint8_t USART2_QUEUE_FRONT;//���ڻ����������ף�����λ��
extern uint8_t USART2_QUEUE_FULL(void);
extern uint8_t USART2_QUEUE_EMPTY(void);
extern uint8_t USART2_PUSH_QUEUE(uint8_t ch);
extern uint8_t USART2_POP_QUEUE(void);
extern void USART2_WriteByte(uint8_t ch);
extern void USART2_WriteBytes(uint8_t n, uint8_t *p);


#endif
