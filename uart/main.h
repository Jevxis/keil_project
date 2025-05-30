#include "stm32f0xx.h"
#include <string.h>
#include <stdlib.h>
#include "uart_init.h"
#include "pwm_init.h"
#include "ADC_Holl.h"
#include <stdio.h>
#define BUFFER_SIZE 128
#define BUFFER_SIZE_SPLIT 16
#define LV 0
#define LN 3
#define PV 4
#define PN 5
#define LOW 0
#define HIGH 1
#define PWM_LV 1
#define PWM_LN 2
#define L 0.00273; // Индуктивность одного соленоида
#define DELAY_REQUEST_BIT (1 << 0)
#define DELAY_COMPLETE_BIT (1 << 1)
//----функции----
void ProcessBuffer(void);
void parserArray(char* array, char* string, size_t size);
void setup(void);
//void Pin_Control(uint8_t pin, uint8_t enable);
// Буфер для хранения данных
char buffer_for_split[BUFFER_SIZE_SPLIT];
char buffer[BUFFER_SIZE];
uint8_t buffer_index = 0;
int buffer_of_ints[100];
//----переменные----
int U = 300; // Напряженеие подаваемое на соленоиды
int T = 64; // Время периода в мкс на частоте 15,6 кГц
float Piece = 0.001; // 1/1023 - одно деление для таймера 10 бит
int count = 0;

int I1;
int I; // значение тока, определяемое пользователем, мА
volatile uint64_t TimetoNext;
volatile uint64_t ActiveTime;
int flag;
int mode_signal;
int Startuem;
int settings;

float tau;
float TI;
float D;
float TimeR;
float LU;
volatile float TimeZap;

volatile int firstInter = 0;
int counter = 0;