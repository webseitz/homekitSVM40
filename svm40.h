/*
 * Copyright (c) 2020, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SVM40_H
#define SVM40_H

#include "sensirion_arch_config.h"
#include "sensirion_common.h"
#include "sensirion_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SVM40_I2C_ADDRESS 0x6a

#define SVM40_CMD_START_CONTINUOUS_MEASUREMENT 0x0010
#define SVM40_CMD_STOP_MEASUREMENT 0x0104
#define SVM40_CMD_READ_MEASURED_VALUES_AS_INTEGERS 0x03A6
#define SVM40_CMD_READ_MEASURED_VALUES_AS_INTEGERS_WITH_RAW_PARAMETERS 0x03B0
#define SVM40_CMD_GET_VERSION 0xD100
#define SVM40_CMD_GET_SERIAL_NUMBER 0xD033
#define SVM40_CMD_DEVICE_RESET 0xD304

#define SVM40_MEASUREMENT_INTERVAL_USEC 1000000
#define SVM40_MAX_SERIAL_LEN 26
#define SVM40_CMD_DELAY 1000
#define SVM40_STOP_MEASUREMENT_DELAY 50000
#define SVM40_DEVICE_RESET_DELAY 100000

/**
 * Check if SVM40 sensor is available
 *
 * @return  NO_ERROR on success, an error code otherwise
 */
int16_t svm40_probe(void);

struct svm40_version_information {

    /** Major part of the firmware version major.minor */
    uint8_t firmware_major;
    /** Minor part of the firmware version major.minor */
    uint8_t firmware_minor;
    /** Marks a development firmware version */
    bool firmware_debug;

    /** Major part of the hardware version major.minor */
    uint8_t hardware_major;
    /** Minor part of the hardware version major.minor */
    uint8_t hardware_minor;

    /** Major part of the protocol version major.minor */
    uint8_t protocol_major;
    /** Minor part of the protocol version major.minor */
    uint8_t protocol_minor;
};

/**
 * Read the hardware and firmware version information
 *
 * @param version_information   Struct to store the version information
 * @return  NO_ERROR on success, an error code otherwise
 */
int16_t
svm40_get_version(struct svm40_version_information* version_information);

/**
 * Get Serial Number.
 *
 * @param serial Memory where the serial number is written into.
 * 				 The string is guaranteed to be zero terminated.
 * 				 It must be at least SVM40_MAX_SERIAL_LEN long.
 * @return NO_ERROR on success, an error code otherwise
 */
int16_t svm40_get_serial(char* serial);

/**
 * Start a continuous measurement.
 *
 * @note This command is only available in idle mode.
 * @return  NO_ERROR on success, an error code otherwise
 */
int16_t svm40_start_continuous_measurement(void);

/**
 * Stop a running measurement.
 * Waits 50ms until device is ready again.
 *
 * @note This command is only available in measurement mode.
 * @return  NO_ERROR on success, an error code otherwise
 */
int16_t svm40_stop_measurement(void);

/**
 * Read the current measured values.
 *
 * The firmware updates the measurement values every second. Polling data
 * faster will return the same values. The first measurement is available one
 * second after the start emasurement command is issued. Any readout prior to
 * this will return zero initialized values.
 *
 * @note Only availabe in measurement mode.
 * @param   voc_index           VOC algorithm output with a scaling value of 10.
 * @param   relative_humidity   Compensated ambient humidity in %RH with a
 *                              scaling factor of 100.
 * @param   temperature         Compensated ambient temperature in degree
 *                              celsius with a scaling factor of 200.
 * @return  NO_ERROR on success, an error code otherwise
 */
int16_t svm40_read_measured_values_as_integers(int16_t* voc_index,
                                               int16_t* relative_humidity,
                                               int16_t* temperature);

/**
 * Returns the new measurement results as integers along with the raw voc
 * ticks and uncompensated RH/T values.
 *
 * The firmware updates the measurement values every second. Polling data
 * faster will return the same values. The first measurement is available one
 * second after the start measurement command is issued. Any readout prior to
 * this will return zero initialized values.
 *
 * @note Only available in measurement mode.
 * @param   voc_index                       VOC algorithm output
 *                                          with a scaling value of 10.
 * @param   relative_humidity               Compensated ambient
 *                                          humidity in %RH with a scaling
 *                                          factor of 100.
 * @param   temperature                     Compensated ambient temperature
 *                                          in degree celsius with a scaling
 *                                          factor of 200.
 * @param   voc_ticks_raw                   Raw VOC output ticks as
 *                                          read from the SGP sensor.
 * @param   uncompensated_relative_humidity Uncompensated raw humidity in
 *                                          %RH as read from the SHT40 with a
 *                                          scaling factor of 100.
 * @param   uncompensated_temperature       Uncompensated raw temperature in
 *                                          degrees celsius as read from the
 *                                          SHT40 with a scaling of 200.
 * @return  NO_ERROR on success, an error code otherwise
 */
int16_t svm40_read_measured_values_as_integers_with_raw_params(
    int16_t* voc_index, int16_t* relative_humidity, int16_t* temperature,
    uint16_t* voc_ticks_raw, int16_t* uncompensated_relative_humidity_raw,
    int16_t* uncompensated_temperature);

/**
 * Return the driver version
 *
 * @return  Driver version string
 */
const char* svm40_get_driver_version(void);

/**
 * Executes a reset on the device.
 * Waits 100ms until device is ready again.
 *
 * @return NO_ERROR on success, an error code otherwise
 */
int16_t svm40_device_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* SVM40_H */
