#ifndef _INDICATORLIGHT_H
#define _INDICATORLIGHT_H

#include "system.h"
/*  LEDʱ�Ӷ˿ڡ����Ŷ��� */
#define LED_PORT 			GPIOC   
#define LED_PIN 			(GPIO_Pin_13)
#define LED_PORT_RCC		RCC_APB2Periph_GPIOC


#define led PCout(13)


void LED_Init(void);

#endif
