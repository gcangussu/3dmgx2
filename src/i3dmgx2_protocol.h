/*
 * Define macros and funtions for the 3DM-GX2 protocol.
 *
 *  Created on: 18 de set de 2016
 */

#ifndef I3DMGX2_PROTOCOL_H_
#define I3DMGX2_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "i3dmgx2_utils.h"


/* Macros for handling packages */
#define I3DMGX2_ADDR_INDEX 3
#define I3DMGX2_ADDR(pack) BYTEARRAY_TO_HS((pack) + I3DMGX2_ADDR_INDEX)

#define I3DMGX2_PAYLOAD_INDEX 7
#define I3DMGX2_PAYLOAD_PTR(pack) ((pack) + I3DMGX2_PAYLOAD_INDEX)
#define I3DMGX2_PAYLOAD_LEN(pack) ((unsigned)(pack)[I3DMGX2_LEN_INDEX] - 1)
#define I3DMGX2_CMD(pack) ((pack)[I3DMGX2_PAYLOAD_INDEX])

#define I3DMGX2_LEN_INDEX 5

#define I3DMGX2_CMDP_OVERHEAD 8
#define I3DMGX2_CMDP_LEN(pack) ((pack)[[I3DMGX2_LEN_INDEX] \
                                 + I3DMGX2_CMDP_OVERHEAD)
#define I3DMGX2_CMDP_MINSIZE 10
#define I3DMGX2_CMDP_SIZE(cmd_size) ((cmd_size) + I3DMGX2_CMDP_OVERHEAD + 1)

#define I3DMGX2_DATAP_OVERHEAD 10
#define I3DMGX2_DATAP_LEN(pack) ((pack)[I3DMGX2_LEN_INDEX] \
                                 + I3DMGX2_DATAP_OVERHEAD)
#define I3DMGX2_DATAP_MINSIZE 13

/* Commands */
#define CMD_WIRELESS_PING        0x02
#define CMD_RAW_ACC_ANGR         0xC1
#define CMD_ACC_ANGR             0xC2
#define CMD_DELTA_ANG_VEL        0xC3
#define CMD_SET_CONTINUOUS       0xC4
#define CMD_ORIENT               0xC5
#define CMD_ORIENT_UP            0xC6
#define CMD_MAGV                 0xC7
#define CMD_ACC_ANGR_ORIENT      0xC8
#define CMD_WRITE_ACC_BIAS       0xC9
#define CMD_WRITE_GYRO_BIAS      0xCA
#define CMD_ACC_ANGR_MAGV        0xCB
#define CMD_ACC_ANGR_MAGV_ORIENT 0xCC
#define CMD_CAPTURE_GYRO_BIAS    0xCD
#define CMD_EULER_ANGLES         0xCE
#define CMD_EULER_ANGLES_ANGR    0xCF
#define CMD_TRANSFER_NONV_MEM    0xD0
#define CMD_TEMPERATURES         0xD1
#define CMD_GYROSA_ANGR_MAGV     0xD2
#define CMD_DELTA_ANG_VEL_MAGV   0xD3
#define CMD_WRITE_WORD_EEPROM    0xE4
#define CMD_READ_WORD_EEPROM     0xE5
#define CMD_FIRWARE_VERSION      0xE9
#define CMD_GET_DEVICE_ID        0xEA
#define CMD_STOP_CONTINUOUS      0xFA
#define CMD_BUILT_IN_TEST        0xFB


/* Commands' total sizes in bytes */
#define SIZE_WIRELESS_PING        3
#define SIZE_RAW_ACC_ANGR         1
#define SIZE_ACC_ANGR             1
#define SIZE_DELTA_ANG_VEL        1
#define SIZE_SET_CONTINUOUS       4
#define SIZE_ORIENT               1
#define SIZE_ORIENT_UP            1
#define SIZE_MAGV                 1
#define SIZE_ACC_ANGR_ORIENT      1
#define SIZE_WRITE_ACC_BIAS       15
#define SIZE_WRITE_GYRO_BIAS      15
#define SIZE_ACC_ANGR_MAGV        1
#define SIZE_ACC_ANGR_MAGV_ORIENT 1
#define SIZE_CAPTURE_GYRO_BIAS    5
#define SIZE_EULER_ANGLES         1
#define SIZE_EULER_ANGLES_ANGR    1
#define SIZE_TRANSFER_NONV_MEM    5
#define SIZE_TEMPERATURES         1
#define SIZE_GYROSA_ANGR_MAGV     1
#define SIZE_DELTA_ANG_VEL_MAGV   1
#define SIZE_WRITE_WORD_EEPROM    8
#define SIZE_READ_WORD_EEPROM     4
#define SIZE_FIRWARE_VERSION      1
#define SIZE_GET_DEVICE_ID        2
#define SIZE_STOP_CONTINUOUS      1
#define SIZE_BUILT_IN_TEST        4


/* Constant commands */
extern const uint8_t cmd_raw_acc_angr[SIZE_RAW_ACC_ANGR];
extern const uint8_t cmd_acc_angr[SIZE_ACC_ANGR];
extern const uint8_t cmd_delta_ang_vel[SIZE_DELTA_ANG_VEL];
extern const uint8_t cmd_orient[SIZE_ORIENT];
extern const uint8_t cmd_orient_up[SIZE_ORIENT_UP];
extern const uint8_t cmd_magv[SIZE_MAGV];
extern const uint8_t cmd_acc_angr_orient[SIZE_ACC_ANGR_ORIENT];
extern const uint8_t cmd_acc_angr_magv[SIZE_ACC_ANGR_MAGV];
extern const uint8_t cmd_acc_angr_magv_orient[SIZE_ACC_ANGR_MAGV_ORIENT];
extern const uint8_t cmd_euler_angles[SIZE_EULER_ANGLES];
extern const uint8_t cmd_euler_angles_angr[SIZE_EULER_ANGLES_ANGR];
extern const uint8_t cmd_temperatures[SIZE_TEMPERATURES];
extern const uint8_t cmd_gyrosa_angr_magv[SIZE_GYROSA_ANGR_MAGV];
extern const uint8_t cmd_delta_ang_vel_magv[SIZE_DELTA_ANG_VEL_MAGV];
extern const uint8_t cmd_firware_version[SIZE_FIRWARE_VERSION];
extern const uint8_t cmd_stop_continuous[SIZE_STOP_CONTINUOUS];


/* Wireless Ping */
void init_wireless_ping(uint8_t buffer[SIZE_WIRELESS_PING], uint16_t addr);

int parse_wireless_ping(uint8_t *resp, unsigned resp_len, int *status);


/* Raw Accelerometer and Angular Rate Sensor */
struct raw_acc_angr {
    float acc[3];
    float angr[3];
    uint_fast32_t timer;
};

int parse_raw_acc_angr(uint8_t *resp, unsigned resp_len,
        struct raw_acc_angr *record);


/* Acceleration and Angular Rate */
struct acc_angr {
    float acc[3];
    float angr[3];
    uint_fast32_t timer;
};

int parse_acc_angr(uint8_t *resp, unsigned resp_len,
        struct acc_angr *record);


/* DeltaAngle & DeltaVelocity */
struct delta_ang_vel {
    float ang[3];
    float vel[3];
    uint_fast32_t timer;
};

int parse_delta_ang_vel(uint8_t *resp, unsigned resp_len,
        struct delta_ang_vel *record);


/* Set Continuous Mode */
void init_set_continuous_mode(uint8_t buffer[SIZE_SET_CONTINUOUS],
        uint8_t cmd_byte);

struct set_continuous_resp {
    uint_fast32_t timer;
    uint_fast8_t cmd_byte;
};

int parse_set_continuous(uint8_t *resp, unsigned resp_len,
        struct set_continuous_resp *record);


/* Orientation Matrix */
struct orient_matrix {
    float matrix[3][3];
    uint_fast32_t timer;
};

int parse_orient_matrix(uint8_t *resp, unsigned resp_len,
        struct orient_matrix *record);


/* Orientation Update Matrix */
struct orient_up_matrix {
    float matrix[3][3];
    uint_fast32_t timer;
};

int parse_orient_up_matrix(uint8_t *resp, unsigned resp_len,
        struct orient_up_matrix *record);


/* Scaled Magnetometer Vector */
struct magv_resp {
    float mag[3];
    uint_fast32_t timer;
};

int parse_magv_resp(uint8_t *resp, unsigned resp_len,
        struct magv_resp *record);


/* Acceleration, Angular Rate & Orientation Matrix */
struct acc_angr_orient {
    float acc[3];
    float angr[3];
    float matrix[3][3];
    uint_fast32_t timer;
};

int parse_acc_angr_orient(uint8_t *resp, unsigned resp_len,
        struct acc_angr_orient *record);


/* Write Accelerometer Bias Correction */
void init_write_acc_bias(uint8_t buffer[SIZE_WRITE_ACC_BIAS],
        float accbias[3]);

struct acc_bias_resp {
    float accbias[3];
    uint_fast32_t timer;
};

int parse_acc_bias_resp(uint8_t *resp, unsigned resp_len,
        struct acc_bias_resp *record);


/* Write Gyro Bias Correction */
void init_write_gyro_bias(uint8_t buffer[SIZE_WRITE_GYRO_BIAS],
        float gyrobias[3]);

struct gyro_bias_resp {
    float gyrobias[3];
    uint_fast32_t timer;
};

int parse_gyro_bias_resp(uint8_t *resp, unsigned resp_len,
        struct gyro_bias_resp *record);


/* Acceleration, Angular Rate & Magnetometer Vector */
struct acc_angr_magv {
    float acc[3];
    float angr[3];
    float mag[3];
    uint_fast32_t timer;
};


int parse_acc_angr_magv(uint8_t *resp, unsigned resp_len,
        struct acc_angr_magv *record);


/* Acceleration, Angular Rate, Magnetometer Vectors & Orientation Matrix */
struct acc_angr_magv_orient {
    float acc[3];
    float angr[3];
    float mag[3];
    float matrix[3][3];
    uint_fast32_t timer;
};

int parse_acc_angr_magv_orient(uint8_t *resp, unsigned resp_len,
        struct acc_angr_magv_orient *record);


/* Capture Gyro Bias */
void init_capture_gyro_bias(uint8_t buffer[SIZE_CAPTURE_GYRO_BIAS],
        uint16_t sampling_time);

struct capture_gyro_bias {
    float gyrobias[3];
    uint_fast32_t timer;
};

int parse_capture_gyro_bias(uint8_t *resp, unsigned resp_len,
        struct capture_gyro_bias *record);


/* Euler Angles */
struct euler_angles_resp {
    float roll;
    float pitch;
    float yaw;
    uint_fast32_t timer;
};

int parse_euler_angles_resp(uint8_t *resp, unsigned resp_len,
        struct euler_angles_resp *record);


/* Euler Angles and Angular Rates */
struct euler_angles_angr {
    float roll;
    float pitch;
    float yaw;
    float angr[3];
    uint_fast32_t timer;
};

int parse_euler_angles_angr(uint8_t *resp, unsigned resp_len,
        struct euler_angles_angr *record);


/* Transfer Quantity to Non-Volatile Memory */
void init_transfer_nonv_mem(uint8_t buffer[SIZE_TRANSFER_NONV_MEM],
        uint16_t transf_qty);

struct transfer_nonv_mem {
    uint_fast32_t timer;
    uint_fast16_t trans_qty;
};

int parse_transfer_nonv_mem(uint8_t *resp, unsigned resp_len,
        struct transfer_nonv_mem *record);


/* Temperatures */
struct i3dmgx2_temperatures {
    uint_fast16_t temp_acc;
    uint_fast16_t temp_gyro[3];
    uint_fast32_t timer;
};

int parse_i3dmgx2_temperatures(uint8_t *resp, unsigned resp_len,
        struct i3dmgx2_temperatures *record);


/* Gyro Stabilized Acceleration, Angular Rate & Magnetometer Vector */
struct gyrosa_angr_magv {
    float stabacc[3];
    float angr[3];
    float stabmag[3];
    uint_fast32_t timer;
};

int parse_gyrosa_angr_magv(uint8_t *resp, unsigned resp_len,
        struct gyrosa_angr_magv *record);


/* DeltaAngle, DeltaVelocity & Magnetometer Vectors */
struct delta_ang_vel_magv {
    float ang[3];
    float vel[3];
    float mag[3];
    uint_fast32_t timer;
};

int parse_delta_ang_vel_magv(uint8_t *resp, unsigned resp_len,
        struct delta_ang_vel_magv *record);


/* Write Word to EEPROM */
void init_write_word_eeprom(uint8_t buffer[SIZE_WRITE_WORD_EEPROM],
        uint16_t addr, uint16_t word);

int parse_write_word_resp(uint8_t *resp, unsigned resp_len,
        uint_fast16_t *word);


/* Read Word from EEPROM */
void init_read_word_eeprom(uint8_t buffer[SIZE_READ_WORD_EEPROM],
        uint16_t addr);

int parse_read_word_resp(uint8_t *resp, unsigned resp_len,
        uint_fast16_t *word);


/* Read Firmware Version Number */
int parse_firmware_version(uint8_t *resp, unsigned resp_len,
        uint_fast32_t *dword);


/* Read Device Identifier String */
void init_get_device_id(uint8_t buffer[SIZE_GET_DEVICE_ID],
        uint8_t selector);

struct i3dmgx2_device_id {
    char str[17];  /* 16 + 1 bytes for the null termination */
    uint_fast8_t selector;
};

int parse_device_id_string(uint8_t *resp, unsigned resp_len,
        struct i3dmgx2_device_id *record);


/* Built-in Test */
void init_built_in_test(uint8_t buffer[SIZE_BUILT_IN_TEST],
        uint8_t config);

int parse_built_in_test_resp(uint8_t *resp, unsigned resp_len,
        uint8_t *config);


/*
 * Initialize a command packet.
 */
void i3dmgx2_init_cmdp(uint8_t *cmd_pack, uint16_t addr, unsigned cmd_len);


/*
 * Array with the fixed beginning of a packet.
 */
extern const uint8_t i3dmgx2_data_start[3];

#ifdef __cplusplus
}
#endif

#endif /* I3DMGX2_PROTOCOL_H_ */
