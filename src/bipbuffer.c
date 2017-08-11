#include "bipbuffer.h"

#define get_read_region(bip) ((bip)->read_from?&(bip)->b:&(bip)->a)
#define get_write_region(bip) ((bip)->write_to?&(bip)->b:&(bip)->a)
#define reset_region(region) ({(region)->start=(region)->end=0;})
#define get_region_length(region) ((region)->end-(region)->start)

struct bipbuffer* bb_create(long int size) {
    struct bipbuffer* bip = NULL;
    char *body = NULL;
    int page_size = 4 << 10, page_count = 1;

    while (page_size < size) {
        page_count++;
        page_size *= page_count;
    }

    bip = (struct bipbuffer *)malloc(sizeof(struct bipbuffer));
    memset(bip, 0, sizeof(struct bipbuffer));

    body = (char *)malloc(page_size);
    bip->data = body;
    bip->size = page_size;

    return bip;
}

void bb_destroy(struct bipbuffer *bip) {
    if (bip) {
        if (bip->data) {
            free(bip->data);
        }

        free(bip);
    }
}

void bb_clear(struct bipbuffer *bip) {
    bip->a.start = bip->a.end = 0;
    bip->b.start = bip->b.end = 0;

    bip->allocated = 0;
}

void *bb_alloc(struct bipbuffer *bip, long int size) {
    struct region *reader = get_read_region(bip);
    struct region *writer = get_write_region(bip);

    if (reader == writer) {
        if (bip->size - writer->end >= size) {
            bip->allocated = size;
        } else {
            // try activate another region
            if (reader->start < size) {
                // failed!
                return NULL;
            } else {
                // success
                // change writer
                if (bip->write_to == 0) {
                    bip->write_to = 1;
                } else {
                    bip->write_to = 0;
                }

                writer = get_write_region(bip);
                reset_region(writer);
            }
        }

        return bip->data+writer->end;
    } else {
        if (reader->start - writer->end >= size) {
            bip->allocated = size;
            return bip->data+writer->end;
        }
    }

    return NULL;
}

int bb_commit(struct bipbuffer *bip, long int used) {
    struct region *writer = get_write_region(bip);

    if (bip->allocated < used) {
        return -1;
    }

    writer->end += used;
    bip->allocated = 0;

    return 0;
}

long int bb_look(struct bipbuffer *bip, void *dst, long int size) {
    struct region *reader = get_read_region(bip);
    struct region *writer = get_write_region(bip);
    long int have_read = 0;

    if (size <= get_region_length(reader)) {
        if (dst) {
            memcpy(dst, bip->data+reader->start, size);
        }
        have_read += size;
    } else {
        if (reader == writer ||
            size > get_region_length(writer)+get_region_length(reader)) {
            // no enough data for this read
            return -1;
        }

        // cross region read

        // read first part
        have_read = get_region_length(reader);
        if (dst) {
            memcpy(dst, bip->data+reader->start, have_read);
        }

        // change reader
        if (bip->read_from == 0) {
            reader = &bip->b;
        } else {
            reader = &bip->a;
        }

        // read another part
        if (dst) {
            memcpy((char *)dst+have_read, bip->data+reader->start, size - have_read);
        }
        have_read = size;
    }

    return have_read;
}

long int bb_read(struct bipbuffer *bip, void *dst, long int size) {
    struct region *reader = get_read_region(bip);
    struct region *writer = get_write_region(bip);
    long int have_read = 0;

    if (size <= get_region_length(reader)) {
        memcpy(dst, bip->data+reader->start, size);
        have_read += size;
        reader->start += size;
    } else {
        if (reader == writer ||
            size > get_region_length(writer)+get_region_length(reader)) {
            // no enough data for this read
            return -1;
        }

        // cross region read

        // read first part
        have_read = get_region_length(reader);
        memcpy(dst, bip->data+reader->start, have_read);
        reset_region(reader);

        // change reader
        if (bip->read_from == 0) {
            bip->read_from = 1;
        } else {
            bip->read_from = 0;
        }
        reader = get_read_region(bip);

        // read another part
        memcpy((char *)dst+have_read, bip->data+reader->start, size - have_read);
        have_read = size;

        reader->start += size;
    }

    if (reader->start == reader->end) {
        reset_region(reader);
    }

    return have_read;
}
