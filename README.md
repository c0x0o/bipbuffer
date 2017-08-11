# bipbuffer

## introduction

This project is a C implementation of [Simon Cooke's bipbuffer](https://www.codeproject.com/Articles/3479/The-Bip-Buffer-The-Circular-Buffer-with-a-Twist). Bipbuffer is a special kind of circular buffer. Just like the circular buffer, it avoids byte moving or copying during the lifetime of the buffer, but more impressive, bipbuffer don't transform the buffer pointer when it reaches the end of the buffer because bipbuffer always alloc a contiguous space for you. If you want to know more about it, see the link.

## Usage

You can just copy the files in `src` directory(including header file and source files), and compile them yourself, or you can use CMake to get a static library. Run `cmake ${bipbuffer_dir} && make` in any directory for a try, `libbipbuffer.a` is waiting for you in `lib` ^o^

## API

### data structure
```c
struct bipbuffer;
```
use it to represent a bipbuffer

### function

```c
// 'size' means the size of buffer, it will be rounded up to the nearest 'n*pagesize'
struct bipbuffer* bb_create(long int size);
void bb_destroy(struct bipbuffer *buffP);

// clear all data in buffer and reset the buffer
void bb_clear(struct bipbuffer *buffP);

// pushing data into the buffer is divided into two steps:
// first use 'bb_alloc' to get a pointer can contain 'size' bytes data,
// after the IO operation, use 'bb_commit' to confirm the quantity of data you actual write in.
void *bb_alloc(struct bipbuffer *buffP, long int size);
int bb_commit(struct bipbuffer *buffP, long int size);

// read 'size' bytes data from buffer
long int bb_read(struct bipbuffer *buffP, void *dst, long int size);

// the same as 'bb_read', but it won't remove the data from the buffer
// if you only want to know whether there is enough data in buffer, pass a NULL to 'dst'
long int bb_look(struct bipbuffer *buffP, void *dst, long int size);
```

you can see more details in `bipbuffer.h`(I guess you need them).
