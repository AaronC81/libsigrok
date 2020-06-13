/*
 * Copyright (C) 2020 Aaron Christiansen <hello@aaronc.cc>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBSIGROK_HARDWARE_POLOLU_SLO_SCOPE_PROTOCOL_H
#define LIBSIGROK_HARDWARE_POLOLU_SLO_SCOPE_PROTOCOL_H

#include <stdint.h>
#include <glib.h>
#include <libsigrok/libsigrok.h>
#include "libsigrok-internal.h"
#include <stdbool.h>

#define LOG_PREFIX "pololu-slo-scope"

enum pololu_slo_scope_state {
    POLOLU_SLO_SCOPE_STATE_OFF = 0,
    POLOLU_SLO_SCOPE_STATE_2ANALOG = 1,
    POLOLU_SLO_SCOPE_STATE_1ANALOG_1DIGITAL = 2,
};
typedef enum pololu_slo_scope_state pololu_slo_scope_state;

enum pololu_slo_scope_output_state {
    POLOLU_SLO_SCOPE_OUTPUT_STATE_OFF = 0,
    POLOLU_SLO_SCOPE_OUTPUT_STATE_LOW = 1,
    POLOLU_SLO_SCOPE_OUTPUT_STATE_HIGH = 3,
    POLOLU_SLO_SCOPE_OUTPUT_STATE_NO_CHANGE = 0xFF,
};
typedef enum pololu_slo_scope_output_state pololu_slo_scope_output_state;

struct pololu_slo_scope_packet {
    uint8_t missed_readings;
    uint8_t frame;
    unsigned char data[20];
};
typedef struct pololu_slo_scope_packet pololu_slo_scope_packet;

struct pololu_slo_scope_data_2analog {
    float channel_a_readings[10];
    float channel_b_readings[10];
};
typedef struct pololu_slo_scope_data_2analog pololu_slo_scope_data_2analog;

struct pololu_slo_scope_data_1analog_1digital {
    float channel_a_readings[20];
    bool channel_b_readings[20];
};
typedef struct pololu_slo_scope_data_1analog_1digital pololu_slo_scope_data_1analog_1digital;

int pololu_slo_scope_set_var(struct sr_dev_inst *sdi, uint16_t var, uint16_t value);
int pololu_slo_scope_get_var(struct sr_dev_inst *sdi, uint16_t var, unsigned char *value, size_t size);
int pololu_slo_scope_set_state(struct sr_dev_inst *sdi, pololu_slo_scope_state state);
int pololu_slo_scope_set_output_state(struct sr_dev_inst *sdi, pololu_slo_scope_output_state output_state);
int pololu_slo_scope_get_packet(struct sr_dev_inst *sdi, pololu_slo_scope_packet *packet);
float pololu_slo_scope_voltage_byte_to_float(uint8_t voltage);
int pololu_slo_scope_packet_to_data_2analog(pololu_slo_scope_packet *packet, pololu_slo_scope_data_2analog *data);

#endif
