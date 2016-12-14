/*
 * Node implementation for the use of a 3DM-GX2 with ROS.
 *
 *  Created on: Nov 7, 2016
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
 * Print acc_angr_magv_orient record on stdout.
 * For debug purposes only.
 */
void print_rec(acc_angr_magv_orient *record) {
    printf("\n%f;", record->acc[0]);
    printf("%f;", record->acc[1]);
    printf("%f;", record->acc[2]);
    printf("%f;", record->angr[0]);
    printf("%f;", record->angr[1]);
    printf("%f;", record->angr[2]);
    printf("%f;", record->mag[0]);
    printf("%f;", record->mag[1]);
    printf("%f;", record->mag[2]);

    const float *M = (float*) record->matrix;
    printf("%f;%f;%f;"
           "%f;%f;%f;"
           "%f;%f;%f;\n",
           M[0], M[1], M[2],
           M[3], M[4], M[5],
           M[6], M[7], M[8]);
}


/*
 * Convert a rotation matrix to a quaternion.
 * This uses the Shepperd algorithm to make the conversion. The choice of the
 * best indices permutation to use is credited to Mike Day on "Converting a
 * Rotation Matrix to a Quaternion".
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
    ros::Publisher publisher = node.advertise<sensor_msgs::Imu>(
            "imu/data_raw",
            1000);

    if (argc < 3) {
        ROS_INFO("Incorrect use.\n"
                 "Usage: rosrun i3dmgx2 i3dmgx2_node PORT ADDRESS [DIVIDER]\n");
        exit(1);
    }

    // Open serial port to 3DM-GX2 base station.
    int fd = i3dmgx2_open_port(argv[1]);
    if (fd < 0) {
        ROS_INFO("Error trying to open serial port.");
        perror("Error trying to open serial port");
        exit(1);
    }

    if (argc > 3) {
        /* If provided, set the IMU's divider constant to configure the data
         * output rate (see section "Calculation Cycle, and Data Output Rate"
         * in the protocol specification document, page 29). */
        int divider = atoi(argv[3]);
        if (divider >= 170 && divider <= 51200) {
            // Send pack to configure data output rate
            ROS_INFO("Setting IMU divider constant to %d\n", divider);
            const unsigned ww_size = I3DMGX2_CMDP_SIZE(SIZE_WRITE_WORD_EEPROM);
            uint8_t ww_pack[ww_size];
            init_write_word_eeprom(I3DMGX2_PAYLOAD_PTR(ww_pack),
                                   0xFCA2, (u_int16_t) divider);
            i3dmgx2_init_cmdp(ww_pack, atoi(argv[2]), SIZE_WRITE_WORD_EEPROM);

            ssize_t nbytes = write(fd, ww_pack, ww_size);
            if (nbytes < 0) {
                ROS_INFO("Error writing to serial port.");
                perror("Error writing to serial port");
                exit(1);
            }
            else if (nbytes != ww_size) {
                ROS_INFO("Error: parcial write on serial port.");
                exit(1);
            }

            usleep(100000);
        }
        else {
            ROS_INFO("Divider provided is out of range [170 51200]\n");
        }
    }

    /* There is a weird bug where the continuous mode doesn't work properly
     * when it is requested as the first command. The solution found was to
     * make some requests before setting the continuous mode. */
    for (int i = 0; i < 3; ++i) {
        const unsigned aa_size = I3DMGX2_CMDP_SIZE(SIZE_ACC_ANGR);
        uint8_t aa_pack[aa_size];
        aa_pack[I3DMGX2_PAYLOAD_INDEX] = CMD_ACC_ANGR;
        i3dmgx2_init_cmdp(aa_pack, atoi(argv[2]), SIZE_ACC_ANGR);

        ssize_t nbytes = write(fd, aa_pack, aa_size);
        if (nbytes < 0) {
            ROS_INFO("Error writing to serial port.");
            perror("Error writing to serial port");
            exit(1);
        }
        else if (nbytes != aa_size) {
            ROS_INFO("Error: parcial write on serial port.");
            exit(1);
        }

        usleep(100000);
    }

    // Send pack to 3DM-GX2 with set continuous command.
    const unsigned sc_size = I3DMGX2_CMDP_SIZE(SIZE_SET_CONTINUOUS);
    uint8_t sc_pack[sc_size];
    init_set_continuous_mode(I3DMGX2_PAYLOAD_PTR(sc_pack),
                             CMD_ACC_ANGR_MAGV_ORIENT);
    i3dmgx2_init_cmdp(sc_pack, atoi(argv[2]), SIZE_SET_CONTINUOUS);

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
    uint8_t *bufend = buf;
    uint8_t *parseptr = buf;
    uint8_t *pack;
    size_t pack_size;
    int parse_result;
    acc_angr_magv_orient record;

    while (ros::ok()) {
        // Read from serial port.
        nbytes = read(fd, bufend, sizeof(buf) - (bufend - buf));

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
                // Reset bufend and parseptr if buffer is full.
                if (bufend >= buf + sizeof(buf) - 100) {
                    ROS_INFO("Buffer got full. Clearing buffer.");
                    bufend = buf;
                    parseptr = buf;
                }
                break;
            }

            // Treat pack based on its command.
            switch (I3DMGX2_CMD(pack)) {
                case CMD_SET_CONTINUOUS:
                case CMD_ACC_ANGR:
                case CMD_WRITE_WORD_EEPROM:
                    // Ignore these replies.
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
