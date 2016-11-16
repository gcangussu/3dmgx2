/*
 * i3dmgx2_integration_test.c
 *
 *  Created on: 7 de out de 2016
 *      Author: gabriel
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>

#include "i3dmgx2_io.h"
#include "i3dmgx2_protocol.h"


#define UNIT_ADDR 95


int main() {
    int stdin;
    int fd;
    int max_fd;
    fd_set input;
    struct timeval timeout;
    uint8_t buf[255];
    uint8_t *bufptr = buf;
    ssize_t n;

    /* Open file descriptors */
    stdin = 0;
    fd = i3dmgx2_open_port("/dev/ttyUSB0");

    /* Send pack to i3dmgx2 with set continuous command */
    const unsigned pack_size = I3DMGX2_CMDP_SIZE(SIZE_SET_CONTINUOUS);
    uint8_t pack[pack_size];
    init_set_continuous_mode(I3DMGX2_PAYLOAD_PTR(pack),
                             CMD_ACC_ANGR_MAGV_ORIENT);
    i3dmgx2_init_cmdp(pack, UNIT_ADDR, SIZE_SET_CONTINUOUS);

    n = write(fd, pack, pack_size);
    if (n < 0)
        perror("Failed writing to serial port");
    else if (n != pack_size)
        puts("Attention, parcial write!");

    /* Initialize the input set */
    FD_ZERO(&input);
    FD_SET(fd, &input);
    FD_SET(stdin, &input);

    max_fd = (stdin > fd ? stdin : fd) + 1;

    /* Initialize the timeout structure */
    timeout.tv_sec  = 10;
    timeout.tv_usec = 0;

    /* Do the select */
    for(int try = 0; try < 10; ++try) {
        n = select(max_fd, &input, NULL, NULL, &timeout);

        /* Check for error */
        if (n < 0)
            perror("Select failed");
        else if (n == 0)
            puts("TIMEOUT");
        else {
            /* There is input */
            if (FD_ISSET(fd, &input)) {
                /* Read serial port */
                n = read(fd, bufptr, sizeof(buf) - (bufptr - buf));
                if (n < 0)
                    perror("Failed reading serial port");
                else
                    bufptr += n;
            }

            if (FD_ISSET(stdin, &input))
                return 2;
        }
    }

    /* Close serial port */
    if (fd > 0)
        close(fd);
        puts("Closed serial port.");

    /* Print buffer */
    puts("In buffer:");
    for (uint8_t *ptr = buf; ptr < bufptr; ++ptr) {
        printf("%02X ", *ptr);
    }
    puts("");

    return 0;
}
