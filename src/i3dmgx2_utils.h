/*
 * i3dmgx2_utils.h
 *
 *  Created on: 25 de set de 2016
 *      Author: gabriel
 */

#ifndef I3DMGX2_UTILS_H_
#define I3DMGX2_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <arpa/inet.h>

#define I3DMGX2_CMD_OVERHEAD 9

#define I3DMGX2_RESP_OVERHEAD 11

#define I3DMGX2_PACK_OFFSET 7

/* Get a host uint32_t from network received bytes */
#define BYTEARRAY_TO_HL(ptr_bytes) ntohl(*((uint32_t*)(ptr_bytes)))

/* Get a host uint16_t from network received bytes */
#define BYTEARRAY_TO_HS(ptr_bytes) ntohs(*((uint16_t*)(ptr_bytes)))

#define DEREFER_32BITS(ptr) (*((uint32_t*)(ptr)))

#define DEREFER_16BITS(ptr) (*((uint16_t*)(ptr)))

/*
 * Calculates the checksum for the wired standard response
 */
uint16_t i3dmgx2_resp_chksum(uint8_t *resp, unsigned resp_len);


/*
 * Calculates the checksum for the packetized request
 */
uint16_t i3dmgx2_cmd_pack_chksum(uint8_t *cmd_pack, unsigned len);


/*
 * Calculates the checksum for the packetized response
 */
uint16_t i3dmgx2_resp_pack_chksum(uint8_t *resp_pack, unsigned len);


/*
 * Checks the length and header of response.
 * Returns 0 if no inconsistency detected, different than zero otherwise.
 */
int i3dmgx2_check_consistency(unsigned cmd_byte, unsigned expect_len,
        uint8_t *resp, unsigned resp_len);


/*
 * Parse num floats from buffer and save it on values
 */
void i3dmgx2_parse_floats(float *values, uint8_t *buffer, unsigned num);


/*
 * Copy an array of floats to a bytearray for each float in network order.
 */
void i3dmgx2_floats_to_net_bytes(uint8_t *buffer, float *values,
        unsigned num);

#ifdef __cplusplus
}
#endif

#endif /* I3DMGX2_UTILS_H_ */
