/*
 ============================================================================
 Name        : i3dmgx2_protocol_test.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description :
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include <arpa/inet.h>

#include "i3dmgx2_protocol.h"
#include "i3dmgx2_errors.h"
#include "i3dmgx2_utils.h"


void test_wireless_ping() {
    uint8_t ping_cmd[] = CMD_WIRELESS_PING(0x00FF);
    assert(sizeof(ping_cmd) == 3);
    assert(ping_cmd[0] == 0x02);
    assert(ping_cmd[1] == 0x00);
    assert(ping_cmd[2] == 0xFF);

    uint8_t ping_cmd2[] = CMD_WIRELESS_PING(0xFF00);
    assert(ping_cmd2[1] == 0xFF);
    assert(ping_cmd2[2] == 0x00);

    int status;
    uint8_t resp1[] = {0x02};
    assert(parse_wireless_ping(resp1, sizeof(resp1), &status) == 0);
    assert(status);

    uint8_t resp2[] = {0x21};
    assert(parse_wireless_ping(resp2, sizeof(resp2), &status) == 0);
    assert(!status);

    uint8_t resp3[] = {0x13};
    int s = parse_wireless_ping(resp3, sizeof(resp3), &status);
    assert(s == I3DMGX2_UNEXP_RESP);

    uint8_t resp4[] = {0xFF, 0xAA};
    s = parse_wireless_ping(resp4, sizeof(resp4), &status);
    assert(s == I3DMGX2_WRONG_RESP_LEN);
}


void test_raw_acc_angr() {
    uint8_t resp[31];
    resp[0] = 0xC1;

    float myfloats[6] = {20.0, 20.5, 21.0, 21.5, 22.0, 22.5};
    uint32_t network_floats[6];
    for (int i = 0; i < 6; ++i) {
        network_floats[i] = htonl(*((uint32_t*)(myfloats + i)));
    }
    memcpy(resp + 1, network_floats, sizeof(network_floats));

    *((uint32_t*)(resp + 25)) = htonl(0xFFFFFF00);

    unsigned sum = 0;
    for (int i = 0; i < 29; ++i) {
        sum += resp[i];
    }
    *((uint16_t*)(resp + 29)) = htons(sum & 0xFFFF);

    struct raw_acc_angr record;
    int result = parse_raw_acc_angr(resp, sizeof(resp), &record);
    assert(result == 0);

    for (int i = 0; i < 3; ++i) {
        assert(record.acc[i] == myfloats[i]);
        assert(record.angr[i] == myfloats[i+3]);
    }

    assert(record.timer == 0xFFFFFF00);
}


void test_raw_acc() {
    uint8_t resp[31];
    resp[0] = 0xC2;

    float myfloats[6] = {20.0, 20.5, 21.0, 21.5, 22.0, 22.5};
    uint32_t network_floats[6];
    for (int i = 0; i < 6; ++i) {
        network_floats[i] = htonl(*((uint32_t*)(myfloats + i)));
    }
    memcpy(resp + 1, network_floats, sizeof(network_floats));

    *((uint32_t*)(resp + 25)) = htonl(0xFFFFFF00);

    unsigned sum = 0;
    for (int i = 0; i < 29; ++i) {
        sum += resp[i];
    }
    *((uint16_t*)(resp + 29)) = htons(sum & 0xFFFF);

    struct acc_angr record;
    int result = parse_acc_angr(resp, sizeof(resp), &record);
    assert(result == 0);

    for (int i = 0; i < 3; ++i) {
        assert(record.acc[i] == myfloats[i]);
        assert(record.angr[i] == myfloats[i+3]);
    }

    assert(record.timer == 0xFFFFFF00);
}


void test_delta_ang_vel() {
    uint8_t resp[31];
    resp[0] = 0xC3;

    float myfloats[6] = {20.0, 20.5, 21.0, 21.5, 22.0, 22.5};
    uint32_t network_floats[6];
    for (int i = 0; i < 6; ++i) {
        network_floats[i] = htonl(*((uint32_t*)(myfloats + i)));
    }
    memcpy(resp + 1, network_floats, sizeof(network_floats));

    *((uint32_t*)(resp + 25)) = htonl(0xFFFFFF00);

    unsigned sum = 0;
    for (int i = 0; i < 29; ++i) {
        sum += resp[i];
    }
    *((uint16_t*)(resp + 29)) = htons(sum & 0xFFFF);

    struct delta_ang_vel record;
    int result = parse_delta_ang_vel(resp, sizeof(resp), &record);
    assert(result == 0);

    for (int i = 0; i < 3; ++i) {
        assert(record.ang[i] == myfloats[i]);
        assert(record.vel[i] == myfloats[i+3]);
    }

    assert(record.timer == 0xFFFFFF00);
}


void test_continuous_mode() {
    uint8_t cmd[] = CMD_SET_CONTINUOUS(0xFB);
    assert(cmd[0] == 0xC4);
    assert(cmd[1] == 0xC1);
    assert(cmd[2] == 0x29);
    assert(cmd[3] == 0xFB);

    uint8_t resp[8] = {0xC4, 0xFB};
    uint32_t timer = 0xFFFFFF00;
    DEREFER_32BITS(resp + 2) = htonl(timer);
    DEREFER_16BITS(resp + 6) = htons(i3dmgx2_calc_chksum(resp, sizeof(resp)));

    struct set_continuous_resp record;
    int result = parse_set_continuous(resp, sizeof(resp), &record);
    assert(result == 0);

    assert(record.cmd_byte == 0xFB);
    assert(record.timer == timer);
}


void test_orient_matrix() {
    uint8_t resp[43];
    resp[0] = 0xC5;

    float test_values[9] = {0.9f, 1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f,
                            1.7f};

    uint32_t *test_values_u32 = (uint32_t*) test_values;
    uint32_t *resp_u32 = (uint32_t*) (resp + 1);
    for (int i = 0; i < 9; ++i) {
        resp_u32[i] = htonl(test_values_u32[i]);
    }

    uint32_t timer = 0xABCD0123;
    DEREFER_32BITS(resp + 37) = htonl(timer);
    DEREFER_16BITS(resp + 41) = htons(i3dmgx2_calc_chksum(resp, sizeof(resp)));

    struct orient_matrix record;
    int result = parse_orient_matrix(resp, sizeof(resp), &record);
    assert(result == 0);

    for (int i = 0; i < 9; ++i) {
        assert(((float*) record.matrix)[i] == test_values[i]);
    }
    assert(record.timer == timer);
}


void test_update_matrix() {
    /* TODO */
}


void test_write_acc_bias() {
//    uint8_t cmd[] = CMD_WRITE_ACC_BIAS()
}


int main(void) {
    test_wireless_ping();
    test_raw_acc_angr();
    test_raw_acc();
    test_delta_ang_vel();
    test_continuous_mode();
    test_orient_matrix();
    test_update_matrix();
    test_write_acc_bias();

    return EXIT_SUCCESS;
}





















