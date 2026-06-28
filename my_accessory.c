#include <homekit/homekit.h>
#include <homekit/characteristics.h>

void my_accessory_identify(homekit_value_t _value);

// Lamba durumunu tutan ana karakteristik
homekit_characteristic_t cha_on = HOMEKIT_CHARACTERISTIC_(ON, false);

// Aksesuarın kimlik bilgileri (Metadata)
homekit_accessory_t *accessories[] = {
    HOMEKIT_ACCESSORY(.id=1, .category=homekit_accessory_category_lightbulb, .services=(homekit_service_t*[]) {
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
            HOMEKIT_CHARACTERISTIC(NAME, "Akilli Duvardan Anahtar"),
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "DEU EEE - Berkay"),
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "0123456"),
            HOMEKIT_CHARACTERISTIC(MODEL, "ESP32-C3 v1.0"),
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "1.1"),
            HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
            NULL
        }),
        HOMEKIT_SERVICE(LIGHTBULB, .primary=true, .characteristics=(homekit_characteristic_t*[]) {
            &cha_on,
            HOMEKIT_CHARACTERISTIC(NAME, "Oda Lambasi"),
            NULL
        }),
        NULL
    }),
    NULL
};

homekit_server_config_t config = {
    .accessories = accessories,
    .password = "466-37-726" // Apple Ev uygulamasında gireceğin QR/Eşleşme kodu
};
