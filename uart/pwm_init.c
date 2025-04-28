#include "pwm_init.h"

void PWM_setup(void)
{
	
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

  // Настройка PA5 как альтернативной функции (AF2 для TIM2_CH1)
  GPIOA->MODER &= ~GPIO_MODER_MODER5; // Сброс режима
  GPIOA->MODER |= GPIO_MODER_MODER5_1; // Альтернативная функция
  GPIOA->AFR[0] |= (2 << (5 * 4)); // AF2 для PA5 (TIM2_CH1)

  // Настройка PA1 как альтернативной функции (AF2 для TIM2_CH2)
  GPIOA->MODER &= ~GPIO_MODER_MODER1; // Сброс режима
  GPIOA->MODER |= GPIO_MODER_MODER1_1; // Альтернативная функция
  GPIOA->AFR[0] |= (2 << (1 * 4)); // AF2 для PA1 (TIM2_CH2)

  // Включение тактирования TIM2
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  // Настройка таймера TIM2
  TIM2->PSC = 48-1; // Предделитель = 1 (частота таймера = 48 МГц)
  TIM2->ARR = 1000 - 1; // Автоперезагрузка (48 МГц / 1024 = 46.875 кГц)
	TIM2->EGR |= TIM_EGR_UG;
	TIM2->CR1 |= TIM_CR1_ARPE;
  // Настройка канала 1 (PA5) в режиме PWM mode 1
  TIM2->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; // PWM mode 1
  TIM2->CCMR1 |= TIM_CCMR1_OC1PE; // Включение предзагрузки
  TIM2->CCR1 = 500; // Скважность 50%
	// Настройка канала 2 (PA1) в режиме PWM mode 1
	TIM2->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1; // PWM mode 1 для канала 2
	TIM2->CCMR1 |= TIM_CCMR1_OC2PE; // Включениеф предзагрузки для канала 2
	TIM2->CCR2 = 234; // Скважность 50%

	// Включение таймера (но каналы пока отключены)
	TIM2->CR1 |= TIM_CR1_CEN;
}
void PWM_Control(TIM_TypeDef* TIMx, uint8_t channel, uint8_t enable) 
{
	if (enable) 
	{
		// Включение канала
    if (channel == 1) 
		{
			TIMx->CCER |= TIM_CCER_CC1E;
    } 
		else if (channel == 2) 
		{
      TIMx->CCER |= TIM_CCER_CC2E;
    } 
		else if (channel == 3) 
		{
      TIMx->CCER |= TIM_CCER_CC3E;
    } 
		else if (channel == 4) 
		{
      TIMx->CCER |= TIM_CCER_CC4E;
		}
  } 
	else 
	{
		// Отключение канала
    if (channel == 1) 
		{
      TIMx->CCER &= ~TIM_CCER_CC1E;
    } 
		else if (channel == 2) 
		{
      TIMx->CCER &= ~TIM_CCER_CC2E;
    } 
		else if (channel == 3) 
		{
      TIMx->CCER &= ~TIM_CCER_CC3E;
    } 
		else if (channel == 4) 
		{
      TIMx->CCER &= ~TIM_CCER_CC4E;
    }
  }
}
void TIM2_setup(void)
{
	
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	// Включение тактирования TIM2
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
	
  // Настройка PA1 как альтернативной функции (AF2 TIM2_CH2)
  GPIOA->MODER &= ~GPIO_MODER_MODER1; // Сброс режима
  GPIOA->MODER |= GPIO_MODER_MODER1_1; // Альтернативная функция
	GPIOA->AFR[0] |= 2 << (1 * 4) ; // AF2 для PA1 (TIM2_CH2)
	
	// Начальная настройка таймера TIM2
  TIM2->PSC = 1 - 1; //частота 8 МГц
  TIM2->ARR = 16; // Автоперезагрузка (800 кГц / 16 = 50 кГц)
	TIM2->CCR2 = 8; // Скважность 50%
	TIM2->CNT = 0;
	// Настройка канала 2 (PA1) в режиме PWM mode 1
	TIM2->CCMR1 &= ~TIM_CCMR1_OC2M;
	TIM2->CCMR1 |= (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE); // PWM mode 1 для канала 2
	//TIM2->CCMR1 |= TIM_CCMR1_OC2FE;
	
	//TIM2->EGR |= TIM_EGR_UG;
	
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->CR1 |= TIM_CR1_ARPE;
	
	NVIC_EnableIRQ(TIM2_IRQn);
	
}

void TIM1_setup(void)
{
	// 1. Включаем тактирование TIM1 и GPIO для TIM1 PA8
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    
    GPIOA->MODER &= ~GPIO_MODER_MODER8_Msk ;
    GPIOA->MODER |= 0x2 << GPIO_MODER_MODER8_Pos;
    
    GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL8_Msk ;
    GPIOA->AFR[1] |= 0x2 << GPIO_AFRH_AFSEL8_Pos;
    
		TIM1->PSC = 1 - 1; 
    TIM1->ARR = 100; 
		
		TIM1->CCR1 = 30;
    TIM1->EGR |= TIM_EGR_UG;
		
		TIM1->CR1 &= ~TIM_CR1_DIR; //если счет вверх, то CNT изначально должен быть >= CCR
		TIM1->CR1 |= TIM_CR1_OPM;

    TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0| TIM_CCMR1_OC1PE; // OPM mode 1 
    
		TIM1->SMCR |= RESET;
    TIM1->CCER |= TIM_CCER_CC1E;
    TIM1->DIER |= TIM_DIER_UIE;
		TIM1->BDTR |= TIM_BDTR_MOE;
    // Включаем таймер
		NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
		
}
