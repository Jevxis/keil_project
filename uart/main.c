#include "main.h"

int vhod;
int first_inter_tim6;
int first_inter_tim7;
int cycle = 1;
int dlit_PWM;
int next_impulse;
int global_time_zap;
int flag_for_plus_odnopolyarnii;
int flag_for_minus_odnopolyarnii;
int flag_for_dvupolyarnii_plus;
int flag_for_dvupolyarnii_minus;
int in_work;
float pole;
float temp;
uint16_t pole_V;
uint32_t temp_v;
char tx_buf[64];
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

void odnopolyarnii_polojitelnii(int time_PWM, int time_next_impulse, int time_zap)
{
	/*
	ПН вкл
	запуск ЛВ на время
	запуск ЛВ ШИМ на время
	ПН выкл
	Задержка на время
	*/
	in_work = 1;
	flag_for_plus_odnopolyarnii = 1;

	dlit_PWM = time_PWM; // задаем длительность ШИМ с ЛВ (работает корректно)
	next_impulse = time_next_impulse; //не корректно по времени
	global_time_zap = time_zap;
	//GPIOA->ODR |= GPIO_ODR_5; // ПН вкл
	TIM2->CNT = 0;
	TIM2_CH_2_start(global_time_zap); // запуск ЛВ на 180 мкс
}

void odnopolyarnii_otricatelnii(int time_PWM, int time_next_impulse, int time_zap)
{
	/*
	ЛН вкл
	запуск ПВ на время
	запуск ПВ ШИМ на время
	ЛН выкл
	Задержка на время
	*/
	flag_for_minus_odnopolyarnii = 1;
	dlit_PWM = time_PWM; // задаем длительность ШИМ с ПВ (работает корректно)
	next_impulse = time_next_impulse; //не корректно по времени
	global_time_zap = time_zap;
	GPIOA->ODR |= GPIO_ODR_6; // ЛН вкл
	TIM2->CNT = 0;
	TIM2_CH_3_start(time_zap); // запуск ПВ на 180 мкс
}

void dvupolyarnii(int time_PWM, int time_next_impulse, int time_zap)
{
	/*
	ПН вкл
	запуск ЛВ на время
	запуск ЛВ ШИМ на время
	ПН выкл
	Задержка на время
	
	ЛН вкл
	запуск ПВ на время
	запуск ПВ ШИМ на время
	ЛН выкл
	Задержка на время
	*/
	in_work = 1;
	flag_for_dvupolyarnii_plus = 1;
	dlit_PWM = time_PWM; // задаем длительность ШИМ с ЛВ (работает корректно)
	next_impulse = time_next_impulse; //не корректно по времени
	global_time_zap = time_zap;
	//GPIOA->ODR |= GPIO_ODR_5; // ПН вкл
	TIM2->CNT = 0;
	TIM2_CH_2_start(time_zap); // запуск ЛВ на 180 мкс
}

int main(void) 
{
    setup();
    UART_Init();
		SystemClock_Config();
		ports_for_timers_init();
		TIM2_setup();
		TIM3_setup();
		TIM6_setup();
		TIM7_setup();
		//in_work = 0;
		config_DAC();
		odnopolyarnii_polojitelnii(200, 100, 400);//time_PWM, time_next_impulse, time_zap
		
    while (1) 
			{
//				if(in_work == 0)
//				{
//					odnopolyarnii_polojitelnii(100, 100, 200);//time_PWM, time_next_impulse, time_zap
//				}
				uint16_t hall_value = Read_ADC(0); //Pole
				// Расчет магнитного поля
				pole = (((hall_value * 4.0f) / 4096.0f) - 1.98f) / 0.0004f;
				pole_V = ((hall_value * 4.2f) / 4096.0f);
				uint16_t temp_value = Read_ADC(2); //temp
				
				//temp = (((temp_value * 4.25f) / 4096.0f) - 1.25f) * 50.0f;
				temp = (((temp_value * 3.0f) / 4095.0f) - 1.45f ) * 16.0f;
				temp_v =  (temp_value * 3.0f) / 4095.0f;
				
				sprintf(tx_buf, "%.2f,%.2f\n", pole, temp); 
				UART_SendString(tx_buf);

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
            if (buffer_of_ints[1] == 0) 
						{
                //GPIOC->ODR &= ~GPIO_ODR_6; // Выключение светодиода
            } else 
						{
                //GPIOC->ODR |= GPIO_ODR_6; // Включение светодиода
            }
        } else if (buffer_of_ints[0] == 1 && settings == 0) 
				{
            I = buffer_of_ints[1];
            LU = I * L;
            TimeZap = (LU / 0.3) * 1000;
            TI = I * T;
            tau = TI / 346;
            D = tau / TimeR;
            TimetoNext = buffer_of_ints[2];
            ActiveTime = buffer_of_ints[3];
						global_time_zap = TimeZap;
						dlit_PWM = ActiveTime;
						next_impulse = TimetoNext;
        } else if (buffer_of_ints[0] == 2 && settings == 0) 
				{
            mode_signal = buffer_of_ints[1];
        } else if (buffer_of_ints[0] == 3) 
				{
            flag = buffer_of_ints[1];
        }
// Логика работы с режимами сигналов
        if (flag == 1 && in_work == 0) 
				{
					if(mode_signal == 0)
					{
						//---------Однопололярный + -------------
							in_work = 0;
							odnopolyarnii_polojitelnii(ActiveTime, TimetoNext, TimeZap);
					}
				}
//				else if(flag == 0)
//				{
//					vhod = 0;
//					off_all_channels();
//				}
    }
}

void TIM2_IRQHandler(void)
{
	if (TIM2->SR & TIM_SR_UIF) 
	{
		TIM2->SR &= ~TIM_SR_UIF;
		

		if(flag_for_plus_odnopolyarnii == 1)
		{
			TIM3->CNT = 0;
		
			TIM3->CCER |= TIM_CCER_CC1E;
			//GPIOA->ODR |= GPIO_ODR_5; //включение ПН тест
			start_counting_time_mks(dlit_PWM);
			TIM2->CNT = 0;
			TIM2->CR1 &= ~TIM_CR1_CEN;

		}
		else if(flag_for_minus_odnopolyarnii == 1)
		{
			TIM3->CCER |= TIM_CCER_CC2E;
			start_counting_time_mks(dlit_PWM);
		}
		else if(flag_for_dvupolyarnii_plus == 1)
		{
			TIM3->CCER |= TIM_CCER_CC1E;
			start_counting_time_mks(dlit_PWM);
		}
		else if(flag_for_dvupolyarnii_minus == 1)
		{
			TIM3->CCER |= TIM_CCER_CC2E; //запуск ПВ ШИМ
			start_counting_time_mks(dlit_PWM);
		}
		
	}
}

void TIM6_DAC_IRQHandler(void)
{ 
	if (TIM6->SR & TIM_SR_UIF)
	{
		TIM6->SR &= ~TIM_SR_UIF;  // Сброс флага
		if(flag_for_plus_odnopolyarnii == 1 && (TIM2->CR1 & TIM_CR1_CEN) == 0)
		{
			TIM3->CNT = 0;
			TIM3->CCER &= ~TIM_CCER_CC1E;
			start_count_for_delay(next_impulse);
			TIM2_CH_2_start(global_time_zap);
			TIM6->CR1 &= ~TIM_CR1_CEN;
			GPIOA->ODR |= GPIO_ODR_5;
		}
//	if(first_inter_tim6 == 1 && flag_for_minus_odnopolyarnii == 1)
//	{
//		GPIOA->ODR &= ~GPIO_ODR_6; //выключение ЛН
//		TIM3->CCER &= ~TIM_CCER_CC2E;
//		TIM6->CR1 &= ~TIM_CR1_CEN;
//		start_count_for_delay(next_impulse);
//	}
//	if(first_inter_tim6 == 1 && flag_for_dvupolyarnii_plus == 1)
//	{
//		//GPIOA->ODR &= ~GPIO_ODR_5; //выключение ПН
//		TIM3->CCER &= ~TIM_CCER_CC1E;
//		TIM6->CR1 &= ~TIM_CR1_CEN;
//		start_count_for_delay(next_impulse);
//	}
//	if(first_inter_tim6 == 1 && flag_for_dvupolyarnii_minus == 1)
//	{
//		GPIOA->ODR &= ~GPIO_ODR_6; //выключение ЛН
//		TIM3->CCER &= ~TIM_CCER_CC2E;
//		TIM6->CR1 &= ~TIM_CR1_CEN;
//		start_count_for_delay(next_impulse);
//	}
		//TIM6->CR1 &= ~TIM_CR1_CEN;
	}
	
}

void TIM7_IRQHandler(void)
{
	
	if (TIM7->SR & TIM_SR_UIF)
	{
		TIM7->SR &= ~TIM_SR_UIF;  // Сброс флага
		GPIOA->ODR &= ~GPIO_ODR_5;
//		if(flag_for_plus_odnopolyarnii == 1 && (TIM6->CR1 & TIM_CR1_CEN) == 0)
//		{
////			TIM2->CR1 |= TIM_CR1_CEN;
////			TIM2->CNT = 0;
////			TIM2->CCER |= TIM_CCER_CC2E;
//			//TIM7->CNT = 0;
//			GPIOA->ODR &= ~GPIO_ODR_5;
//			TIM7->CR1 &= ~TIM_CR1_CEN;
//			//включить тут ПН?
//			
		}
////	else if(first_inter_tim7 == 1 && flag_for_minus_odnopolyarnii == 1)
////	{
////		TIM2->CR1 |= TIM_CR1_CEN;
////		TIM2->CNT = 0;
////		TIM2->CCER |= TIM_CCER_CC3E;
////		TIM7->CR1 &= ~TIM_CR1_CEN;
////		GPIOA->ODR |= GPIO_ODR_6;
////		//включить тут ЛН?
////	}
////	else if(first_inter_tim7 == 1 && flag_for_dvupolyarnii_plus == 1)
////	{
////		flag_for_dvupolyarnii_plus = 0;
////		flag_for_dvupolyarnii_minus = 1;
////		
////		GPIOA->ODR |= GPIO_ODR_6; // включение ЛН
////		TIM2_CH_3_start(global_time_zap); // включение ПВ ШИМ 
////		TIM7->CR1 &= ~TIM_CR1_CEN;
////	}
////	else if(first_inter_tim7 == 1 && flag_for_dvupolyarnii_minus == 1)
////	{
////		flag_for_dvupolyarnii_plus = 1;
////		flag_for_dvupolyarnii_minus = 0;
////		//GPIOA->ODR |= GPIO_ODR_5; // включение ПН
////		TIM2_CH_2_start(global_time_zap);
////		TIM7->CNT = 0;
////		TIM7->CR1 &= ~TIM_CR1_CEN;
////	}
//	}
}
