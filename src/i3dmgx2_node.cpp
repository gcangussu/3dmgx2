/*
 * i3dmgx2_node.cpp
 *
 *  Created on: Nov 7, 2016
 *      Author: Gabriel Mesquita Cangussu (gabrielcangussu@gmail.com)
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>

#include "ros/ros.h"
#include "sensor_msgs/Imu.h"

#include "i3dmgx2_io.h"
#include "i3dmgx2_protocol.h"


/*
 * Convert a rotation matrix to a quaternion.
 * Based on "Converting a Rotation Matrix to a Quaternion" by Mike Day.
 * (https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2015/01/matrix-to-quat.pdf)
 */
void rotmatrix_to_quaternion(float m[3][3],
        sensor_msgs::Imu::_orientation_type *q) {
    double t;

    if (m[2][2] < 0) {
        if (m[0][0] > m[1][1]) {
            t = 1 + m[0][0] - m[1][1] - m[2][2];
            q->x = t;
            q->y = m[0][1] + m[1][0];
            q->z = m[2][0] + m[0][2];
            q->w = m[1][2] - m[2][1];
        }
        else {
            t = 1 - m[0][0] + m[1][1] - m[2][2];
            q->x = m[0][1] + m[1][0];
            q->y = t;
            q->z = m[1][2] + m[2][1];
            q->w = m[2][0] - m[0][2];
        }
    }
    else {
        if (m[0][0] < -m[1][1]) {
            t = 1 - m[0][0] - m[1][1] + m[2][2];
            q->x = m[2][0] + m[0][2];
            q->y = m[1][2] + m[2][1];
            q->z = t;
            q->w = m[0][1] - m[1][0];
        }
        else {
            t = 1 + m[0][0] + m[1][1] + m[2][2];
            q->x = m[1][2] - m[2][1];
            q->y = m[2][0] - m[0][2];
            q->z = m[0][1] - m[1][0];
            q->w = t;
        }
    }

    const double k = 0.5 / sqrt(t);
    q->x *= k;
    q->y *= k;
    q->z *= k;
    q->w *= k;
}


/*
 * Initialize a sensor_msgs::Imu with the data from the 3DM-GX2 record.
 */
void init_imu_msg(sensor_msgs::Imu *imu_msg_ptr,
        acc_angr_magv_orient *record) {
    // Using the actual time in the message header.
    imu_msg_ptr->header.stamp = ros::Time::now();

    // Using /imu frame_id.
    imu_msg_ptr->header.frame_id = "/imu";

    // Initialize orientation quaternion with rotation matrix.
    rotmatrix_to_quaternion(record->matrix, &(imu_msg_ptr->orientation));

    // Unknown orientation covariance.
    imu_msg_ptr->orientation_covariance.fill(0.0);

    // Initialize angular velocity.
    imu_msg_ptr->angular_velocity.x = record->angr[0];
    imu_msg_ptr->angular_velocity.y = record->angr[1];
    imu_msg_ptr->angular_velocity.z = record->angr[2];

    // Unknown angular velocity covariance.
    imu_msg_ptr->angular_velocity_covariance.fill(0.0);

    // Initialize acceleration converting from g's to m/(s^2).
    imu_msg_ptr->linear_acceleration.x = record->acc[0] * 9.80665;
    imu_msg_ptr->linear_acceleration.y = record->acc[1] * 9.80665;
    imu_msg_ptr->linear_acceleration.z = record->acc[2] * 9.80665;

    // Unknown acceleration covariance.
    imu_msg_ptr->linear_acceleration_covariance.fill(0.0);
}


int main(int argc, char **argv) {
    // Initialize ROS variables.
    ros::init(argc, argv, "i3dmgx2");
    ros::NodeHandle node;
    ros::Publisher publisher = node.advertise<sensor_msgs::Imu>("imu/data",
                                                                1000);

    // Open serial port to 3DM-GX2 base station.
    int fd = i3dmgx2_open_port("/dev/ttyUSB0");  // TODO: put argument for port
    if (fd < 0) {
        ROS_INFO("Error trying to open serial port.");
        perror("Error trying to open serial port");
        exit(1);
    }

    // Send pack to 3DM-GX2 with set continuous command.
    const unsigned sc_size = I3DMGX2_CMDP_SIZE(SIZE_SET_CONTINUOUS);
    uint8_t sc_pack[sc_size];
    init_set_continuous_mode(I3DMGX2_PAYLOAD_PTR(sc_pack),
                             CMD_ACC_ANGR_MAGV_ORIENT);
    i3dmgx2_init_cmdp(sc_pack, 95, SIZE_SET_CONTINUOUS);  // TODO: change 95 to argument

    ssize_t nbytes = write(fd, sc_pack, sc_size);
    if (nbytes < 0) {
        ROS_INFO("Error writing to serial port.");
        perror("Error writing to serial port");
        exit(1);
    }
    else if (nbytes != sc_size) {
        ROS_INFO("Error: parcial write on serial port.");
        exit(1);
    }

    uint8_t buf[4095];
    //uint8_t buf[] = {0xAA, 0x00, 0x00, 0x00, 0x5F, 0x03, 0x12, 0x09, 0x6D, 0x19, 0xC7, 0xAA, 0xAA, 0x07, 0x00, 0x00, 0x5F, 0x07, 0x00, 0xC4, 0xCC, 0x5E, 0xD6, 0xAE, 0xB9, 0x26, 0xD4, 0x04, 0x98, 0xAA, 0x07, 0x00, 0x00, 0x5F, 0x4E, 0xAA, 0x07, 0x00, 0x7B, 0xD4, 0xDA, 0x3D, 0x17, 0xC3, 0xDD, 0xBD, 0x99, 0x7C, 0x3D, 0xBC, 0x0A, 0x2F, 0xFC, 0xBC, 0xD0, 0x94, 0xA6, 0xBB, 0xB4, 0x7D, 0x59, 0xFF, 0xC0, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x00, 0x3E, 0x82, 0x1E, 0x21, 0x3E, 0x07, 0x05, 0x00, 0xBF, 0x75, 0x48, 0x8E, 0xBF, 0x30, 0xCE, 0x22, 0x3F, 0x37, 0xDF, 0xCB, 0xBD, 0xAC, 0xBB, 0xD6, 0x3F, 0x2D, 0x54, 0x77, 0x3F, 0x2E, 0xE4, 0x04, 0x3E, 0x8C, 0x15, 0x5D, 0x4E, 0xD9, 0xAE, 0xB9, 0x27, 0xD5, 0x24, 0xC6, 0xAA, 0x07, 0x00, 0x00, 0x5F, 0x4E, 0x00, 0xCC, 0x3F, 0x7B, 0xF4, 0x56, 0x3D, 0x15, 0x72, 0x3B, 0xBD, 0x9A, 0xCA, 0xB0, 0xBC, 0x15, 0xF6, 0xAC, 0xBC, 0xE9, 0xB0, 0x5A, 0xBB, 0xDB, 0x5D, 0xB1, 0xFF, 0xC0, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x00, 0x3E, 0x82, 0x26, 0xF1, 0x3E, 0x07, 0x10, 0x64, 0xBF, 0x75, 0x46, 0xFF, 0xBF, 0x30, 0xD2, 0x14, 0x3F, 0x37, 0xDB, 0xAD, 0xBD, 0xAC, 0xD2, 0x2C, 0x3F, 0x2D, 0x4E, 0xCB, 0x3F, 0x2E, 0xE7, 0xE6, 0x3E, 0x8C, 0x1E, 0x92, 0x5E, 0xDC, 0xAE, 0xB9, 0x26, 0xD4, 0x27, 0x74, 0xAA, 0x07, 0x00, 0x00, 0x5F, 0x4E, 0x00, 0xCC, 0x3F, 0x7C, 0x0E, 0xD3, 0x3D, 0x15, 0x2C, 0x4C, 0xBD, 0x9A, 0x53, 0x4E, 0xBB, 0xAD, 0xD1, 0x29, 0xBC, 0xC1, 0xBC, 0xE5, 0xBB, 0xCB, 0x72, 0x18, 0xFF, 0xC0, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x00, 0x3E, 0x82, 0x2A, 0xF1, 0x3E, 0x07};
    uint8_t *bufend = buf;
    uint8_t *parseptr = buf;
    uint8_t *pack;
    size_t pack_size;
    int parse_result;
    acc_angr_magv_orient record;

    while (true) {
        // Read from serial port.
        nbytes = read(fd, bufend, sizeof(buf) - (bufend - buf));

        // Check if should stop running the process.
        if (!ros::ok()) {
            break;
        }

        // In case of error.
        if (nbytes < 0) {
            ROS_INFO("Failed reading serial port.");
            perror("Failed reading serial port");
            continue;
        }

        // In case of timeout
        if (nbytes == 0) {
            ROS_INFO("Timeout while reading serial port.");
            continue;
        }

        // Move bufend by nbytes (read from serial port).
        bufend += nbytes;

        // Parse buffer searching for received packs.
        while (true) {
            parse_result = i3dmgx2_parse_buffer(parseptr,
                                                bufend - parseptr,
                                                &pack,
                                                &pack_size);

            // In case of pack not found.
            if (parse_result != 0) {
                const size_t desloc = parseptr - buf;
                const size_t buf_left = sizeof(buf) - desloc;

                // Clear unused buffer if space left is smaller than 100 bytes.
                if (buf_left < 100) {
                    memcpy(buf, parseptr, bufend - parseptr);
                    bufend = bufend - desloc;
                    parseptr = buf;
                }
                // Or else, reset bufend and parseptr if buffer is full.
                else if (bufend >= buf + sizeof(buf)) {
                    bufend = buf;
                    parseptr = buf;
                }
                break;
            }

            // Treat pack based on its command.
            switch (I3DMGX2_CMD(pack)) {
                case CMD_SET_CONTINUOUS:
                    // Ignore set continuous reply.
                    break;

                case CMD_ACC_ANGR_MAGV_ORIENT:
                    parse_result = parse_acc_angr_magv_orient(
                            I3DMGX2_PAYLOAD_PTR(pack),
                            I3DMGX2_PAYLOAD_LEN(pack),
                            &record);

                    if (parse_result == 0) {
                        // Initialize Imu message and publish it.
                        sensor_msgs::Imu imu_msg;
                        init_imu_msg(&imu_msg, &record);
                        publisher.publish(imu_msg);
                    }
                    else {
                        ROS_INFO("Error parsing received pack.");
                    }
                    break;

                default:
                    ROS_INFO("Received unexpected response from 3DM-GX2.");
                    break;
            }

            // Move parseptr to end of parsed pack.
            parseptr = pack + pack_size;

            // If parseptr is at end of buffer, reset parseptr and bufend.
            if (parseptr >= bufend) {
                parseptr = buf;
                bufend = buf;
                break;  // Stop search as buffer is empty now.
            }
        }
    }

    close(fd);  // Close file descriptor.
    return 0;
}
