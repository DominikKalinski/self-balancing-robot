#pragma once
#include "nvs_flash.h"
#include "nvs.h"
class FlashStorage
{
    public:
    static FlashStorage& instance();
    void save_to_flash(const char*, float);
    float load_from_flash(const char*);
    FlashStorage(const FlashStorage&) = delete;
    FlashStorage& operator=(const FlashStorage&) = delete;
    private:
    FlashStorage();
};