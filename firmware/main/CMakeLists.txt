
# Get WiFi credentials from environment variables (set by setenv.sh)
if(DEFINED ENV{CONFIG_EXAMPLE_WIFI_SSID})
    set(WIFI_SSID $ENV{CONFIG_EXAMPLE_WIFI_SSID})
else()
    message(FATAL_ERROR "CONFIG_EXAMPLE_WIFI_SSID not set - run 'source setenv.sh' first")
endif()

if(DEFINED ENV{CONFIG_EXAMPLE_WIFI_PASSWORD})
    set(WIFI_PASSWORD $ENV{CONFIG_EXAMPLE_WIFI_PASSWORD})
else()
    message(FATAL_ERROR "CONFIG_EXAMPLE_WIFI_PASSWORD not set - run 'source setenv.sh' first")
endif()

idf_component_register(SRCS "main.cpp"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES "esp_wifi"
                                     "nvs_flash"
                                     "esp_netif"
                                     "esp_event"
                                     "wifi"
                                     "espressif__esp-tflite-micro"
                      )

# Set compile definitions for WiFi credentials
target_compile_definitions(${COMPONENT_LIB} PRIVATE
    CONFIG_EXAMPLE_WIFI_SSID="${WIFI_SSID}"
    CONFIG_EXAMPLE_WIFI_PASSWORD="${WIFI_PASSWORD}"
)