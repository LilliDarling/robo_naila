#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_heap_caps.h>

#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <tensorflow/lite/version.h>


// Tag for ESP_LOG messages
static const char* TAG = "MAIN_APP";

// --- Global TFLM Objects ---


// --- Global Instances ---


// Main Entry Point
extern "C" void app_main() {
  ESP_LOGI("MAIN", "Starting Wake Word Detection Application...");

  // 1. Allocate tensor arena from PSRAM (SPIRAM)

  // 2. Load the Model

  // 3. Populate the OpResolver with necessary operations

  // 4. Build an Interpreter

  // 5. Initialize Audio and Feature Providers

  // 6. Main Inference Loop
}