idf_component_register(SRCS "main.cpp"
                            "audio_provider.cpp"
                            "feature_provider.cpp"
                            "command_responder.cpp"
                       INCLUDE_DIRS "."
                       "$<TARGET_PROPERTY:esp-tflite-micro,INCLUDE_DIRS>/tflite-lib/tensorflow/lite/micro/examples/micro_speech/micro_features_generator"
                       "$<TARGET_PROPERTY:esp-tflite-micro,INCLUDE_DIRS>/tflite-lib/tensorflow/lite/micro/examples/micro_speech/mfcc"
                       "$<TARGET_PROPERTY:esp-tflite-micro,INCLUDE_DIRS>/tflite-lib/tensorflow/lite/micro/examples/micro_speech/spectrogram"

                       PRIV_REQUIRES "esp-tflite-micro"
                                     "driver"       # For I2S driver
                                     "esp_heap_caps" # For PSRAM allocation
                                     "esp_timer"    # Might be useful for timing
                                     "log"          # For ESP_LOGx
                      )