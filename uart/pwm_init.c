#include "pwm_init.h"

void ports_for_timers_init(void) //инициализация портов и включение такстирования таймеров
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //init PA

  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; //TIM2
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; //TIM3
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // init PB
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN; //TIM6
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; //TIM7
	// Настройка PA5 и PA6 как выхода
	GPIOA->MODER |= GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0; // PA5 как выход (01) PA5(ЛН) PA6(ПН)
	//GPIOA->OTYPER &= ~GPIO_OTYPER_OT_5;  // Push-Pull (0)
  // Настройка PA1 как альтернативной функции (AF2 TIM2_CH2)
  GPIOA->MODER &= ~(GPIO_MODER_MODER1); // Сброс режима
  GPIOA->MODER |= (GPIO_MODER_MODER1_1); // Альтернативная функция
	GPIOA->AFR[0] |= ((2 << (1 * 4))); // AF2 для PA1 (TIM2_CH2)
	// Настройка PB4 (TIM3_CH1)
  GPIOB->MODER &= ~(GPIO_MODER_MODER4 | GPIO_MODER_MODER5 | GPIO_MODER_MODER10);
  GPIOB->MODER |= (GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1 | GPIO_MODER_MODER10_1);
  GPIOB->AFR[0] |= (1 << (4 * 4)) | (1 << (5 * 4)); // AF1 для TIM3_CH1
	GPIOB->AFR[1] |= (2 << (10-8)*4);
}

void TIM2_setup(void) //задаем сколько времени будет идти запускающий импульс
{
	// Начальная настройка таймера TIM2
  TIM2->PSC = 48 - 1; //частота 8 МГц
  TIM2->ARR = 1000 - 1; 
//	TIM2->CCR1 = 1000 - 200; 
	TIM2->CCR2 = 1000 - 200; 
//	TIM2->CCR3 = 1000 - 200; 
//	TIM2->CCR4 = 1000 - 200; 
	TIM2->EGR |= TIM_EGR_UG;
	TIM2->SR &= ~TIM_SR_UIF;
	TIM2->CR1 &= ~TIM_CR1_DIR;
	
	// Настройка канала 2 (PA1) в режиме PWM mode 1
	TIM2->CCMR1 &= ~TIM_CCMR1_OC2M;
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;
	
	TIM2->CCMR1 |= (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_0 | TIM_CCMR1_OC2PE); // PWM mode 1 для канала 2
	TIM2->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1PE);
	TIM2->CCMR2 |= (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0 | TIM_CCMR2_OC3PE);
	TIM2->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_0 | TIM_CCMR2_OC4PE);
	
	TIM2->CCMR1 |= TIM_CCMR1_OC2FE;
	
	TIM2->CCMR1 |= TIM_CCMR1_OC1FE;
	TIM2->CCMR2 |= TIM_CCMR2_OC3FE;
	TIM2->CCMR2 |= TIM_CCMR2_OC4FE;
	
	TIM2->CR1 |= TIM_CR1_OPM;
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->CR1 |= TIM_CR1_ARPE;
	NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_CH_3_start(int tpulse) //запускающий ПВ на tpulse мкс
{
	TIM2->CCR3 = 1000 - tpulse;
	TIM2->EGR |= TIM_EGR_UG;
	TIM2->SR &= ~TIM_SR_UIF;
	TIM2->CR1 |= TIM_CR1_CEN;
	TIM2->CCER |= TIM_CCER_CC3E;
}

void TIM2_CH_2_start(int tpulse) // запускающий ЛВ на tpulse мкс
{
	TIM2->CCR2 = 1000 - tpulse;
	TIM2->EGR |= TIM_EGR_UG;
	TIM2->SR &= ~TIM_SR_UIF;
	TIM2->CR1 |= TIM_CR1_CEN;
	TIM2->CCER |= TIM_CCER_CC2E;
}

void TIM3_setup(void) 
{
	TIM3->PSC = 1 - 1;  // 48 МГц
	TIM3->ARR = 2200 - 1; //f = 22 кГц
	TIM3->CCR1 = 2200 - 110; // 2200 - 110
	TIM3->CCR2 = 2200 - 110; // 2200 - 110
	TIM3->EGR = TIM_EGR_UG;
	TIM3->SR &= ~TIM_SR_UIF;
	TIM3->CCMR1 |= TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE;
	TIM3->CCMR1 |= TIM_CCMR1_OC2M_0 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2PE;
	
	TIM3->CR1 |= TIM_CR1_ARPE;
	TIM3->DIER |= TIM_DIER_UIE;
	TIM3->CR1 |= TIM_CR1_CEN;
}
void TIM6_setup(void)
{
	TIM6->SR = 0;
	TIM6->PSC = 48 - 1; // 48/48 1 МГц (1 тик = 1 мкс)
	//TIM6->ARR = 1000 - 1; //секунда
	//TIM6->EGR |= TIM_EGR_UG;
	//TIM6->SR &= ~TIM_SR_UIF;
	TIM6->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

void TIM7_setup(void)
{
	TIM7->SR = 0;
	TIM7->PSC = 1 - 1;
	//TIM7->ARR = 100 - 1;
	//TIM7->EGR |= TIM_EGR_UG;
	//TIM7->SR &= ~TIM_SR_UIF;
	TIM7->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM7_IRQn);
}

void start_counting_time_mks(int mks) // сколько будет длиться ШИМ сигнал
{
	if(TIM6->CR1 & TIM_CR1_CEN) return;
	TIM6->CR1 &= ~TIM_CR1_CEN;
	TIM6->ARR = mks - 1;
	TIM6->CNT = 0;
	
	TIM6->CR1 |= TIM_CR1_CEN;
}
void start_count_for_delay(int mks)
{
	if(TIM7->CR1 & TIM_CR1_CEN) return;
	TIM7->CR1 &= ~TIM_CR1_CEN;
	TIM7->ARR = mks - 1;
	TIM7->CNT = 0;
	

	TIM7->CR1 |= TIM_CR1_CEN;
	
}
void off_all_channels(void)
{
	TIM3->CCER &= ~TIM_CCER_CC1E;
	TIM3->CCER &= ~TIM_CCER_CC2E;
	TIM2->CCER &= ~TIM_CCER_CC1E;
	TIM2->CCER &= ~TIM_CCER_CC2E;
	TIM2->CCER &= ~TIM_CCER_CC3E;
	TIM2->CCER &= ~TIM_CCER_CC4E;
}
