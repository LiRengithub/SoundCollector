#ifndef _USART_H
#define _USART_H

#include "system.h"
#include "stdio.h"

extern void USART1_Init(u32 baud);
extern int fputc(int ch, FILE *p);
#define USART1_MAX_QUEUE_BUF_SIZE 100	//USART1���ڻ�������С����λ���ֽ�

extern uint8_t USART1_RECEIEVE_QUEUE_BUF[USART1_MAX_QUEUE_BUF_SIZE];//USART1���ڻ�����
extern uint8_t USART1_QUEUE_REAR;//���ڻ���������β�����λ��
extern uint8_t USART1_QUEUE_FRONT;//���ڻ����������ף�����λ��
extern uint8_t USART1_QUEUE_FULL(void);
extern uint8_t USART1_QUEUE_EMPTY(void);
extern uint8_t USART1_PUSH_QUEUE(uint8_t ch);
extern uint8_t USART1_POP_QUEUE(void);
extern void ReadBytes(uint8_t n);
extern void WriteByte(uint8_t ch);
extern void WriteBytes(uint8_t n, uint8_t *p);


#endif
