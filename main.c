#include "stm32f10x.h"
#include <stdio.h>

void UART1_Init(uint32_t baud);
void UART1_SendStr(char *s);
void ADC1_Init(void);
uint16_t ADC1_Read_Avg(void);
void delay_ms(uint32_t ms);

int main(void)
{
    SystemInit();
    UART1_Init(9600);
    ADC1_Init();

    char buffer[64];
    uint16_t adc_value;
    float voltage;

    while (1)
    {
        adc_value = ADC1_Read_Avg();                        // Ðoc gia tri ADC
        voltage = (adc_value * 3300.0f) / 4095.0f;          // mV

        sprintf(buffer, "ADC=%u  Voltage=%.2f mV\r\n", adc_value, voltage);
        UART1_SendStr(buffer);

        delay_ms(500);
    }
}

void UART1_Init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    // PA9 (TX)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // PA10 (RX)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART_InitStruct.USART_BaudRate = baud;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStruct);

    USART_Cmd(USART1, ENABLE);
}

void UART1_SendStr(char *s)
{
    while (*s)
    {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, *s++);
    }
}

void ADC1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   // ADC clk = 72/6 = 12 MHz

    // PA0 (Analog)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStruct);

    ADC_Cmd(ADC1, ENABLE);

   
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

uint16_t ADC1_Read_Avg(void)
{
    uint32_t sum = 0;
    const int N = 16;
    for(int i=0;i<N;i++){
        ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
        sum += ADC_GetConversionValue(ADC1);
    }
    return (uint16_t)(sum / N);
}


void delay_ms(uint32_t ms)
{
    SysTick->LOAD = 72000 - 1;      // 1ms @72MHz
    SysTick->VAL = 0;
    SysTick->CTRL = 5;
    for (uint32_t i = 0; i < ms; i++)
    {
        while (!(SysTick->CTRL & (1 << 16)));
    }
    SysTick->CTRL = 0;
}
