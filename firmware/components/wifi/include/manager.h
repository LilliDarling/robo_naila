#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>

typedef struct {
  const char *ssid;
  const char *password;
  int max_retry;
} wifi_config_simple_t;

esp_err_t wifi_manager_init(void);
esp_err_t wifi_manager_scan_networks(void);
esp_err_t wifi_manager_connect(const wifi_config_simple_t *config);
bool wifi_manager_is_connected(void);
void wifi_manager_disconnect(void);

#endif