#include "main.h"


void ProcessBuffer(void) {
    // Копируем данные из buffer в buffer_for_split до символа '\n'
    int i;
    for (i = 0; i < sizeof(buffer) && buffer[i] != '\n'; i++) {
        buffer_for_split[i] = buffer[i];
    }
    buffer_for_split[i] = '\0'; // Завершаем строку

    // Очищаем буферы
    memset(buffer, 0, BUFFER_SIZE);
    memset(buffer_of_ints, 0, BUFFER_SIZE_SPLIT);
    buffer_index = 0;

    // Разделяем строку на числа
    char* token = strtok(buffer_for_split, ",");
    while (token != NULL && count < BUFFER_SIZE_SPLIT) {
        buffer_of_ints[count] = atoi(token);
        count++;
        token = strtok(NULL, ",");
    }
    count = 0; // Сбрасываем счетчик
}

char UART_ReceiveChar(void) {
    while (!(USART1->ISR & USART_ISR_RXNE));
    return USART1->RDR;
}

//void Pin_Control(uint8_t pin, uint8_t enable) {
//    // Включение тактирования порта C
//    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

//    // Настройка выбранного пина как выход
//    GPIOC->MODER &= ~(3 << (pin * 2)); // Сброс режима
//    GPIOC->MODER |= (1 << (pin * 2));  // Режим выхода

//    // Управление состоянием пина
//    if (enable) {
//        GPIOC->BSRR = (1 << pin);  // Установка пина в HIGH
//    } else {
//        GPIOC->BSRR = (1 << (pin + 16)); // Установка пина в LOW
//    }
//}



void setup(void) {
    // Настройка пинов и периферии
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER |= GPIO_MODER_MODER6_0; // Настройка PC6 как выхода
    TimeR = Piece * T;
    settings = 0;
	
}

int main(void) {
    setup();
    UART_Init();
		TIM1_setup();
		TIM2_setup();
		
    while (1) {
        // Принимаем символ по UART
        if (USART1->ISR & USART_ISR_RXNE) {
            char receivedChar = UART_ReceiveChar();

            // Сохраняем символ в буфер
            if (buffer_index < BUFFER_SIZE - 1) {
                buffer[buffer_index++] = receivedChar;
            }

            // Обработка буфера при получении символа новой строки или переполнении
            if (receivedChar == '\n' || buffer_index >= BUFFER_SIZE - 1) {
                buffer[buffer_index] = '\0'; // Завершаем строку
                ProcessBuffer(); // Обрабатываем буфер
            }
        }

        // Обработка команд
        if (buffer_of_ints[0] == 0) {
            if (buffer_of_ints[1] == 0) {
                GPIOC->ODR &= ~GPIO_ODR_6; // Выключение светодиода
            } else {
                GPIOC->ODR |= GPIO_ODR_6; // Включение светодиода
            }
        } else if (buffer_of_ints[0] == 1 && settings == 0) {
            I = buffer_of_ints[1];
            LU = I * L;
            TimeZap = (LU / 0.3) * 1000;
            TI = I * T;
            tau = TI / 346;
            D = tau / TimeR;
            TimetoNext = buffer_of_ints[2];
            ActiveTime = buffer_of_ints[3];
        } else if (buffer_of_ints[0] == 2 && settings == 0) {
            mode_signal = buffer_of_ints[1];
        } else if (buffer_of_ints[0] == 3) {
            flag = buffer_of_ints[1];
        }

        // Логика работы с режимами сигналов
        if (flag == 1) {
					
					
					TIM1->CR1 |= TIM_CR1_CEN;

					
					
//                switch (mode_signal) 
//								{
//									
//                }
}
            

            // Выход из цикла, если flag == 0
            //settings = 0;
//            Startuem = 0;
//            PWM_Control(TIM2, PWM_LV, LOW);
//            PWM_Control(TIM2, PWM_LN, LOW);
//            Pin_Control(LV, LOW);
//            Pin_Control(LN, LOW);
//            Pin_Control(PV, LOW);
//            Pin_Control(PN, LOW);
//        }
    }
}

void TIM2_IRQHandler(void)
{
	TIM2->SR &= ~TIM_SR_UIF;
	counter++;
	
	if(counter > 4)
	{
		//TIM2->CCR2 = 0;
		TIM2->CCER &= ~TIM_CCER_CC2E;
		counter = 0;
	}
}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
	TIM1->SR &= ~TIM_SR_UIF;
	TIM1->CCER &= ~TIM_CCER_CC1E;
	TIM2->CR1 |= TIM_CR1_CEN;
	TIM2->CCER |= TIM_CCER_CC2E; //включение 2 канала
	
}