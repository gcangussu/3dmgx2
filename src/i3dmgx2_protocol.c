/*
 * Implement the core functions to the 3DM-GX2 protocol.
 *
 *  Created on: 25 de set de 2016
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "i3dmgx2_protocol.h"
#include "i3dmgx2_errors.h"
#include "i3dmgx2_utils.h"

#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>


/* Constant commands */
const uint8_t cmd_raw_acc_angr[SIZE_RAW_ACC_ANGR] = {CMD_RAW_ACC_ANGR};
const uint8_t cmd_acc_angr[SIZE_ACC_ANGR] = {CMD_ACC_ANGR};
const uint8_t cmd_delta_ang_vel[SIZE_DELTA_ANG_VEL] = {CMD_DELTA_ANG_VEL};
const uint8_t cmd_orient[SIZE_ORIENT] = {CMD_ORIENT};
const uint8_t cmd_orient_up[SIZE_ORIENT_UP] = {CMD_ORIENT_UP};
const uint8_t cmd_magv[SIZE_MAGV] = {CMD_MAGV};
const uint8_t cmd_acc_angr_orient[SIZE_ACC_ANGR_ORIENT] = {
        CMD_ACC_ANGR_ORIENT};
const uint8_t cmd_acc_angr_magv[SIZE_ACC_ANGR_MAGV] = {CMD_ACC_ANGR_MAGV};
const uint8_t cmd_acc_angr_magv_orient[SIZE_ACC_ANGR_MAGV_ORIENT] = {
        CMD_ACC_ANGR_MAGV_ORIENT};
const uint8_t cmd_euler_angles[SIZE_EULER_ANGLES] = {CMD_EULER_ANGLES};
const uint8_t cmd_euler_angles_angr[SIZE_EULER_ANGLES_ANGR] = {
        CMD_EULER_ANGLES_ANGR};
const uint8_t cmd_temperatures[SIZE_TEMPERATURES] = {CMD_TEMPERATURES};
const uint8_t cmd_gyrosa_angr_magv[SIZE_GYROSA_ANGR_MAGV] = {
        CMD_GYROSA_ANGR_MAGV};
const uint8_t cmd_delta_ang_vel_magv[SIZE_DELTA_ANG_VEL_MAGV] = {
        CMD_DELTA_ANG_VEL_MAGV};
const uint8_t cmd_firware_version[SIZE_FIRWARE_VERSION] = {
        CMD_FIRWARE_VERSION};
const uint8_t cmd_stop_continuous[SIZE_STOP_CONTINUOUS] = {
        CMD_STOP_CONTINUOUS};


/* Variable commands */
void init_wireless_ping(uint8_t buffer[SIZE_WIRELESS_PING], uint16_t addr) {
    buffer[0] = CMD_WIRELESS_PING;
    DEREFER_16BITS(buffer + 1) = htons(addr);
}


void init_set_continuous_mode(uint8_t buffer[SIZE_SET_CONTINUOUS],
        uint8_t cmd_byte) {
    buffer[0] = CMD_SET_CONTINUOUS;
    buffer[1] = 0xC1;
    buffer[2] = 0x29;
    buffer[3] = cmd_byte;
}


void init_write_acc_bias(uint8_t buffer[SIZE_WRITE_ACC_BIAS],
        float accbias[3]) {
    buffer[0] = CMD_WRITE_ACC_BIAS;
    buffer[1] = 0xB7;
    buffer[2] = 0x44;
    i3dmgx2_floats_to_net_bytes(buffer + 3, accbias, 3);
}


void init_write_gyro_bias(uint8_t buffer[SIZE_WRITE_GYRO_BIAS],
        float gyrobias[3]) {
    buffer[0] = CMD_WRITE_GYRO_BIAS;
    buffer[1] = 0x12;
    buffer[2] = 0xA5;
    i3dmgx2_floats_to_net_bytes(buffer + 3, gyrobias, 3);
}


void init_capture_gyro_bias(uint8_t buffer[SIZE_CAPTURE_GYRO_BIAS],
        uint16_t sampling_time) {
    buffer[0] = CMD_CAPTURE_GYRO_BIAS;
    buffer[1] = 0xC1;
    buffer[2] = 0x29;
    DEREFER_16BITS(buffer + 3) = htons(sampling_time);
}


void init_transfer_nonv_mem(uint8_t buffer[SIZE_TRANSFER_NONV_MEM],
        uint16_t transf_qty) {
    buffer[0] = CMD_TRANSFER_NONV_MEM;
    buffer[1] = 0xC1;
    buffer[2] = 0x29;
    DEREFER_16BITS(buffer + 3) = htons(transf_qty);
}


void init_write_word_eeprom(uint8_t buffer[SIZE_WRITE_WORD_EEPROM],
        uint16_t addr, uint16_t word){
    buffer[0] = CMD_WRITE_WORD_EEPROM;
    buffer[1] = 0xC1;
    buffer[2] = 0x29;
    buffer[3] = 0x00;
    DEREFER_16BITS(buffer + 4) = htons(addr);
    DEREFER_16BITS(buffer + 6) = htons(word);
}


void init_read_word_eeprom(uint8_t buffer[SIZE_READ_WORD_EEPROM],
        uint16_t addr) {
    buffer[0] = CMD_READ_WORD_EEPROM;
    buffer[1] = 0x00;
    DEREFER_16BITS(buffer + 2) = htons(addr);
}



void init_get_device_id(uint8_t buffer[SIZE_GET_DEVICE_ID], uint8_t selector) {
    buffer[0] = CMD_GET_DEVICE_ID;
    buffer[1] = selector;
}


void init_built_in_test(uint8_t buffer[SIZE_BUILT_IN_TEST],
        uint8_t config) {
    buffer[0] = CMD_BUILT_IN_TEST;
    buffer[1] = 0xC1;
    buffer[2] = 0x29;
    buffer[3] = config;
}


/* Parse functions */
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
    int result = i3dmgx2_check_consistency(0xC1, 29, resp, resp_len);
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
    int result = i3dmgx2_check_consistency(0xC2, 29, resp, resp_len);
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
    int result = i3dmgx2_check_consistency(0xC3, 29, resp, resp_len);
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
    int result = i3dmgx2_check_consistency(0xC4, 6, resp, resp_len);
    if (result != 0) {
        return result;
    }

    record->cmd_byte = resp[1];
    record->timer = BYTEARRAY_TO_HL(resp + 2);

    return 0;
}


int parse_orient_matrix(uint8_t *resp, unsigned resp_len,
        struct orient_matrix *record) {
    int result = i3dmgx2_check_consistency(0xC5, 41, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats((float*) record->matrix, resp + 1, 9);
    record->timer = BYTEARRAY_TO_HL(resp + 37);

    return 0;
}


int parse_orient_up_matrix(uint8_t *resp, unsigned resp_len,
        struct orient_up_matrix *record) {
    int result = i3dmgx2_check_consistency(0xC6, 41, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats((float*) record->matrix, resp + 1, 9);
    record->timer = BYTEARRAY_TO_HL(resp + 37);

    return 0;
}


int parse_magv_resp(uint8_t *resp, unsigned resp_len,
        struct magv_resp *record) {
    int result = i3dmgx2_check_consistency(0xC7, 17, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->mag, resp + 1, 3);
    record->timer = BYTEARRAY_TO_HL(resp + 13);

    return 0;
}


int parse_acc_angr_orient(uint8_t *resp, unsigned resp_len,
        struct acc_angr_orient *record) {
    int result = i3dmgx2_check_consistency(0xC8, 65, resp, resp_len);
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
    int result = i3dmgx2_check_consistency(0xC9, 17, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->accbias, resp + 1, 3);
    record->timer = BYTEARRAY_TO_HL(resp + 13);

    return 0;
}


int parse_gyro_bias_resp(uint8_t *resp, unsigned resp_len,
        struct gyro_bias_resp *record) {
    int result = i3dmgx2_check_consistency(0xCA, 17, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->gyrobias, resp + 1, 3);
    record->timer = BYTEARRAY_TO_HL(resp + 13);

    return 0;
}


int parse_acc_angr_magv(uint8_t *resp, unsigned resp_len,
        struct acc_angr_magv *record) {
    int result = i3dmgx2_check_consistency(0xCB, 41, resp, resp_len);
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
    int result = i3dmgx2_check_consistency(0xCC, 77, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->acc, resp + 1, 3);
    i3dmgx2_parse_floats(record->angr, resp + 13, 3);
    i3dmgx2_parse_floats(record->mag, resp + 25, 3);
    i3dmgx2_parse_floats((float*) record->matrix, resp + 37, 9);
    record->timer = BYTEARRAY_TO_HL(resp + 73);

    return 0;
}


int parse_capture_gyro_bias(uint8_t *resp, unsigned resp_len,
        struct capture_gyro_bias *record) {
    int result = i3dmgx2_check_consistency(0xCD, 17, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->gyrobias, resp + 1, 3);
    record->timer = BYTEARRAY_TO_HL(resp + 13);

    return 0;
}


int parse_euler_angles_resp(uint8_t *resp, unsigned resp_len,
        struct euler_angles_resp *record) {
    int result = i3dmgx2_check_consistency(0xCE, 17, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(&(record->roll), resp + 1, 3);
    record->timer = BYTEARRAY_TO_HL(resp + 13);

    return 0;
}


int parse_euler_angles_angr(uint8_t *resp, unsigned resp_len,
        struct euler_angles_angr *record) {
    int result = i3dmgx2_check_consistency(0xCF, 29, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(&(record->roll), resp + 1, 6);
    record->timer = BYTEARRAY_TO_HL(resp + 25);

    return 0;
}


int parse_transfer_nonv_mem(uint8_t *resp, unsigned resp_len,
        struct transfer_nonv_mem *record) {
    int result = i3dmgx2_check_consistency(0xD0, 7, resp, resp_len);
    if (result != 0) {
        return result;
    }

    record->trans_qty = BYTEARRAY_TO_HS(resp + 1);
    record->timer = BYTEARRAY_TO_HL(resp + 3);

    return 0;
}


int parse_i3dmgx2_temperatures(uint8_t *resp, unsigned resp_len,
        struct i3dmgx2_temperatures *record) {
    int result = i3dmgx2_check_consistency(0xD1, 13, resp, resp_len);
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
    int result = i3dmgx2_check_consistency(0xD2, 41, resp, resp_len);
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
    int result = i3dmgx2_check_consistency(0xD3, 41, resp, resp_len);
    if (result != 0) {
        return result;
    }

    i3dmgx2_parse_floats(record->ang, resp + 1, 9);
    record->timer = BYTEARRAY_TO_HL(resp + 37);

    return 0;
}


int parse_write_word_resp(uint8_t *resp, unsigned resp_len,
        uint_fast16_t *word) {
    int result = i3dmgx2_check_consistency(0xE4, 3, resp, resp_len);
    if (result != 0) {
        return result;
    }

    *word = BYTEARRAY_TO_HS(resp + 1);

    return 0;
}


int parse_read_word_resp(uint8_t *resp, unsigned resp_len,
        uint_fast16_t *word) {
    int result = i3dmgx2_check_consistency(0xE5, 3, resp, resp_len);
    if (result != 0) {
        return result;
    }

    *word = BYTEARRAY_TO_HS(resp + 1);

    return 0;
}


int parse_firmware_version(uint8_t *resp, unsigned resp_len,
        uint_fast32_t *dword) {
    int result = i3dmgx2_check_consistency(0xE9, 5, resp, resp_len);
    if (result != 0) {
        return result;
    }

    *dword = BYTEARRAY_TO_HL(resp + 1);

    return 0;
}


int parse_device_id_string(uint8_t *resp, unsigned resp_len,
        struct i3dmgx2_device_id *record) {
    int result = i3dmgx2_check_consistency(0xEA, 18, resp, resp_len);
    if (result != 0) {
        return result;
    }

    record->selector = resp[1];
    strncpy(record->str, (const char *) (resp + 2), 16);
    record->str[16] = '\0';  /* ensure it is a null terminated C string */

    return 0;
}


int parse_built_in_test_resp(uint8_t *resp, unsigned resp_len,
        uint8_t *config) {
    int result = i3dmgx2_check_consistency(0xFB, 2, resp, resp_len);
    if (result != 0) {
        return result;
    }

    *config = resp[1];

    return 0;
}


void i3dmgx2_init_cmdp(uint8_t *cmd_pack, uint16_t addr, unsigned cmd_len) {
    cmd_pack[0] = 0xAA;  /* Start of Packet */
    cmd_pack[1] = 0x0B;
    cmd_pack[2] = 0x00;
    DEREFER_16BITS(cmd_pack + 3) = htons(addr);
    cmd_pack[5] = (cmd_len + 1) & 0xFF;
    cmd_pack[6] = 0x00;

    const uint16_t chksum = i3dmgx2_cmd_pack_chksum(cmd_pack, cmd_len + 9);
    DEREFER_16BITS(cmd_pack + 7 + cmd_len) = htons(chksum);
}


unsigned i3dmgx2_get_node_addr(uint8_t *pack) {
    return BYTEARRAY_TO_HS(pack + 3);
}


unsigned i3dmgx2_get_data_length(uint8_t *pack) {
    const unsigned raw_len = pack[5];
    if (raw_len == 0) return 0;
    return raw_len - 1;
}


const uint8_t i3dmgx2_data_start[3] = {0xAA, 0x07, 0x00};

#ifdef __cplusplus
}
#endif
