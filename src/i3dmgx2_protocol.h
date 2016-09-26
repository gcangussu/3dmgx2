/*
 * i3dmgx2_protocol.h
 *
 *  Created on: 18 de set de 2016
 *      Author: gabriel
 */


#ifndef I3DMGX2_PROTOCOL_H_
#define I3DMGX2_PROTOCOL_H_

#include <stdint.h>


/* Commands to Bytes Mapping */
#define CMD_WIRELESS_PING(addr)  {0x02, addr >> 8, addr & 0xFF}
#define CMD_RAW_ACC_ANGR         {0xC1}
#define CMD_ACC_ANGR             {0xC2}
#define CMD_DELTA_ANG_VEL        {0xC3}
#define CMD_SET_CONTINUOUS(cmd)  {0xC4, 0xC1, 0x29, (cmd)}
#define CMD_ORIENT               {0xC5}
#define CMD_ORIENT_UP            {0xC6}
#define CMD_MAGV                 {0xC7}
#define CMD_ACC_ANGR_ORIENT      {0xC8}
#define CMD_WRITE_ACC_BIAS       {0xC9, 0xB7, 0x44} // TODO
#define CMD_WRITE_GYRO_BIAS      {0xCA}
#define CMD_ACC_ANGR_MAGV        {0xCB}
#define CMD_ACC_ANGR_MAGV_ORIENT {0xCC}
#define CMD_CAPTURE_GYRO_BIAS    {0xCD}
#define CMD_EULER_ANGLES         {0xCE}
#define CMD_EULER_ANGLES_ANGR    {0xCF}
#define CMD_TRANSFER_NONV_MEM    {0xD0}
#define CMD_TEMPERATURES         {0xD1}
#define CMD_GYROSA_ANGR_MAGV     {0xD2}
#define CMD_DELTA_ANG_VEL_MAGV   {0xD3}
#define CMD_WRITE_WORD_EEPROM    {0xE4}
#define CMD_READ_WORD_EEPROM     {0xE5}
#define CMD_FIRWARE_VERSION      {0xE9}
#define CMD_GET_DEVICE_ID        {0xEA}
#define CMD_STOP_CONTINIOUS      {0xFA}
#define CMD_BUILT_IN_TEST        {0xFB}


/* Wireless Ping */
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
struct acc_bias_resp {
    float accbias[3];
    uint_fast32_t timer;
};

int parse_acc_bias_resp(uint8_t *resp, unsigned resp_len,
        struct acc_bias_resp *record);










#endif /* I3DMGX2_PROTOCOL_H_ */
