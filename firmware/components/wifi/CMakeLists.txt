idf_component_register(
    SRCS "manager.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_wifi nvs_flash
)