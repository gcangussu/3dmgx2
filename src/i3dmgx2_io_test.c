/*
 * Test for the i3dmgx2_io.h
 *
 *  Created on: 24 de out de 2016
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "i3dmgx2_io.h"
#include "i3dmgx2_protocol.h"


int main() {
    uint8_t buf[] = {0xAA, 0x00, 0x00, 0x00, 0x5F, 0x03, 0x12, 0x09, 0x6D, 0x19, 0xC7, 0xAA, 0xAA, 0x07, 0x00, 0x00, 0x5F, 0x07, 0x00, 0xC4, 0xCC, 0x5E, 0xD6, 0xAE, 0xB9, 0x26, 0xD4, 0x04, 0x98, 0xAA, 0x07, 0x00, 0x00, 0x5F, 0x4E, 0xAA, 0x07, 0x00, 0x7B, 0xD4, 0xDA, 0x3D, 0x17, 0xC3, 0xDD, 0xBD, 0x99, 0x7C, 0x3D, 0xBC, 0x0A, 0x2F, 0xFC, 0xBC, 0xD0, 0x94, 0xA6, 0xBB, 0xB4, 0x7D, 0x59, 0xFF, 0xC0, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x00, 0x3E, 0x82, 0x1E, 0x21, 0x3E, 0x07, 0x05, 0x00, 0xBF, 0x75, 0x48, 0x8E, 0xBF, 0x30, 0xCE, 0x22, 0x3F, 0x37, 0xDF, 0xCB, 0xBD, 0xAC, 0xBB, 0xD6, 0x3F, 0x2D, 0x54, 0x77, 0x3F, 0x2E, 0xE4, 0x04, 0x3E, 0x8C, 0x15, 0x5D, 0x4E, 0xD9, 0xAE, 0xB9, 0x27, 0xD5, 0x24, 0xC6, 0xAA, 0x07, 0x00, 0x00, 0x5F, 0x4E, 0x00, 0xCC, 0x3F, 0x7B, 0xF4, 0x56, 0x3D, 0x15, 0x72, 0x3B, 0xBD, 0x9A, 0xCA, 0xB0, 0xBC, 0x15, 0xF6, 0xAC, 0xBC, 0xE9, 0xB0, 0x5A, 0xBB, 0xDB, 0x5D, 0xB1, 0xFF, 0xC0, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x00, 0x3E, 0x82, 0x26, 0xF1, 0x3E, 0x07, 0x10, 0x64, 0xBF, 0x75, 0x46, 0xFF, 0xBF, 0x30, 0xD2, 0x14, 0x3F, 0x37, 0xDB, 0xAD, 0xBD, 0xAC, 0xD2, 0x2C, 0x3F, 0x2D, 0x4E, 0xCB, 0x3F, 0x2E, 0xE7, 0xE6, 0x3E, 0x8C, 0x1E, 0x92, 0x5E, 0xDC, 0xAE, 0xB9, 0x26, 0xD4, 0x27, 0x74, 0xAA, 0x07, 0x00, 0x00, 0x5F, 0x4E, 0x00, 0xCC, 0x3F, 0x7C, 0x0E, 0xD3, 0x3D, 0x15, 0x2C, 0x4C, 0xBD, 0x9A, 0x53, 0x4E, 0xBB, 0xAD, 0xD1, 0x29, 0xBC, 0xC1, 0xBC, 0xE5, 0xBB, 0xCB, 0x72, 0x18, 0xFF, 0xC0, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x00, 0x3E, 0x82, 0x2A, 0xF1, 0x3E, 0x07};
    uint8_t *bufptr = buf;
    uint8_t *pack;
    size_t pack_size;
    int result;

    while (1) {
        result = i3dmgx2_parse_buffer(bufptr,
                                      sizeof(buf) - (size_t)(bufptr - buf),
                                      &pack,
                                      &pack_size);

        if (result == 0) {
            switch (I3DMGX2_CMD(pack)) {
                case CMD_SET_CONTINUOUS: {
                    struct set_continuous_resp record;
                    int result = parse_set_continuous(
                            I3DMGX2_PAYLOAD_PTR(pack),
                            I3DMGX2_PAYLOAD_LEN(pack),
                            &record);

                    if (result == 0) {
                        puts("Response for CMD_SET_CONTINUOUS");
                        printf("Command byte: 0x%02X\n", record.cmd_byte);
                    }
                    else {
                        puts("Failed parsing CMD_SET_CONTINUOUS response.");
                    }
                    break;
                }

                case CMD_ACC_ANGR_MAGV_ORIENT: {
                    struct acc_angr_magv_orient record;
                    int result = parse_acc_angr_magv_orient(
                            I3DMGX2_PAYLOAD_PTR(pack),
                            I3DMGX2_PAYLOAD_LEN(pack),
                            &record);

                    if (result == 0) {
                        puts("Response for CMD_ACC_ANGR_MAGV_ORIENT");
                        puts("       X     Y     Z");

                        float x, y, z;
                        x = record.acc[0];
                        y = record.acc[1];
                        z = record.acc[2];
                        printf("Acc:  % 3.2f % 3.2f % 3.2f\n", x, y, z);

                        x = record.angr[0];
                        y = record.angr[1];
                        z = record.angr[2];
                        printf("AngR: % 3.2f % 3.2f % 3.2f\n", x, y, z);

                        x = record.mag[0];
                        y = record.mag[1];
                        z = record.mag[2];
                        printf("Mag:  % 3.2f % 3.2f % 3.2f\n", x, y, z);

                        const float *M = (float*) record.matrix;
                        printf("Or. Matrix: |% 3.2f  % 3.2f  % 3.2f|\n"
                               "            |% 3.2f  % 3.2f  % 3.2f|\n"
                               "            |% 3.2f  % 3.2f  % 3.2f|\n",
                               M[0], M[1], M[2],
                               M[3], M[4], M[5],
                               M[6], M[7], M[8]);
                    }
                    else {
                        puts("Failed parsing CMD_ACC_ANGR_MAGV_ORIENT resp.");
                    }
                    break;
                }

                default:
                    puts("Unrecognizable response received.");
                    break;
            }
        }

        if (result >= 0) {
            bufptr = pack + pack_size;
            puts("----- Start of packet -----");
            for (size_t i = 0; i < pack_size; ++i) {
                printf("0x%02X ", pack[i]);
            }
            if (result > 0)
                puts("\n----- End of partial packet -----");
            else
                puts("\n----- End of packet -----\n");
        }
        else
            break;
    }

    return 0;
}
