#include "stm32f0xx.h"
#include "ADC_Holl.h"



void config_DAC(void){
// Включить тактирование порта A
RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

// Настроить PA0 (ADC_IN0) в аналоговом режиме
GPIOA->MODER |= GPIO_MODER_MODER0_0 | GPIO_MODER_MODER0_1; // Аналоговый режим (0b11)
GPIOA->MODER |= GPIO_MODER_MODER2_0 | GPIO_MODER_MODER2_1;
RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // Включить тактирование ADC1

// Настройка делителя ADC_CLK (если PCLK=48 МГц)
ADC1_COMMON->CCR |= (0x2 << 18);

// Настройка канала PA0
ADC1->CHSELR = ADC_CHSELR_CHSEL0 | ADC_CHSELR_CHSEL2;

// Время выборки
ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2;

// Калибровка
ADC1->CR |= ADC_CR_ADCAL;
while (ADC1->CR & ADC_CR_ADCAL);

// Включение ADC
ADC1->CR |= ADC_CR_ADEN;
while (!(ADC1->ISR & ADC_ISR_ADRDY));
}

uint16_t Read_ADC(uint8_t channel) {
    ADC1->CHSELR = (1 << 2); // Выбрать канал
    ADC1->CR |= ADC_CR_ADSTART;     // Запустить преобразование
    while (!(ADC1->ISR & ADC_ISR_EOC)); // Дождаться завершения
    return ADC1->DR; // Вернуть результат
}
