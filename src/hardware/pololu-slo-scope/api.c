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

#include <config.h>
#include "protocol.h"
#include <stddef.h>

#define SLOSCOPE_VENDOR 0x1FFB
#define SLOSCOPE_PRODUCT 0x0081

static const uint32_t drvopts[] = {
	SR_CONF_OSCILLOSCOPE,
};

static const uint32_t devopts[] = {
	SR_CONF_SAMPLERATE | SR_CONF_GET | SR_CONF_SET | SR_CONF_LIST,
	SR_CONF_CONTINUOUS | SR_CONF_GET,
};

static const uint64_t samplerates[] = { SR_KHZ(10) };

static GSList *scan(struct sr_dev_driver *di, GSList *options)
{
	struct drv_context *drvc;
	GSList *devices;
	libusb_device **device_list;

	(void)options;

	devices = NULL;
	drvc = di->context;
	drvc->instances = NULL;

	// We assume there'll only be one SLO-scope
	libusb_device_handle *device_handle = libusb_open_device_with_vid_pid(drvc->sr_ctx->libusb_ctx, SLOSCOPE_VENDOR, SLOSCOPE_PRODUCT);
	if (device_handle == NULL) {
		return devices;
	}
	libusb_device *device = libusb_get_device(device_handle);
	libusb_close(device_handle);

	char connection_id[64];
	usb_get_port_path(device, connection_id, sizeof(connection_id));

	struct sr_dev_inst *sdi = g_malloc0(sizeof(struct sr_dev_inst));
	sdi->vendor = g_strdup("Pololu");
	sdi->model = g_strdup("SLO-scope");
	sdi->connection_id = g_strdup(connection_id);

	sr_channel_new(sdi, 0, SR_CHANNEL_ANALOG, TRUE, "A");
	sr_channel_new(sdi, 1, SR_CHANNEL_ANALOG, TRUE, "B");

	sr_dbg("Found an SLO-scope.");
	sdi->status = SR_ST_INACTIVE;
	sdi->inst_type = SR_INST_USB;
	sdi->conn = sr_usb_dev_inst_new(libusb_get_bus_number(device), libusb_get_device_address(device), NULL);

	// Some stuff will fail if priv == NULL, but I don't need a priv struct
	sdi->priv = 1;

	devices = g_slist_append(devices, sdi);

	return std_scan_complete(di, devices);
}

static int dev_open(struct sr_dev_inst *sdi)
{
	(void)sdi;
	
	struct sr_dev_driver *di = sdi->driver;
	struct drv_context *drvc = di->context;
	struct dev_context *devc = sdi->priv;
	struct sr_usb_dev_inst *usb = sdi->conn;
	int ret;

	if (sr_usb_open(drvc->sr_ctx->libusb_ctx, usb) != SR_OK)
		return SR_ERR;

	if ((ret = libusb_claim_interface(usb->devhdl, 4))) {
		sr_err("Failed to claim interface: %s.", libusb_error_name(ret));
		return SR_ERR;
	}

	if (pololu_slo_scope_set_state(sdi, POLOLU_SLO_SCOPE_STATE_2ANALOG))
		return SR_ERR;
	
	return SR_OK;
}

static int dev_close(struct sr_dev_inst *sdi)
{
	(void)sdi;

	/* TODO: get handle from sdi->conn and close it. */

	return SR_OK;
}

static int config_get(uint32_t key, GVariant **data,
	const struct sr_dev_inst *sdi, const struct sr_channel_group *cg)
{
	printf("Getting a config item, key is %x...\n", key);
	(void)sdi;
	(void)data;
	(void)cg;

	switch (key) {
	case SR_CONF_SAMPLERATE:
		// There's only one sample rate anyway
		*data = g_variant_new_uint64(SR_KHZ(10));
		break;
	case SR_CONF_CONTINUOUS:
		*data = TRUE;
		break;
	default:
		return SR_ERR_NA;
	}

	return SR_OK;
}

static int config_set(uint32_t key, GVariant *data,
	const struct sr_dev_inst *sdi, const struct sr_channel_group *cg)
{
	int ret;

	(void)sdi;
	(void)data;
	(void)cg;

	ret = SR_OK;
	switch (key) {
	case SR_CONF_SAMPLERATE:
		// Only one sample rate, so do nothing
		break;
	default:
		ret = SR_ERR_NA;
	}

	return ret;
}

static int config_list(uint32_t key, GVariant **data,
	const struct sr_dev_inst *sdi, const struct sr_channel_group *cg)
{
	printf("Grabbing config list, key is %x...\n", key);
	int ret;

	(void)sdi;
	(void)data;
	(void)cg;

	ret = SR_OK;
	switch (key) {
	case SR_CONF_DEVICE_OPTIONS:
		return STD_CONFIG_LIST(key, data, sdi, cg, NO_OPTS, drvopts, devopts);
	case SR_CONF_SAMPLERATE:
		*data = std_gvar_samplerates_steps(ARRAY_AND_SIZE(samplerates));
		break;
	default:
		return SR_ERR_NA;
	}

	return ret;
}

static int dev_acquisition_start(const struct sr_dev_inst *sdi)
{
	/* TODO: configure hardware, reset acquisition state, set up
	 * callbacks and send header packet. */

	(void)sdi;

	// sr_session_source_add(sdi->session, -1, 0, 100,
	// 	demo_prepare_data, (struct sr_dev_inst *)sdi);

	std_session_send_df_header(sdi);

	std_session_send_frame_begin(sdi); // maybe?

	struct sr_analog_encoding encoding;
	struct sr_analog_meaning meaning;
	struct sr_analog_spec spec;

	pololu_slo_scope_packet packet;

	while (1) {
		// pololu_slo_scope_data_2analog data;
		pololu_slo_scope_get_packet(sdi, &packet);
		// pololu_slo_scope_packet_to_data_2analog(&packet, &data);
		float readings[20];
		for (int i = 0; i < 10; i++)
			readings[i] = pololu_slo_scope_voltage_byte_to_float(packet.data[i * 2]);
		for (int i = 0; i < 10; i++)
			readings[10 + i] = pololu_slo_scope_voltage_byte_to_float(packet.data[i * 2 + 1]);

		struct sr_datafeed_analog analog;

		sr_analog_init(&analog, &encoding, &meaning, &spec, 3);
		analog.meaning->mq = SR_MQ_VOLTAGE;
		analog.meaning->unit = SR_UNIT_VOLT;
		analog.meaning->mqflags = SR_MQFLAG_DC;
		analog.meaning->channels = sdi->channels;
		analog.num_samples = 10;
		analog.data = &readings;

		const struct sr_datafeed_packet analog_packet = {
			.type = SR_DF_ANALOG,
			.payload = &analog
		};

		sr_session_send(sdi, &analog_packet);
	}

	return SR_OK;
}

static int dev_acquisition_stop(struct sr_dev_inst *sdi)
{
	/* TODO: stop acquisition. */


	printf("Stopping!\n");
	(void)sdi;

	return SR_OK;
}

static struct sr_dev_driver pololu_slo_scope_driver_info = {
	.name = "pololu-slo-scope",
	.longname = "Pololu SLO-scope",
	.api_version = 1,
	.init = std_init,
	.cleanup = std_cleanup,
	.scan = scan,
	.dev_list = std_dev_list,
	.dev_clear = std_dev_clear,
	.config_get = config_get,
	.config_set = config_set,
	.config_list = config_list,
	.dev_open = dev_open,
	.dev_close = dev_close,
	.dev_acquisition_start = dev_acquisition_start,
	.dev_acquisition_stop = dev_acquisition_stop,
	.context = NULL,
};
SR_REGISTER_DEV_DRIVER(pololu_slo_scope_driver_info);
