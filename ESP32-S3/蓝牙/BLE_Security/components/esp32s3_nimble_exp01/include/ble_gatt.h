#ifndef __BLE_GATT_H__
#define __BLE_GATT_H__


#include "host/ble_gatt.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"

int gatt_svc_init(void);
void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
void send_heart_rate_indication(void);
int gatt_svr_subscribe_cb(struct ble_gap_event *event);

#endif
