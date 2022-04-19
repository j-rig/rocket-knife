/**
 * @file netstring.c
 * @author Josh Righetti
 * @date sept 2020
 * @brief this module implements reading and writing netstrings to a posix file handle
 * @see https://cr.yp.to/proto/netstrings.txt
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "rw_it.h"
#include "netstring.h"

/**
 * @brief writes byte data in the buffer to a file or socket in netstring format
 * @param h posix file descriptor
 * @param buf pointer to buffer to write
 * @param buflen length of buffer data to write
 * @return zero on sucess, negative on error
 */
int write_netstring_fd(int h, char *buf, unsigned long buflen)
{
	unsigned long j, k;
	char hdr[11];

	if (buflen > 999999999) goto erra;

	k = snprintf(hdr, sizeof(hdr), "%lu:", buflen);

	j = write_it(h, hdr, k);
	if (j < k) goto errb;

	j = write_it(h, buf, buflen);
	if (j < buflen) goto errb;

	j = write_it(h, ",", 1);
	if (j < 1) goto errb;

	return 0;

erra:
	fprintf(stderr, "netstring >999999999 bytes\n");
	return -1;

errb:
	fprintf(stderr, "netstring error writing\n");
	perror(NULL);
	return -2;
}

int write_netstring_buff(char *bufi, unsigned long bufleni, char *bufo, unsigned long bufleno)
{
	unsigned long j = 0, k = 0;

	if (bufleni > 999999999) goto erra;

	j = snprintf(bufo, bufleno, "%lu:", bufleni);
	if (j < 0) goto errb; k += j;
	memcpy(bufo + k, bufi, bufleni);
	k += bufleni;
	j = snprintf(bufo + k, bufleno - k, ",");
	if (j < 0) goto errb; k += j;

	return k;

erra:
	fprintf(stderr, "netstring >999999999 bytes\n");
	return -1;

errb:
	fprintf(stderr, "error writing netstring to buffer\n");
	return -2;
}

/**
 * @brief reads byte data from a file or socket in netstring format
 * @param h posix file descriptor
 * @param buflen pointer to store length of data read
 * @return pointer to the allocated buffer, NULL on error
 * @note need to free returned buffer as it is created with malloc
 */
char *read_netstring_fd(int h, unsigned long *buflen)
{
	char *buf = NULL;
	unsigned long i, j;
	char hdr[11];

	for (i = 0; i < sizeof(hdr) - 1; i++) {
		j = read_it(h, hdr + i, 1);
		if (j < 1) goto erra;
		if (hdr[i] == ':') break;
		if (hdr[i] != '0' && hdr[i] != '1' && hdr[i] != '2' && hdr[i] != '3'
		    && hdr[i] != '4' && hdr[i] != '5' && hdr[i] != '6' && hdr[i] != '7'
		    && hdr[i] != '8' && hdr[i] != '9') goto errb;
	}
	if (hdr[i] != ':') goto errc;
	hdr[i + 1] = 0;

	*buflen = 0;
	if (sscanf(hdr, "%9lu:", buflen) < 1) goto errb;
	if (*buflen > 999999999) goto errb;

	buf = malloc(*buflen);
	if (buf == NULL) goto errd;

	j = read_it(h, buf, *buflen);
	if (j < *buflen) goto err;

	j = read_it(h, hdr, 1);
	if (j < 1) goto err;

	if (hdr[0] != ',') goto erre;

	return buf;

err:
	free(buf);
erra:
	fprintf(stderr, "netstring error reading\n");
	perror(NULL);
	return NULL;

errb:
	fprintf(stderr, "netstring error reading length\n");
	return NULL;

errc:
	fprintf(stderr, "netstring error expected \":\"\n");
	return NULL;

errd:
	fprintf(stderr, "netstring error allocating buffer\n");
	return NULL;

erre:
	fprintf(stderr, "netstring error expected \",\"\n");
	return NULL;
}

char *read_netstring_buf(char *bufi, unsigned long bufleni, unsigned long *bufleno)
{
	fprintf(stderr, "not implimented\n");
	return NULL;
}
