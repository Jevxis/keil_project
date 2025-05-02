#include "pwm_init.h"

void ports_for_timers_init(void) //инициализация портов и включение такстирования таймеров
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //init PA

  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; //TIM2
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; //TIM3
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // init PB
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN; //TIM6
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; //TIM7
	// Настройка PA5 как выхода
	GPIOA->MODER |= GPIO_MODER_MODER5_0; // PA5 как выход (01)
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_5;  // Push-Pull (0)
  // Настройка PA1 как альтернативной функции (AF2 TIM2_CH2)
  GPIOA->MODER &= ~GPIO_MODER_MODER1; // Сброс режима
  GPIOA->MODER |= GPIO_MODER_MODER1_1; // Альтернативная функция
	GPIOA->AFR[0] |= 2 << (1 * 4) ; // AF2 для PA1 (TIM2_CH2)
	// Настройка PB4 (TIM3_CH1)
  GPIOB->MODER &= ~GPIO_MODER_MODER4;
  GPIOB->MODER |= GPIO_MODER_MODER4_1;
  GPIOB->AFR[0] |= (1 << (4 * 4)); // AF1 для TIM3_CH1
}

void TIM2_setup(int tpulse) //задаем сколько времени будет идти запускающий импульс
{
	// Начальная настройка таймера TIM2
  TIM2->PSC = 48 - 1; //частота 8 МГц
  TIM2->ARR = 200 - 1; 
	TIM2->CCR2 = 200 - tpulse; 
	TIM2->EGR |= TIM_EGR_UG;
	TIM2->SR &= ~TIM_SR_UIF;
	TIM2->CR1 &= ~TIM_CR1_DIR;
	
	// Настройка канала 2 (PA1) в режиме PWM mode 1
	TIM2->CCMR1 &= ~TIM_CCMR1_OC2M;
	TIM2->CCMR1 |= (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_0 | TIM_CCMR1_OC2PE); // PWM mode 1 для канала 2
	TIM2->CCMR1 |= TIM_CCMR1_OC2FE;
	
	TIM2->CR1 |= TIM_CR1_OPM;
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->CR1 |= TIM_CR1_ARPE;
}

void TIM2_start()
{
	
}

void TIM3_setup(void) 
{
	TIM3->PSC = 1 - 1;  // 48 МГц
	TIM3->ARR = 2200 - 1; //f = 22 кГц
	TIM3->CCR1 = 1100; // 2200 - 110
	TIM3->EGR = TIM_EGR_UG;
	TIM3->SR &= ~TIM_SR_UIF;
	TIM3->CCMR1 |= TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE;
	TIM3->CR1 |= TIM_CR1_ARPE;
	TIM3->DIER |= TIM_DIER_UIE;
	
}
void TIM6_setup(void)
{
	TIM6->PSC = 48 - 1; // 48/48 1 МГц (1 тик = 1 мкс)
	TIM6->ARR = 1000 - 1; //секунда
	TIM6->EGR |= TIM_EGR_UG;
	TIM6->SR &= ~TIM_SR_UIF;
	//TIM6->CR1 |= TIM_CR1_URS;
	TIM6->DIER |= TIM_DIER_UIE;
}

void TIM7_setup(void)
{
	TIM7->PSC = 48 - 1;
	TIM7->ARR = 100 - 1;
	TIM7->EGR |= TIM_EGR_UG;
	TIM7->SR &= ~TIM_SR_UIF;
	TIM7->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM7_IRQn);
}

void start_counting_time_mks(int mks)
{
	TIM6->ARR = mks - 1;
	TIM6->CNT = 0;
	TIM6->CR1 |= TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
}
