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

#include "svm40.h"
#include "svm40_git_version.h"

const char* svm40_get_driver_version(void) {
    return SVM40_DRV_VERSION_STR;
}

int16_t svm40_get_serial(char* serial) {
    int16_t error;

    error =
        sensirion_i2c_write_cmd(SVM40_I2C_ADDRESS, SVM40_CMD_GET_SERIAL_NUMBER);
    if (error != NO_ERROR) {
        return error;
    }
    sensirion_sleep_usec(SVM40_CMD_DELAY);
    error = sensirion_i2c_read_words_as_bytes(
        SVM40_I2C_ADDRESS, (uint8_t*)serial,
        SVM40_MAX_SERIAL_LEN / SENSIRION_WORD_SIZE);
    if (error != NO_ERROR) {
        return error;
    }
    serial[SVM40_MAX_SERIAL_LEN - 1] = '\0';
    return NO_ERROR;
}

int16_t svm40_probe(void) {
    struct svm40_version_information version_information;
    return svm40_get_version(&version_information);
}

int16_t svm40_start_continuous_measurement(void) {
    int16_t error;
    error = sensirion_i2c_write_cmd(SVM40_I2C_ADDRESS,
                                    SVM40_CMD_START_CONTINUOUS_MEASUREMENT);
    sensirion_sleep_usec(SVM40_CMD_DELAY);
    return error;
}

int16_t svm40_stop_measurement(void) {
    int16_t error;
    error =
        sensirion_i2c_write_cmd(SVM40_I2C_ADDRESS, SVM40_CMD_STOP_MEASUREMENT);
    sensirion_sleep_usec(SVM40_STOP_MEASUREMENT_DELAY);
    return error;
}

int16_t svm40_read_measured_values_as_integers(int16_t* voc_index,
                                               int16_t* relative_humidity,
                                               int16_t* temperature) {
    int16_t error;
    int16_t buffer[3];
    error = sensirion_i2c_write_cmd(SVM40_I2C_ADDRESS,
                                    SVM40_CMD_READ_MEASURED_VALUES_AS_INTEGERS);
    if (error) {
        return error;
    }
    sensirion_sleep_usec(SVM40_CMD_DELAY);

    error = sensirion_i2c_read_words(SVM40_I2C_ADDRESS, (uint16_t*)buffer,
                                     SENSIRION_NUM_WORDS(buffer));
    if (error) {
        return error;
    }

    *voc_index = buffer[0];
    *relative_humidity = buffer[1];
    *temperature = buffer[2];

    return NO_ERROR;
}

int16_t svm40_read_measured_values_as_integers_with_raw_params(
    int16_t* voc_index, int16_t* relative_humidity, int16_t* temperature,
    uint16_t* voc_ticks_raw, int16_t* uncompensated_relative_humidity,
    int16_t* uncompensated_temperature) {
    int16_t error;
    uint16_t buffer[6];

    error = sensirion_i2c_write_cmd(
        SVM40_I2C_ADDRESS,
        SVM40_CMD_READ_MEASURED_VALUES_AS_INTEGERS_WITH_RAW_PARAMETERS);
    if (error) {
        return error;
    }
    sensirion_sleep_usec(SVM40_CMD_DELAY);

    error = sensirion_i2c_read_words(SVM40_I2C_ADDRESS, buffer,
                                     SENSIRION_NUM_WORDS(buffer));
    if (error) {
        return error;
    }

    *voc_index = (int16_t)buffer[0];
    *relative_humidity = (int16_t)buffer[1];
    *temperature = (int16_t)buffer[2];
    *voc_ticks_raw = buffer[3];
    *uncompensated_relative_humidity = (int16_t)buffer[4];
    *uncompensated_temperature = (int16_t)buffer[5];

    return NO_ERROR;
}

int16_t
svm40_get_version(struct svm40_version_information* version_information) {
    int16_t error;
    uint8_t buffer[8];
    error = sensirion_i2c_write_cmd(SVM40_I2C_ADDRESS, SVM40_CMD_GET_VERSION);
    if (error) {
        return error;
    }

    sensirion_sleep_usec(SVM40_CMD_DELAY);

    error = sensirion_i2c_read_words_as_bytes(SVM40_I2C_ADDRESS, buffer,
                                              SENSIRION_NUM_WORDS(buffer));

    if (error) {
        return error;
    }

    version_information->firmware_major = buffer[0];
    version_information->firmware_minor = buffer[1];
    version_information->firmware_debug = buffer[2];
    version_information->hardware_major = buffer[3];
    version_information->hardware_minor = buffer[4];
    version_information->protocol_major = buffer[5];
    version_information->protocol_minor = buffer[6];
    // the last byte is a padding byte

    return NO_ERROR;
}

int16_t svm40_device_reset(void) {
    int16_t error;
    error = sensirion_i2c_write_cmd(SVM40_I2C_ADDRESS, SVM40_CMD_DEVICE_RESET);
    sensirion_sleep_usec(SVM40_DEVICE_RESET_DELAY);
    return error;
}
