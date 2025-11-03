#include "hello.h"
#include "esp_log.h"

static const char *TAG = "HELLO_LIB";

void hello_message(void)
{
#ifdef CONFIG_ENABLE_HELLO
    // Ispis poruke ako je ENABLE_HELLO uključen
    ESP_LOGI(TAG, "Hello from component!");
#else
    ESP_LOGW(TAG, "Hello message disabled (CONFIG_ENABLE_HELLO=n)");
#endif
}
