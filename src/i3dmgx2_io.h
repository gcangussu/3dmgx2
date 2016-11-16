/*
 * i3dmgx2_io.h
 *
 *  Created on: 7 de out de 2016
 *      Author: gabriel
 */

#ifndef I3DMGX2_IO_H_
#define I3DMGX2_IO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>


/*
 * Open serial port for i3dmgx2.
 *
 * Returns the file descriptor on success or -1 on error.
 */
int i3dmgx2_open_port(const char *port);


/*
 * Parse a buffer to check if there is any data packet on it.
 *
 * Save the pointer to the start of the data packet in data_pack and
 * this packet size on data_pack_size.
 *
 * Returns 0 if there is a data pack on the buffer, 1 if there is a
 * incomplete data pack on the buffer and -1 if there is no data
 * packet on the buffer. On incomplete data packets the data_pack_size
 * represents the size of the incomplete data packet, on -1 this value
 * is undefined.
 */
int i3dmgx2_parse_buffer(const uint8_t *buf, size_t buf_size,
                         uint8_t **data_pack, size_t *data_pack_size);

#ifdef __cplusplus
}
#endif

#endif /* I3DMGX2_IO_H_ */
