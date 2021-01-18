/*
   SVM40_accessory.c

    Created on: 2021-01-18
        Author: webseitz (Jan Seitz)
*/

#include <homekit/homekit.h>
#include <homekit/characteristics.h>

// Called to identify this accessory. See HAP section 6.7.6 Identify Routine
// Generally this is called when paired successfully or click the "Identify Accessory" button in Home APP.
void my_accessory_identify(homekit_value_t _value) {
  printf("accessory identify\n");
}

// the optional characteristics are: NAME, STATUS_ACTIVE, STATUS_FAULT, STATUS_TAMPERED, STATUS_LOW_BATTERY
// See HAP section 8.41 and characteristics.h

// Device characteristics
homekit_characteristic_t cha_serial_number = HOMEKIT_CHARACTERISTIC_(SERIAL_NUMBER, "01234567");
homekit_characteristic_t cha_firmware_revision = HOMEKIT_CHARACTERISTIC_(FIRMWARE_REVISION, "0.0");
// (optional) format: uint8; HAP section 9.99; 0 "Battery level is normal", 1 "Battery level is low"
// homekit_characteristic_t cha_status_low_battery = HOMEKIT_CHARACTERISTIC_(STATUS_LOW_BATTERY, 0);

// Air Quaility
homekit_characteristic_t cha_airquality  = HOMEKIT_CHARACTERISTIC_(AIR_QUALITY, 0);

// Temperature: format: float; HAP section 9.35; min 0, max 100, step 0.1, unit celsius
homekit_characteristic_t cha_current_temperature = HOMEKIT_CHARACTERISTIC_(CURRENT_TEMPERATURE, 0);

// Humidity
homekit_characteristic_t cha_humidity  = HOMEKIT_CHARACTERISTIC_(CURRENT_RELATIVE_HUMIDITY, 0);



homekit_accessory_t *accessories[] = {
  HOMEKIT_ACCESSORY(.id = 1, .category = homekit_accessory_category_sensor, .services = (homekit_service_t*[]) {
    HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics = (homekit_characteristic_t*[]) {
      HOMEKIT_CHARACTERISTIC(NAME, "SVM40 Development Board"),
      HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Sensirion"),
      &cha_serial_number,
      HOMEKIT_CHARACTERISTIC(MODEL, "SVM40"),
      &cha_firmware_revision,
      HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
      NULL
    }),
    HOMEKIT_SERVICE(AIR_QUALITY_SENSOR, .primary = true, .characteristics = (homekit_characteristic_t*[]) {
      HOMEKIT_CHARACTERISTIC(NAME, "Air Quality (SGP40)"),
                             &cha_airquality,
                             //&cha_status_low_battery,//optional
                             NULL
    }),
    HOMEKIT_SERVICE(TEMPERATURE_SENSOR, .characteristics = (homekit_characteristic_t*[]) {
      HOMEKIT_CHARACTERISTIC(NAME, "Temperature (SHT40)"),
                             &cha_current_temperature,
                             NULL
    }),
    // Add this HOMEKIT_SERVICE if you has a HUMIDITY_SENSOR together
    HOMEKIT_SERVICE(HUMIDITY_SENSOR, .characteristics = (homekit_characteristic_t*[]) {
      HOMEKIT_CHARACTERISTIC(NAME, "Humidity (SHT40)"),
                             &cha_humidity,
                             NULL
    }),

    NULL
  }),
  NULL
};

homekit_server_config_t config = {
  .accessories = accessories,
  .password = "111-11-111"
};
