#include "usart1.h"
#include "adc.h"
uint8_t USART1_RECEIEVE_QUEUE_BUF[USART1_MAX_QUEUE_BUF_SIZE];//USART1���ڻ�����
uint8_t USART1_QUEUE_REAR=0;//���ڻ���������β�����λ��
uint8_t USART1_QUEUE_FRONT=0;//���ڻ����������ף�����λ��
/**
  * @brief :�ж�USART1���ڻ������Ƿ���
  * @param :void
  * @retval :����1:�� ����0:����
  * @usage:if(USART1_QUEUE_FULL()==1)...	if(USART1_QUEUE_FULL()==0)...
  */
uint8_t  USART1_QUEUE_FULL(void)
{
    return (uint8_t)(((USART1_QUEUE_REAR+1)% USART1_MAX_QUEUE_BUF_SIZE)==USART1_QUEUE_FRONT);
}


/**
  * @brief	:�ж�USART1���ڻ������Ƿ��
  * @param	:void
  * @retval	:����1:�� ����0:�ǿ�
  * @usage	:if(USART1_QUEUE_EMPTY()==1)...	if(USART1_QUEUE_EMPTY()==0)...
  */
uint8_t USART1_QUEUE_EMPTY(void)
{
    return (uint8_t)(USART1_QUEUE_REAR==USART1_QUEUE_FRONT);
}

/**
  * @brief	:��USART1���ڻ������ڴ���һ���ֽ�����
  * @param	ch: uint8_t���ͣ�д��ch
  * @retval	:1,��������д��ʧ�ܣ�0��д��ɹ�
  * @usage	:USART1_PUSH_QUEUE(0x3f)
  */
uint8_t USART1_PUSH_QUEUE(uint8_t ch)
{
    if(USART1_QUEUE_FULL())
    {
        printf("queue full!\n");
        return 1;
    }
    USART1_QUEUE_REAR=(USART1_QUEUE_REAR+1) % USART1_MAX_QUEUE_BUF_SIZE;
    USART1_RECEIEVE_QUEUE_BUF[USART1_QUEUE_REAR]=ch;
    return 0;
}
/**
  * @brief	:��USART1���ڻ�����ȡ��һ���ֽ�����
  * @param	:void
  * @retval	:uint8_t ���ͣ���Ϊ�ӻ�����ȡ�����ֽ�
  * @usage	:uint8_t temp;
  * @usage	:temp=USART1_POP_QUEUE();
  */
uint8_t USART1_POP_QUEUE(void)
{
    uint8_t temp;
    if(USART1_QUEUE_EMPTY())
    {
        printf("queue empty!\n");
        return 1;
    }
    USART1_QUEUE_FRONT = (USART1_QUEUE_FRONT+1)%USART1_MAX_QUEUE_BUF_SIZE;
    temp = USART1_RECEIEVE_QUEUE_BUF[USART1_QUEUE_FRONT];
    return temp;
}
int fputc(int ch, FILE *p)
{
	USART_SendData(USART1,(u8)ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
	return ch;
}

void USART1_WriteByte(uint8_t ch)
{
	USART_SendData(USART1,(u8)ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
}
void USART1_WriteBytes(uint8_t n, uint8_t *p)
{
	uint8_t i;
	for(i=0;i<n;i++)
	{
		USART1_WriteByte(p[i]);
	}
}

void USART1_Init(u32 baud)
{
   //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
 
	
	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX			   //�������PA9
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //�����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);  /* ��ʼ����������IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX			 //��������PA10
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //ģ������
	GPIO_Init(GPIOA,&GPIO_InitStructure); /* ��ʼ��GPIO */
	

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = baud;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
		
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����	
}

/*******************************************************************************
* �� �� ��         : USART1_IRQHandler
* ��������		   : USART1�жϺ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/ 
void USART1_IRQHandler(void)                	//����1�жϷ������
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
	{
		if(!USART1_QUEUE_FULL())
        {
//            USART1_PUSH_QUEUE(USART_ReceiveData(USART1));
			if(USART_ReceiveData(USART1)==0xa9)
			{
				ADC_CLEAR_QUEUE();
				TIM_Cmd(TIM4,ENABLE); //ʹ�ܶ�ʱ��	
			}
			if(USART_ReceiveData(USART1)==0xb9)
			{
				TIM_Cmd(TIM4,DISABLE); //�رն�ʱ��	
			}
			
        }
	} 
	USART_ClearFlag(USART1,USART_FLAG_TC);
} 	
