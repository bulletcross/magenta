// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <ddk/device.h>
#include <ddk/driver.h>
#include <ddk/common/hid.h>  // TODO: move this
#include <ddk/protocol/input.h>
#include <magenta/types.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <threads.h>

typedef struct hid_report_size {
    int16_t id;
    input_report_size_t in_size;
    input_report_size_t out_size;
    input_report_size_t feat_size;
} hid_report_size_t;

typedef struct mx_hid_device {
    mx_device_t dev;
    mx_device_t* busdev;
    mx_driver_t* drv;

    // May be used by a mx_hid_protocol_t to store bus-specific data.
    void* context;

    // To be filled in by the driver
    uint8_t dev_num;
    bool boot_device;
    uint8_t dev_class;

    uint32_t flags;

    size_t hid_report_desc_len;
    uint8_t* hid_report_desc;

#define HID_MAX_REPORT_IDS 16
    size_t num_reports;
    hid_report_size_t sizes[HID_MAX_REPORT_IDS];

    struct list_node instance_list;
    mtx_t instance_lock;

} mx_hid_device_t;

enum {
    HID_DESC_TYPE_REPORT = 0x22,
};

enum {
    HID_REPORT_TYPE_INPUT = 1,
    HID_REPORT_TYPE_OUTPUT = 2,
    HID_REPORT_TYPE_FEATURE = 3,
};

enum {
    HID_PROTOCOL_BOOT = 0,
    HID_PROTOCOL_REPORT = 1,
};

enum {
    HID_DEV_CLASS_OTHER = 0,
    HID_DEV_CLASS_KBD = 1,
    HID_DEV_CLASS_POINTER = 2,
    HID_DEV_CLASS_KBD_POINTER = 3,
};

typedef void (*hid_interrupt_cb)(mx_hid_device_t* dev, mx_status_t cb_status,
        const void* data, size_t len);

typedef struct mx_hid_protocol {
    mx_status_t (*get_descriptor)(mx_device_t* dev, uint8_t desc_type,
            void** data, size_t* len);
    mx_status_t (*get_report)(mx_device_t* dev, uint8_t rpt_type, uint8_t rpt_id,
            void* data, size_t len);
    mx_status_t (*set_report)(mx_device_t* dev, uint8_t rpt_type, uint8_t rpt_id,
            void* data, size_t len);
    mx_status_t (*get_idle)(mx_device_t* dev, uint8_t rpt_id, uint8_t* duration);
    mx_status_t (*set_idle)(mx_device_t* dev, uint8_t rpt_id, uint8_t duration);
    mx_status_t (*get_protocol)(mx_device_t* dev, uint8_t* protocol);
    mx_status_t (*set_protocol)(mx_device_t* dev, uint8_t protocol);
} mx_hid_protocol_t;

mx_status_t hid_create_device(mx_hid_device_t** dev, mx_device_t* busdev, uint8_t dev_num,
        bool boot_device, uint8_t dev_class);
mx_status_t hid_add_device(mx_driver_t* drv, mx_hid_device_t* dev);
// TODO: hid_free_device
