#include "manager.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include <string.h>

static const char *TAG = "wifi_manager";
static EventGroupHandle_t wifi_event_group;
static const int CONNECTED_BIT = BIT0;
static const int FAIL_BIT = BIT1;
static int retry_count = 0;
static int max_retries = 5;

static void event_handler(void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data) {
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
    wifi_event_sta_disconnected_t *disconnected_event =
        (wifi_event_sta_disconnected_t *)event_data;
    ESP_LOGI(TAG, "WiFi disconnected, reason: %d", disconnected_event->reason);

    if (retry_count < max_retries) {
      esp_wifi_connect();
      retry_count++;
      ESP_LOGI(TAG, "Retrying WiFi connection %d/%d", retry_count, max_retries);
    } else {
      ESP_LOGE(TAG, "WiFi connection failed after %d retries", max_retries);
      xEventGroupSetBits(wifi_event_group, FAIL_BIT);
    }
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "Got IP address: " IPSTR, IP2STR(&event->ip_info.ip));
    retry_count = 0;
    xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
  }
}

esp_err_t wifi_manager_init(void) {
  wifi_event_group = xEventGroupCreate();

  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));
  return ESP_OK;
}

esp_err_t wifi_manager_connect(const wifi_config_simple_t *config) {
  wifi_config_t wifi_config = {
      .sta =
          {
              .threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK,
              .pmf_cfg = {.capable = true, .required = false},
          },
  };

  strlcpy(
      (char *)wifi_config.sta.ssid, config->ssid, sizeof(wifi_config.sta.ssid));
  strlcpy((char *)wifi_config.sta.password, config->password,
      sizeof(wifi_config.sta.password));

  ESP_LOGI(TAG, "WiFi config - SSID: %s, Password length: %d",
      wifi_config.sta.ssid, strlen((char *)wifi_config.sta.password));

  max_retries = config->max_retry;
  retry_count = 0;

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
      CONNECTED_BIT | FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

  return (bits & CONNECTED_BIT) ? ESP_OK : ESP_FAIL;
}

bool wifi_manager_is_connected(void) {
  EventBits_t bits = xEventGroupGetBits(wifi_event_group);
  return (bits & CONNECTED_BIT) != 0;
}

esp_err_t wifi_manager_scan_networks(void) {
  ESP_LOGI(TAG, "Starting WiFi scan...");
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());

  wifi_scan_config_t scan_config = {
      .ssid = NULL, .bssid = NULL, .channel = 0, .show_hidden = false};

  ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));

  uint16_t ap_count = 0;
  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));

  ESP_LOGI(TAG, "Found %d access points:", ap_count);

  if (ap_count > 0) {
    wifi_ap_record_t *ap_info = malloc(sizeof(wifi_ap_record_t) * ap_count);
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_count, ap_info));

    for (int i = 0; i < ap_count; i++) {
      ESP_LOGI(TAG, "AP %d: SSID=%s, RSSI=%d, Auth=%d", i, ap_info[i].ssid,
          ap_info[i].rssi, ap_info[i].authmode);
    }
    free(ap_info);
  }

  esp_wifi_stop();
  return ESP_OK;
}

void wifi_manager_disconnect(void) { esp_wifi_stop(); }