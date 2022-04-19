/**
 * @file netstring.h
 * @author Josh Righetti
 * @date sept 2022
 * @brief this module implements reading and writing netstrings to a posix file handle
 * @see https://cr.yp.to/proto/netstrings.txt
 */

#ifndef _NETSTRING_H
#define _NETSTRING_H

int write_netstring_fd( int h, char *buf, unsigned long buflen);
int write_netstring_buff(char *bufi, unsigned long bufleni, char *bufo, unsigned long bufleno);
char *read_netstring_fd( int h, unsigned long *buflen);
char *read_netstring_buf( char *bufi, unsigned long bufleni, unsigned long *bufleno);

#endif
