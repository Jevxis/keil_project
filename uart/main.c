#include "main.h"

int vhod;
int first_inter_tim6;
int first_inter_tim7;
int cycle = 1;
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



void setup(void) {
    // Настройка пинов и периферии
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER |= GPIO_MODER_MODER6_0; // Настройка PC6 как выхода
    TimeR = Piece * T;
    settings = 0;

	
}
void SystemClock_Config(void) {
    // 1. Включить HSI (8 МГц)
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));

    // 2. Настройка PLL: (HSI / 2) * 12 = 48 МГц
    RCC->CFGR &= ~RCC_CFGR_PLLMUL;  // Сбросить биты умножения PLL
    RCC->CFGR |= RCC_CFGR_PLLSRC_HSI_DIV2;  // Источник PLL = HSI / 2 (4 МГц)
    RCC->CFGR |= RCC_CFGR_PLLMUL12;         // Умножение на 12 (4 МГц * 12 = 48 МГц)

    // 3. Включить PLL и ждать готовности
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // 4. Переключить SYSCLK на PLL
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    // 5. Настройка делителей шин (по умолчанию AHB = SYSCLK, APB = AHB)
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE);  // HPRE=1, PPRE=1 (48 МГц)
}

int main(void) 
{
		first_inter_tim6 = 0;
		first_inter_tim7 = 0;
    setup();
    UART_Init();
		SystemClock_Config();
		ports_for_timers_init();
	TIM2_setup(180); //функция принимает время длительности запускающего испульса
		TIM3_setup();
		TIM6_setup();
		TIM7_setup();
		
		//TIM2_start();
		
		TIM2->CR1 |= TIM_CR1_CEN;
		TIM2->CCER |= TIM_CCER_CC2E;
		NVIC_EnableIRQ(TIM2_IRQn);
		
    while (1) 
			{
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
				if(cycle == 1)
				{
					GPIOA->ODR |= GPIO_ODR_5; //включение PA5 (типа ЛН)
					cycle = 0;
				}
// Логика работы с режимами сигналов
//        if (flag == 1) 
//				{
//					
//				}
            


    }
}

void TIM2_IRQHandler(void)
{
	if (TIM2->SR & TIM_SR_UIF) 
	{
		//GPIOA->ODR |= GPIO_ODR_5; //включение PA5 (типа ЛН)
		TIM2->SR &= ~TIM_SR_UIF;
		TIM3->CCER |= TIM_CCER_CC1E;
		TIM3->CR1 |= TIM_CR1_CEN;
		start_counting_time_mks(220); // сколько будет длиться ШИМ сигнал
	}
	
}

void TIM6_DAC_IRQHandler(void)
{ 
	if (TIM6->SR & TIM_SR_UIF)
	{
		TIM6->SR &= ~TIM_SR_UIF;  // Сброс флага
	}
	
	if(first_inter_tim6 == 1)
	{
		TIM3->CNT = 0;
		TIM3->EGR |= TIM_EGR_UG;
		TIM3->CR1 &= ~TIM_CR1_CEN;
		TIM6->CR1 &= ~TIM_CR1_CEN;
		GPIOA->ODR &= ~GPIO_ODR_5; //выключение ЛН или ПН
		//задержка на tdelay(через 7 таймер)
		TIM7->CR1 |= TIM_CR1_CEN;
	}
	first_inter_tim6 = 1;
}

void TIM7_IRQHandler(void)
{
	if (TIM7->SR & TIM_SR_UIF)
	{
		TIM7->SR &= ~TIM_SR_UIF;  // Сброс флага
	}
	if(first_inter_tim7 == 1)
	{
		//TIM2->CNT = 0;
		//TIM2->SR &= ~TIM_SR_UIF;
		TIM2->CR1 |= TIM_CR1_CEN;
		TIM2->CCER |= TIM_CCER_CC2E;
		//NVIC_EnableIRQ(TIM2_IRQn);
		
		TIM7->CR1 &= ~TIM_CR1_CEN;
	}
	cycle = 1;
	first_inter_tim7 = 1;
}
