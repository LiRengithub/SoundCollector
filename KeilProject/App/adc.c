#include "adc.h"
#include "SysTick.h"
#include "usart1.h"

uint8_t ADC_QUEUE_BUF[ADC_MAX_QUEUE_BUF_SIZE];//ADC���ڻ�����
uint8_t ADC_QUEUE_REAR=0;//���ڻ���������β�����λ��
uint8_t ADC_QUEUE_FRONT=0;//���ڻ����������ף�����λ��

/*******************************************************************************
* �� �� ��         : ADCx_Init
* ��������		   : ADC��ʼ��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void ADCx_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; //����ṹ�����
    ADC_InitTypeDef       ADC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);//����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;//ADC
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;	//ģ������
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//�ر�����ת��
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//��ֹ������⣬ʹ���������
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 1;//1��ת���ڹ��������� Ҳ����ֻת����������1
    ADC_Init(ADC1, &ADC_InitStructure);//ADC��ʼ��

    ADC_Cmd(ADC1, ENABLE);//����ADת����

    ADC_ResetCalibration(ADC1);//����ָ����ADC��У׼�Ĵ���
    while(ADC_GetResetCalibrationStatus(ADC1));//��ȡADC����У׼�Ĵ�����״̬

    ADC_StartCalibration(ADC1);//��ʼָ��ADC��У׼״̬
    while(ADC_GetCalibrationStatus(ADC1));//��ȡָ��ADC��У׼����

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);//ʹ�ܻ���ʧ��ָ����ADC�����ת����������
}

/*******************************************************************************
* �� �� ��         : Get_ADC_Value
* ��������		   : ��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ��
* ��    ��         : ch:ͨ�����
					 times:��ȡ����
* ��    ��         : ͨ��ch��times��ת�����ƽ��ֵ
*******************************************************************************/
u16 Get_ADC_Value(u8 ch)
{
    //����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);	//ADC1,ADCͨ��,239.5������,��߲���ʱ�������߾�ȷ��

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);//ʹ��ָ����ADC1�����ת����������
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
    return ADC_GetConversionValue(ADC1);
}





/**
  * @brief :�ж�ADC���ڻ������Ƿ���
  * @param :void
  * @retval :����1:�� ����0:����
  * @usage:if(ADC_QUEUE_FULL()==1)...	if(ADC_QUEUE_FULL()==0)...
  */
uint8_t  ADC_QUEUE_FULL(void)
{
    return (uint8_t)(((ADC_QUEUE_REAR+1)% ADC_MAX_QUEUE_BUF_SIZE)==ADC_QUEUE_FRONT);
}


/**
  * @brief	:�ж�ADC���ڻ������Ƿ��
  * @param	:void
  * @retval	:����1:�� ����0:�ǿ�
  * @usage	:if(ADC_QUEUE_EMPTY()==1)...	if(ADC_QUEUE_EMPTY()==0)...
  */
uint8_t ADC_QUEUE_EMPTY(void)
{
    return (uint8_t)(ADC_QUEUE_REAR==ADC_QUEUE_FRONT);
}

/**
  * @brief	:��ADC���ڻ������ڴ���һ���ֽ�����
  * @param	ch: uint8_t���ͣ�д��ch
  * @retval	:1,��������д��ʧ�ܣ�0��д��ɹ�
  * @usage	:ADC_PUSH_QUEUE(0x3f)
  */
uint8_t ADC_PUSH_QUEUE(uint8_t ch)
{
    if(ADC_QUEUE_FULL())
    {
//        printf("queue full!\n");
        return 1;
    }
    ADC_QUEUE_REAR=(ADC_QUEUE_REAR+1) % ADC_MAX_QUEUE_BUF_SIZE;
    ADC_QUEUE_BUF[ADC_QUEUE_REAR]=ch;
    return 0;
}
/**
  * @brief	:��ADC���ڻ�����ȡ��һ���ֽ�����
  * @param	:void
  * @retval	:uint8_t ���ͣ���Ϊ�ӻ�����ȡ�����ֽ�
  * @usage	:uint8_t temp;
  * @usage	:temp=ADC_POP_QUEUE();
  */
uint8_t ADC_POP_QUEUE(void)
{
    uint8_t temp;
    if(ADC_QUEUE_EMPTY())
    {
//        printf("queue empty!\n");
        return 1;
    }
    ADC_QUEUE_FRONT = (ADC_QUEUE_FRONT+1)%ADC_MAX_QUEUE_BUF_SIZE;
    temp = ADC_QUEUE_BUF[ADC_QUEUE_FRONT];
    return temp;
}

/**
  * @brief	:���ADC���ڻ�����
  * @param	:void
  * @retval	:void
  * @usage	:ADC_CLEAR_QUEUE();
  */
void ADC_CLEAR_QUEUE(void)
{
	ADC_QUEUE_REAR=0;
	ADC_QUEUE_FRONT=0;
}
