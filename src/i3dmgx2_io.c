/*
 * i3dmgx2_io.c
 *
 *  Created on: 7 de out de 2016
 *      Author: gabriel
 */

#define _GNU_SOURCE
#include <string.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#include "i3dmgx2_protocol.h"
#include "i3dmgx2_io.h"


int i3dmgx2_open_port(char *port) {
    int fd;
    struct termios options;

    fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("Unable to open port");
    }
    else fcntl(fd, F_SETFL, 0);

    /* Get the current options for the port. */
    tcgetattr(fd, &options);

    /* Set the baud rate to 115200. */
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    /* Enable the receiver and set local mode. */
    options.c_cflag |= (CLOCAL | CREAD);

    /* Set 8 data bits, no parity, 1 stop bit */
    options.c_cflag &= (unsigned) ~PARENB;
    options.c_cflag &= (unsigned) ~CSTOPB;
    options.c_cflag &= (unsigned) ~CSIZE;
    options.c_cflag |= CS8;

    /* Set raw input */
    options.c_lflag &= (unsigned) ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= (unsigned) ~OPOST;

    /* Set timeout to 1 second */
    options.c_cc[VMIN]  = 0;
    options.c_cc[VTIME] = 10;

    /* Set the new options for the port. */
    tcsetattr(fd, TCSANOW, &options);

    return fd;
}


int i3dmgx2_parse_buffer(const uint8_t *buf, size_t buf_size,
                         uint8_t **datap_ptr, size_t *datap_size_ptr) {
    /* Find start of data in buffer. */
    *datap_ptr = (uint8_t*) memmem(buf, buf_size,
            i3dmgx2_data_start, sizeof(i3dmgx2_data_start));

    /* Nothing found. */
    if (*datap_ptr == NULL)
        return -1;

    /* First check if the buffer have the necessary data to parse. */
    const size_t left_size = (buf_size - (size_t)(*datap_ptr - buf));
    if (left_size >= I3DMGX2_DATAP_MINSIZE) {
        /* If there is the minimun data size, we can check the
         * response length to ensure the whole data pack was received. */
        *datap_size_ptr = (size_t) I3DMGX2_DATAP_LEN(*datap_ptr);
        if (*datap_size_ptr <= left_size) {
            return 0;  /* Complete data pack on buffer */
        }
    }

    *datap_size_ptr = left_size;
    return 1;  /* Partial data pack on buffer */
}


































