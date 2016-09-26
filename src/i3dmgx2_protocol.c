/*
 * i3dmgx2_protocol.c
 *
 *  Created on: 25 de set de 2016
 *      Author: gabriel
 */

#include "i3dmgx2_protocol.h"
#include "i3dmgx2_errors.h"
#include "i3dmgx2_utils.h"

#include <stdint.h>
#include <arpa/inet.h>


int parse_wireless_ping(uint8_t *resp, unsigned resp_len, int *status) {
    if (resp_len != 1) {
        return I3DMGX2_WRONG_RESP_LEN;
    }

    switch (resp[0]) {
        case 0x02:
            *status = 1;
            return 0;

        case 0x21:
            *status = 0;
            return 0;

        default:
            return I3DMGX2_UNEXP_RESP;
    }
}


int parse_raw_acc_angr(uint8_t *resp, unsigned resp_len,
        struct raw_acc_angr *record) {
    int result = i3dmgx2_check_consistency(0xC1, 31, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->acc, resp + 1, 3);
    i3dmgx2_parse_floats(record->angr, resp + 13, 3);
    record->timer = BYTEARRAY_TO_HL(resp + 25);

    return 0;
}


int parse_acc_angr(uint8_t *resp, unsigned resp_len,
        struct acc_angr *record) {
    int result = i3dmgx2_check_consistency(0xC2, 31, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->acc, resp + 1, 3);
    i3dmgx2_parse_floats(record->angr, resp + 13, 3);
    record->timer = BYTEARRAY_TO_HL(resp + 25);

    return 0;
}


int parse_delta_ang_vel(uint8_t *resp, unsigned resp_len,
        struct delta_ang_vel *record) {
    int result = i3dmgx2_check_consistency(0xC3, 31, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->ang, resp + 1, 3);
    i3dmgx2_parse_floats(record->vel, resp + 13, 3);
    record->timer = BYTEARRAY_TO_HL(resp + 25);

    return 0;
}


int parse_set_continuous(uint8_t *resp, unsigned resp_len,
        struct set_continuous_resp *record) {
    int result = i3dmgx2_check_consistency(0xC4, 8, resp, resp_len);
    if (result != 0) {
        return result;
    }

    record->cmd_byte = resp[1];
    record->timer = BYTEARRAY_TO_HL(resp + 2);

    return 0;
}


int parse_orient_matrix(uint8_t *resp, unsigned resp_len,
        struct orient_matrix *record) {
    int result = i3dmgx2_check_consistency(0xC5, 43, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats((float*) record->matrix, resp + 1, 9);
    record->timer = BYTEARRAY_TO_HL(resp + 37);

    return 0;
}


int parse_orient_up_matrix(uint8_t *resp, unsigned resp_len,
        struct orient_up_matrix *record) {
    int result = i3dmgx2_check_consistency(0xC6, 43, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats((float*) record->matrix, resp + 1, 9);
    record->timer = BYTEARRAY_TO_HL(resp + 37);

    return 0;
}


int parse_magv_resp(uint8_t *resp, unsigned resp_len,
        struct magv_resp *record) {
    int result = i3dmgx2_check_consistency(0xC7, 19, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->mag, resp + 1, 3);
    record->timer = BYTEARRAY_TO_HL(resp + 13);

    return 0;
}


int parse_acc_angr_orient(uint8_t *resp, unsigned resp_len,
        struct acc_angr_orient *record) {
    int result = i3dmgx2_check_consistency(0xC8, 67, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->acc, resp + 1, 3);
    i3dmgx2_parse_floats(record->angr, resp + 13, 3);
    i3dmgx2_parse_floats((float*) record->matrix, resp + 25, 9);
    record->timer = BYTEARRAY_TO_HL(resp + 61);

    return 0;
}


int parse_acc_bias_resp(uint8_t *resp, unsigned resp_len,
        struct acc_bias_resp *record) {
    int result = i3dmgx2_check_consistency(0xC9, 19, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->accbias, resp + 1, 3);
    record->timer = BYTEARRAY_TO_HL(resp + 13);

    return 0;
}


int parse_gyro_bias_resp(uint8_t *resp, unsigned resp_len,
        struct gyro_bias_resp *record) {
    int result = i3dmgx2_check_consistency(0xCA, 19, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->gyrobias, resp + 1, 3);
    record->timer = BYTEARRAY_TO_HL(resp + 13);

    return 0;
}


int parse_acc_angr_magv(uint8_t *resp, unsigned resp_len,
        struct acc_angr_magv *record) {
    int result = i3dmgx2_check_consistency(0xCB, 43, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->acc, resp + 1, 3);
    i3dmgx2_parse_floats(record->angr, resp + 13, 3);
    i3dmgx2_parse_floats(record->mag, resp + 25, 3);
    record->timer = BYTEARRAY_TO_HL(resp + 37);

    return 0;
}


int parse_acc_angr_magv_orient(uint8_t *resp, unsigned resp_len,
        struct acc_angr_magv_orient *record) {
    int result = i3dmgx2_check_consistency(0xCC, 79, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->acc, resp + 1, 3);
    i3dmgx2_parse_floats(record->angr, resp + 13, 3);
    i3dmgx2_parse_floats(record->mag, resp + 25, 3);
    i3dmgx2_parse_floats(record->matrix, resp + 37, 3);
    record->timer = BYTEARRAY_TO_HL(resp + 73);

    return 0;
}


int parse_capture_gyro_bias(uint8_t *resp, unsigned resp_len,
        struct capture_gyro_bias *record) {
    int result = i3dmgx2_check_consistency(0xCD, 19, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->gyrobias, resp + 1, 3);
    record->timer = BYTEARRAY_TO_HL(resp + 13);

    return 0;
}


int parse_euler_angles_resp(uint8_t *resp, unsigned resp_len,
        struct euler_angles_resp *record) {
    int result = i3dmgx2_check_consistency(0xCE, 19, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->roll, resp + 1, 3);
    record->timer = BYTEARRAY_TO_HL(resp + 13);

    return 0;
}


int parse_euler_angles_angr(uint8_t *resp, unsigned resp_len,
        struct euler_angles_angr *record) {
    int result = i3dmgx2_check_consistency(0xCF, 31, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->roll, resp + 1, 6);
    record->timer = BYTEARRAY_TO_HL(resp + 25);

    return 0;
}


int parse_transfer_nonv_mem(uint8_t *resp, unsigned resp_len,
        struct transfer_nonv_mem *record) {
    int result = i3dmgx2_check_consistency(0xD0, 9, resp, resp_len);
    if (result != 0) {
        return result;
    }

    record->trans_qty = BYTEARRAY_TO_HS(resp + 1);
    record->timer = BYTEARRAY_TO_HL(resp + 3);

    return 0;
}


int parse_i3dmgx2_temperatures(uint8_t *resp, unsigned resp_len,
        struct i3dmgx2_temperatures *record) {
    int result = i3dmgx2_check_consistency(0xD1, 15, resp, resp_len);
    if (result != 0) {
        return result;
    }

    record->temp_acc = BYTEARRAY_TO_HS(resp + 1);
    record->temp_gyro[0] = BYTEARRAY_TO_HS(resp + 3);
    record->temp_gyro[1] = BYTEARRAY_TO_HS(resp + 5);
    record->temp_gyro[2] = BYTEARRAY_TO_HS(resp + 7);
    record->timer = BYTEARRAY_TO_HL(resp + 9);

    return 0;
}


int parse_gyrosa_angr_magv(uint8_t *resp, unsigned resp_len,
        struct gyrosa_angr_magv *record) {
    int result = i3dmgx2_check_consistency(0xD2, 43, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->stabacc, resp + 1, 3);
    i3dmgx2_parse_floats(record->angr, resp + 13, 3);
    i3dmgx2_parse_floats(record->stabmag, resp + 25, 3);
    record->timer = BYTEARRAY_TO_HL(resp + 37);

    return 0;
}


int parse_delta_ang_vel_magv(uint8_t *resp, unsigned resp_len,
        struct delta_ang_vel_magv *record) {
    int result = i3dmgx2_check_consistency(0xD3, 43, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->ang, resp + 1, 9);
    record->timer = BYTEARRAY_TO_HL(resp + 37);

    return 0;
}





















