#include "protocol.h"

#define POLOLU_SLO_SCOPE_VENDOR 0x1FFB
#define POLOLU_SLO_SCOPE_PRODUCT 0x0081
#define POLOLU_SLO_SCOPE_ENDPOINT 0x85
#define POLOLU_SLO_SCOPE_PACKET_SIZE 22
#define POLOLU_SLO_SCOPE_READINGS_PER_PACKET 20

#define REQUEST_TYPE 0x40
#define REQUEST_GET 0x81
#define REQUEST_SET 0x82

#define POLOLU_SLO_SCOPE_VAR_STATE 0x42
#define POLOLU_SLO_SCOPE_VAR_OUTPUT_STATE 0x43

int pololu_slo_scope_set_var(struct sr_dev_inst *sdi, uint16_t var, uint16_t value) {
    return libusb_control_transfer(
        ((struct sr_usb_dev_inst *)(sdi->conn))->devhdl,
        REQUEST_TYPE, REQUEST_SET, value, var, NULL, 0, 0
    );
}

int pololu_slo_scope_get_var(struct sr_dev_inst *sdi, uint16_t var, unsigned char *value, size_t size) {
    return libusb_control_transfer(
        ((struct sr_usb_dev_inst *)(sdi->conn))->devhdl,
        REQUEST_TYPE, REQUEST_GET, 0, var, value, size, 0
    );
}

int pololu_slo_scope_set_state(struct sr_dev_inst *sdi, pololu_slo_scope_state state) {
    return pololu_slo_scope_set_var(sdi, POLOLU_SLO_SCOPE_VAR_STATE, state);
}

int pololu_slo_scope_set_output_state(struct sr_dev_inst *sdi, pololu_slo_scope_output_state output_state) {
    return pololu_slo_scope_set_var(sdi, POLOLU_SLO_SCOPE_VAR_OUTPUT_STATE, output_state);
}

int pololu_slo_scope_get_packet(struct sr_dev_inst *sdi, pololu_slo_scope_packet *packet) {
    int actual_len;
    int result = libusb_interrupt_transfer(((struct sr_usb_dev_inst *)(sdi->conn))->devhdl, POLOLU_SLO_SCOPE_ENDPOINT, packet, POLOLU_SLO_SCOPE_PACKET_SIZE, &actual_len, 0);

    if (result < 0) {
        return result;
    }
    if (actual_len < POLOLU_SLO_SCOPE_PACKET_SIZE) {
        return -1;
    }

    return result;
}

float pololu_slo_scope_voltage_byte_to_float(uint8_t voltage) {
    return (((float)voltage) / 255.0) * 5;
}

int pololu_slo_scope_packet_to_data_2analog(pololu_slo_scope_packet *packet, pololu_slo_scope_data_2analog *data) {
    for (int i = 0; i < POLOLU_SLO_SCOPE_READINGS_PER_PACKET; i += 2) {
        data->channel_a_readings[i] = pololu_slo_scope_voltage_byte_to_float(packet->data[i]);
    }
    for (int i = 1; i < POLOLU_SLO_SCOPE_READINGS_PER_PACKET; i += 2) {
        data->channel_b_readings[i] = pololu_slo_scope_voltage_byte_to_float(packet->data[i]);
    }
}
