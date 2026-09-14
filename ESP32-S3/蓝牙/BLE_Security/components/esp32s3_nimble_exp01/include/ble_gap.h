#ifndef __BLE_GAP_H__
#define __BLE_GAP_H__

#include "common.h"


#define BLE_GAP_APPEARANCE_GENERIC_TAG 0x0200
#define BLE_GAP_URI_PREFIX_HTTPS 0x17
#define BLE_GAP_LE_ROLE_PERIPHERAL 0x00

int gap_init(void);
void adv_init(void);
bool is_connection_encrypted(uint16_t conn_handle);
#endif
