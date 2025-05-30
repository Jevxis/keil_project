#include "uart_init.h"

void UART_Init(void) 
{
    // Включаем тактирование порта GPIOA и USART1
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    //PA9 (TX) и PA10 (RX)
    GPIOA->MODER &= ~(GPIO_MODER_MODER9 | GPIO_MODER_MODER10);
    GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1;

    GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL9 | GPIO_AFRH_AFSEL10);
    GPIOA->AFR[1] |= (1 << (9 - 8) * 4) | (1 << (10 - 8) * 4); // AF1 для USART1

    // Настраиваем USART1
    USART1->BRR = 48000000 /(115200); // Скорость 9600 бод
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE; // Включаем передатчик, приемник и USART
}

void UART_SendString(char *str) {
    while (*str) {
        while (!(USART1->ISR & USART_ISR_TXE)); // Ждем готовности передатчика
        USART1->TDR = *str++;
    }
}

// Функция для получения символа через UART

