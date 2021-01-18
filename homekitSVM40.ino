#include <Arduino.h>
#include <arduino_homekit_server.h>
#include "wifi_info.h"
#include "svm40.h"
#include <Wire.h>

#define LOG_D(fmt, ...)   printf_P(PSTR(fmt "\n") , ##__VA_ARGS__);

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    delay(100);
  }

  // ---- Init Homekit ----
  wifi_connect(); // in wifi_info.h
  my_homekit_setup();

  // ---- Init SVM40 ----
  int16_t err;

  const char* driver_version = svm40_get_driver_version();
  if (driver_version) {
    Serial.print("SVM40 driver version: ");
    Serial.println(driver_version);
  } else {
    Serial.println("fatal: Getting driver version failed");
  } 

  /* Initialize I2C bus */
  Wire.begin();

  /* Busy loop for initialization. The main loop does not work without
     a sensor. */
  while (svm40_probe() != NO_ERROR) {
    Serial.println("SVM40 sensor probing failed");
    delay(1000);
  }
  Serial.println("SVM40 sensor probing successful");

  svm40_version_information version_information;
  err = svm40_get_version(&version_information);
  if (err) {
    Serial.print("Error reading SVM40 version: ");
    Serial.println(err);
  } else {
    if (version_information.firmware_debug) {
      Serial.print("Development firmware version: ");
    }
    Serial.print("FW: ");
    Serial.print((int)version_information.firmware_major);
    Serial.print(".");
    Serial.print((int)version_information.firmware_minor);
 
    Serial.print(", HW: ");
    Serial.print((int)version_information.hardware_major);
    Serial.print(".");
    Serial.print((int)version_information.hardware_minor);

    Serial.print(", protocol: ");
    Serial.print((int)version_information.protocol_major);
    Serial.print(".");
    Serial.println((int)version_information.protocol_minor);

    // check if firmware is older than 2.2
    if (version_information.firmware_major < 2 ||
        (version_information.firmware_major == 2 &&
         version_information.firmware_minor < 2)) {
      Serial.println("Warning: Old firmware version which may return constant "
             "values after a few hours of operation");
    }

    // Set FIRMWARE_REVISION homekit characteristic
    char* firmware_vs = "";
    char major_string[32];
    char minor_string[32];
    sprintf(major_string, "%d", version_information.firmware_major);
    sprintf(minor_string, "%d", version_information.firmware_minor);
    strcpy(firmware_vs, major_string);
    strcat(firmware_vs, ".");
    strcat(firmware_vs, minor_string);
    set_firmware(firmware_vs);
    
  }

  char serial_id[SVM40_MAX_SERIAL_LEN];
  err = svm40_get_serial(serial_id);
  if (err) {
    Serial.print("Error reading SVM40 serial: ");
    Serial.println(err);
  } else {
    Serial.print("Serial Number: ");
    Serial.println(serial_id);

    // Set SERIAL_NUMBER homekit characteristic
    char* serial_nb = "";
    strcpy(serial_nb, serial_id);
    set_serial(serial_nb);
    
  }
  do {
    err = svm40_start_continuous_measurement();
    if (err) {
      Serial.print("Error starting measurement: ");
      Serial.println(err);
      delay(100);
    }
  } while(err);
  
}

void loop() {
  my_homekit_loop();
  delay(10);
}

//==============================
// Homekit setup and loop
//==============================

// access your homekit characteristics defined in my_accessory.c
extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t cha_airquality;
extern "C" homekit_characteristic_t cha_current_temperature;
extern "C" homekit_characteristic_t cha_humidity;
extern "C" homekit_characteristic_t cha_firmware_revision;
extern "C" homekit_characteristic_t cha_serial_number;

static uint32_t next_heap_millis = 0;
static uint32_t next_report_millis = 0;

void my_homekit_setup() {
  arduino_homekit_setup(&config);
}

void set_firmware(char* firmware_revision_value) {
  cha_firmware_revision.value.string_value = firmware_revision_value;
  LOG_D("Current firmware: %s", firmware_revision_value);
  homekit_characteristic_notify(&cha_firmware_revision, cha_firmware_revision.value); 
} 

void set_serial(char* serial_number_value) {
  cha_serial_number.value.string_value = serial_number_value;
  LOG_D("Serial Number: %s", serial_number_value);
  homekit_characteristic_notify(&cha_serial_number, cha_serial_number.value); 
} 

void my_homekit_loop() {
  arduino_homekit_loop();
  const uint32_t t = millis();
  if (t > next_report_millis) {
    // report sensor values every 10 seconds
    next_report_millis = t + 10 * 1000;
    my_homekit_report();
  }
  if (t > next_heap_millis) {
    // show heap info every 5 seconds
    next_heap_millis = t + 5 * 1000;
    LOG_D("Free heap: %d, HomeKit clients: %d",
        ESP.getFreeHeap(), arduino_homekit_connected_clients_count());

  }
}

void my_homekit_report() {

  // Get values from Sensor
  int16_t err;
  int16_t voc_index;
  int16_t relative_humidity;
  int16_t temperature_celsius;
  err = svm40_read_measured_values_as_integers(
          &voc_index, &relative_humidity, &temperature_celsius);

  // Convert the Sensirion VOC index [0, 5000] to Homekit air quality index [1, 2, 3, 4, 5]
  uint8_t airquality_value = 0; // 0 == undefined
  airquality_value = round(voc_index/1250.0f+1);
  cha_airquality.value.uint8_value = airquality_value;
  Serial.print("Sensirion VOC index (raw): ");
  Serial.println(voc_index);
  Serial.print("Homekit air quality index: ");
  Serial.println(airquality_value);
  homekit_characteristic_notify(&cha_airquality, cha_airquality.value);
  
  float temperature_value = temperature_celsius / 200.0f;
  cha_current_temperature.value.float_value = temperature_value;
  LOG_D("Current temperature: %.1f", temperature_value);
  homekit_characteristic_notify(&cha_current_temperature, cha_current_temperature.value);

  float humidity_value = relative_humidity/100.0f;
  cha_humidity.value.float_value = humidity_value;
  LOG_D("Current humidity: %.1f", humidity_value);
  homekit_characteristic_notify(&cha_humidity, cha_humidity.value);

}
