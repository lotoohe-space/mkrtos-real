#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "lwip/err.h"
#include "lwip/netif.h"
#include <u_types.h>
err_t ethernetif_init(struct netif *netif);
err_t ethernetif_input_raw(struct netif *netif, uint8_t *data, int len);

#endif
