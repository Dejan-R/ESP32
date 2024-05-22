/*ESP32
dejan.rakijasic@gmail.com

Primjer: PWM - Motor Control Pulse Width Modulator (MCPWM)
MCPWM modul je namijenjen posebno za upravljanje motorima, ali se također može koristiti za općenitu PWM kontrolu. 
Ima više specifičnih postavki za kontrolu motora, poput podrške za različite načine rada motora i podrške za enkodere, 
te se koristi za upravljanje brzinom i smjerom vrtnje (DC) motora, kao i za kontrolu drugih motora (servo, koračni)

detaljnije informacije:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/mcpwm.html
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/mcpwm.h"

#define GPIO_PWM0A_OUT 15
#define GPIO_PWM0B_OUT 4

void app_main(void)
{
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_PWM0A_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, GPIO_PWM0B_OUT);

    mcpwm_config_t pwm_config;
    pwm_config.frequency = 5000;    // Postavljamo frekvenciju na 5 kHz
    pwm_config.cmpr_a = 50.0;       // Postavljamo inicijalni duty cycle na 50%
    pwm_config.cmpr_b = 50.0;
    pwm_config.counter_mode = MCPWM_UP_DOWN_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;

    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);

    int direction = 1;  // 1 - povećava se intenzitet, 0 - smanjuje se intenzitet
    int duty_0 = 50;  // Početni duty cycle za LED_PIN_0
    int duty_1 = 50;  // Početni duty cycle za LED_PIN_1

    while (1) {
        if (direction) {
            duty_0 += 5;  // Povećanje duty cyclea za 5
            duty_1 -= 5;  // Smanjenje duty cyclea za 5
            if (duty_0 > 100 || duty_1 < 0) {
                duty_0 = 100;
                duty_1 = 0;
                direction = 0;
            }
        } else {
            duty_0 -= 5;  // Smanjenje duty cyclea za 5
            duty_1 += 5;  // Povećanje duty cyclea za 5
            if (duty_0 < 0 || duty_1 > 100) {
                duty_0 = 0;
                duty_1 = 100;
                direction = 1;
            }
        }

        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, duty_0);
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, duty_1);
        vTaskDelay(50 / portTICK_PERIOD_MS);  // Vrijeme odmora postavljeno na 50 ms
    }
}