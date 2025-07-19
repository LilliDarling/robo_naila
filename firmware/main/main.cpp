#include <esp_event.h>
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>

#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <tensorflow/lite/schema/schema_generated.h>

extern "C" {
#include "../components/wifi/include/manager.h"
#include "config.h"
}

// Tag for ESP_LOG messages
static const char *TAG = "MAIN_APP";

// --- Global TFLM Objects ---

// --- Global Instances ---

// WiFi initialization function
static esp_err_t initialize_wifi() {
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Initialize network interface
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  // Initialize WiFi and scan networks
  ESP_LOGI(TAG, "Initializing WiFi...");
  ESP_ERROR_CHECK(wifi_manager_init());

  // Scan for available networks first
  ESP_LOGI(TAG, "Scanning for available networks...");
  ESP_ERROR_CHECK(wifi_manager_scan_networks());

  wifi_config_simple_t wifi_config = {.ssid = CONFIG_WIFI_SSID,
      .password = CONFIG_WIFI_PASSWORD,
      .max_retry = CONFIG_WIFI_MAXIMUM_RETRY};

  ESP_LOGI(TAG, "Connecting to WiFi SSID: %s", wifi_config.ssid);
  esp_err_t wifi_result = wifi_manager_connect(&wifi_config);
  if (wifi_result != ESP_OK) {
    ESP_LOGE(TAG, "Failed to connect to WiFi (error: 0x%x)", wifi_result);
    return wifi_result;
  }
  ESP_LOGI(TAG, "WiFi connected successfully!");

  return ESP_OK;
}

// Main Entry Point
extern "C" void app_main() {
  ESP_LOGI(TAG, "Starting AI Robot Application...");

  // 1. Initialize WiFi and networking
  ESP_LOGI(TAG, "Initializing WiFi...");
  if (initialize_wifi() != ESP_OK) {
    ESP_LOGE(TAG, "WiFi initialization failed!");
    return;
  }

  // 2. Allocate tensor arena from PSRAM (SPIRAM)

  // 3. Load the Model

  // 4. Populate the OpResolver with necessary operations

  // 5. Build an Interpreter

  // 6. Initialize Audio and Feature Providers

  // 7. Main Inference Loop
  while (1) {
    if (wifi_manager_is_connected()) {
      ESP_LOGI(TAG, "WiFi connected, ready for AI processing");
      // TODO: Add TensorFlow Lite model inference here
    } else {
      ESP_LOGE(TAG, "WiFi disconnected!");
    }
    vTaskDelay(pdMS_TO_TICKS(10000)); // Check every 10 seconds
  }
}