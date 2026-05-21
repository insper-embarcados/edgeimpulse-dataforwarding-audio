#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "mpu6050.h"

#include "Fusion.h"
#define SAMPLE_PERIOD (0.01f) // replace this with actual sample period

const int MPU_ADDRESS = 0x68;
const int I2C_SDA_GPIO = 4;
const int I2C_SCL_GPIO = 5;


#define ADC_NUM 0
#define ADC_PIN (26 + ADC_NUM)
#define ADC_VREF 3.3
#define ADC_RANGE (1 << 12)
#define ADC_CONVERT (ADC_VREF / (ADC_RANGE - 1))

volatile int g_timer_0 = 0;

bool timer_0_callback(repeating_timer_t *rt) {
    g_timer_0 = 1;
    return true; // keep repeating
}

void mic_task(void *p) {
    uint adc_raw;
    while(1) {

        // vTaskDelay(62.5);  // 62.5 us delay for 16kHz sampling rate (1/16000 = 0.0000625 seconds)
        // sleep_us(62.5); 


        if(g_timer_0){
            adc_raw = adc_read(); // raw voltage from ADC
            printf("%.2f\n", adc_raw * ADC_CONVERT);
            g_timer_0 = 0;
        }
    }
}

int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init( ADC_PIN);
    adc_select_input( ADC_NUM);

    repeating_timer_t timer_0;

    if (!add_repeating_timer_us(62.5, 
                                timer_0_callback,
                                NULL, 
                                &timer_0)) {
        printf("Failed to add timer\n");
    }

    xTaskCreate(mic_task, "Mic_Task", 8192, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
