/**
 * @file rw_it.h
 * @author Josh Righetti
 * @date 2022
 */

#ifndef RK_RW_IT_H
#define RK_RW_IT_H

int read_it(int fd, void *buf, int count);
int write_it(int fd, const void *buf, int count);

#endif
