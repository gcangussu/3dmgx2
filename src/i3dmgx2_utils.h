/*
 * i3dmgx2_utils.h
 *
 *  Created on: 25 de set de 2016
 *      Author: gabriel
 */

#ifndef I3DMGX2_UTILS_H_
#define I3DMGX2_UTILS_H_

#include <stdint.h>
#include <arpa/inet.h>

#include "i3dmgx2_errors.h"


/* Get a host uint32_t from network received bytes */
#define BYTEARRAY_TO_HL(ptr_bytes) ntohl(*((uint32_t*)(ptr_bytes)))

/* Get a host uint16_t from network received bytes */
#define BYTEARRAY_TO_HS(ptr_bytes) ntohs(*((uint16_t*)(ptr_bytes)))

#define DEREFER_32BITS(ptr) (*((uint32_t*)(ptr)))

#define DEREFER_16BITS(ptr) (*((uint16_t*)(ptr)))


uint16_t i3dmgx2_calc_chksum(uint8_t *resp, unsigned resp_len);


/*
 * Checks the length, header and checksum of response.
 * Returns 0 if no inconsistency detected, different than zero otherwise.
 */
int i3dmgx2_check_consistency(unsigned cmd_byte, unsigned expect_len,
        uint8_t *resp, unsigned resp_len);


/*
 * Parse num floats from buffer and save it on values
 */
void i3dmgx2_parse_floats(float *values, uint8_t *buffer, unsigned num);


#endif /* I3DMGX2_UTILS_H_ */
