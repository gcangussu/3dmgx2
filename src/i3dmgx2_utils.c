/*
 * i3dmgx2_utils.c
 *
 *  Created on: 25 de set de 2016
 *      Author: gabriel
 */

#include "i3dmgx2_utils.h"
#include "i3dmgx2_errors.h"

#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>


uint16_t i3dmgx2_resp_chksum(uint8_t *resp, unsigned len) {
    unsigned sum = 0;
    const unsigned chksum_pos = len - 2;  /* avoid checksum bytes */
    for (unsigned i = 0; i < chksum_pos; ++i) {
        sum += resp[i];
    }
    return sum & 0xFFFF;
}


uint16_t i3dmgx2_cmd_pack_chksum(uint8_t *cmd_pack, unsigned len) {
    unsigned sum = 0;
    const unsigned chksum_pos = len - 2;  /* avoid checksum bytes */
    for (unsigned i = 1; i < chksum_pos; ++i) {  /* avoid Start of Packet */
        sum += cmd_pack[i];
    }
    return sum & 0xFFFF;
}


uint16_t i3dmgx2_resp_pack_chksum(uint8_t *resp_pack, unsigned len) {
    unsigned sum = 0;
    /* avoid LQI, RSSI and checksum bytes */
    const unsigned chksum_pos = len - 4;
    for (unsigned i = 1; i < chksum_pos; ++i) {  /* avoid Start of Packet */
        sum += resp_pack[i];
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


void i3dmgx2_floats_to_net_bytes(uint8_t *buffer, float *values,
        unsigned num) {
    uint32_t *u32_buffer = (uint32_t*) buffer;
    uint32_t *u32_values = (uint32_t*) values;
    for (unsigned i = 0; i < num; ++i) {
        u32_buffer[i] = htonl(u32_values[i]);
    }
}
