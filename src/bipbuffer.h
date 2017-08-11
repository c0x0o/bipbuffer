#ifndef BIPBUFFER_BIPBUFFER_H_
#define BIPBUFFER_BIPBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>

struct region {
    long int start;
    long int end;
};

struct bipbuffer {
    char *data;
    long int size;
    struct region a, b;
    // 0: region 'a'
    // 1: region 'b'
    short read_from;
    short write_to;
    // record how many bytes you allocated since last call
    long int allocated;
};

// create 'size' bytes long bipbuffer,
// 'size' will be rounded up to the nearest page size
struct bipbuffer* bb_create(long int size);
void bb_destroy(struct bipbuffer *buffP);

// clear all data in buffer
void bb_clear(struct bipbuffer *buffP);

/**
 * To use the bipbuffer, you need to perform two steps:
 * First alloc a "size" bytes area and put all your data (should less equal than "size" bytes) in it;
 * and commit the exact bytes you use.
 * To perform the first step, use 'bb_alloc',
 * and for the second, use 'bb_commit'
 * */

// return a pointer which refer to a memory area has at least "size" bytes space
// return NULL when there is no more contiguous 'size' bytes space
void *bb_alloc(struct bipbuffer *buffP, long int size);

// commit how many bytes you have used in the last IO operation
// @param size    space used since last allocation
// return -1 when commit failed(use to much)
// return 0 when success
int bb_commit(struct bipbuffer *buffP, long int size);

// read 'size' bytes data into dst from bipbuffer
long int bb_read(struct bipbuffer *buffP, void *dst, long int size);

// the same as 'bb_read', but it won't remove data from the buffer
// if you only want to know whether there is enough data in buffer, pass a NULL to 'dst'
long int bb_look(struct bipbuffer *bip, void *dst, long int size);

#ifdef __cplusplus
} // end of extern "C"
#endif

#endif
