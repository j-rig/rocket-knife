/**
 * @file rw_it.c
 * @author Josh Righetti
 * @date 2022
 */

#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include "rw_it.h"

int read_it(int fd, void *buf, int count)
{
	ssize_t i;

	while (1) {
		i = read(fd, buf, count);
		if (i < 0 && errno == EINTR) continue;
		if (i < 0) {
			return i;
		}
		break;
	}
	return i;
}

int write_it(int fd, const void *buf, int count)
{
	ssize_t i, j;

	i = 0;
	while (i < count) {
		j = write(fd, buf + i, count - i);
		if (j < 0 && errno == EINTR) continue;
		if (j < 0) {
			perror(NULL); return j;
		}
		i += j;
	}
	return i;
}
