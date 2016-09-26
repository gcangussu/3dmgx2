/*
 * i3dmgx2_utils.c
 *
 *  Created on: 25 de set de 2016
 *      Author: gabriel
 */

#include "i3dmgx2_utils.h"
#include "i3dmgx2_errors.h"

#include <stdint.h>
#include <arpa/inet.h>


uint16_t i3dmgx2_calc_chksum(uint8_t *resp, unsigned resp_len) {
    unsigned sum = 0;
    unsigned chksum_pos = resp_len - 2;
    for (unsigned i = 0; i < chksum_pos; ++i) {
        sum += resp[i];
    }
    return sum & 0xFFFF;
}


int i3dmgx2_check_consistency(unsigned cmd_byte, unsigned expect_len,
        uint8_t *resp, unsigned resp_len) {
    /* Check length of response */
    if (resp_len != expect_len) {
        return I3DMGX2_WRONG_RESP_LEN;
    }

    /* Check header of response */
    if (resp[0] != cmd_byte) {
        return I3DMGX2_WRONG_RESP_HEADER;
    }

    /* Check checksum of response */
    unsigned chksum_pos = resp_len - 2;
    unsigned chksum = BYTEARRAY_TO_HS(resp + chksum_pos);

    unsigned sum = 0;
    for (unsigned i = 0; i < chksum_pos; ++i) {
        sum += resp[i];
    }

    if ((sum & 0xFFFF) != chksum) {
        return I3DMGX2_CHKSUM_ERROR;
    }

    return 0;
}


void i3dmgx2_parse_floats(float *values, uint8_t *buffer, unsigned num) {
    uint32_t *buff_32bits = (uint32_t*) buffer;
    uint32_t aux;
    for (unsigned i = 0; i < num; ++i) {
        aux = ntohl(buff_32bits[i]);
        values[i] = *((float*)(&aux));  /* Convert 4 bytes to float */
    }
}
