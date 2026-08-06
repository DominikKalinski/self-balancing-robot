#include "flash-storage.h"

FlashStorage& FlashStorage::instance()
{
    static FlashStorage storage;
    return storage;
}

FlashStorage::FlashStorage()
{
    esp_err_t error = nvs_flash_init();
    if(error == ESP_ERR_NVS_NO_FREE_PAGES || error == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }
}

void FlashStorage::save_to_flash(const char* key, float value)
{
    nvs_handle_t handle;
    size_t size = sizeof(value);
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &handle));
    ESP_ERROR_CHECK(nvs_set_blob(handle, key, &value, size ));

    ESP_ERROR_CHECK(nvs_commit(handle));
    nvs_close(handle);
}

float FlashStorage::load_from_flash(const char* key)
{
    nvs_handle_t handle;
    float value = 0.f;
    size_t size = sizeof(value);

    esp_err_t result0 = nvs_open("storage", NVS_READONLY, &handle);
    if(result0 == ESP_ERR_NVS_NOT_FOUND)
    {
        nvs_close(handle);
        return 0.0f;
    }
    ESP_ERROR_CHECK(result0);
    esp_err_t result = nvs_get_blob(handle, key, &value, &size);


    if(result == ESP_ERR_NVS_NOT_FOUND)
    {
        nvs_close(handle);
        return 0.0f;
    }
    ESP_ERROR_CHECK(result);
    nvs_close(handle);
    return value;
}