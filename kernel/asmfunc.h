#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void IoOut32(uint16_t addr, uint32_t data);
uint32_t IoIn32(uint16_t addr);

#ifdef __cplusplus
}
#endif /* __cplusplus */
