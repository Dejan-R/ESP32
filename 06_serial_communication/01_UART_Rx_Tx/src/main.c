/* dejan.rakijasic@gmail.com

primjer: UART - asinkroni način rada
U(S)ART (engl. Universal (Synchronous and) Asynchronous serial Receiver/Transmitter)

 - FULL-DUPLEX komunikacija
 - point-to-point topologija
 - dvije linije (komunikacijska kanala) za asinkroni način rada (Null modem connection): TxD (Transmit data) i RxD (Receive data)

Ovaj primjer baziran je na UART_NUM_0 sučelju (TXD_PIN 1 i RXD_PIN 3).

detaljnije o UART-u:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/uart.html

*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

static const int RX_BUF_SIZE = 1024;

#define TXD_PIN 1
#define RXD_PIN 3

void uart_inicijalizacija(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_0, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

//ESP32 šalje podatke
static void tx_task(void *arg)
{
    char *podaci = "UART primjer - pozdrav od ESP32.\n";
    
    while (1)
    {
        uart_write_bytes(UART_NUM_0, podaci, strlen(podaci));
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

//ESP32 prima podatke
static void rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* podaci = (uint8_t*) malloc(RX_BUF_SIZE + 1);
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_0, podaci, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
        if (rxBytes > 0) {
            podaci[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, podaci);
            ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, podaci, rxBytes, ESP_LOG_INFO);
        }
    }
    free(podaci);
}

void app_main(void)
{
    uart_inicijalizacija();
    xTaskCreate(rx_task, "uart_rx_task", 1024 * 2, NULL, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(tx_task, "uart_tx_task", 1024 * 2, NULL, configMAX_PRIORITIES - 2, NULL);
}