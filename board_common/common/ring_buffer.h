#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

/*
 * The internal representation of the ring buffer. This should not be relied
 * upon. Assume it will change. Direct member access is discouraged.
 */
struct ring_buffer {
	size_t head;
	size_t tail;
	size_t size;
	int * buffer;
};

typedef struct ring_buffer ring_buffer_t;

/**
 * Create a ring buffer with the given size over the given buffer. Note that if
 * the buffer is freed or goes out of scope, then the ring buffer will probably
 * get corrupted.
 * @param size The size of the ring buffer. If you intend to use the entire
 *             buffer, then this should just be the size of it.
 * @param buffer The buffer to write to. This must not be freed before the user
 *               is done using the buffer.
 */
void ring_buffer_t ring_buffer_init(size_t size, int * buffer);

/**
 * Get a value from the ring buffer.
 * @param ring The ring buffer to get from.
 * @param item The address to write the value to.
 * @return True on success. False if the buffer was empty. If you are not sure
 *         if the buffer will be empty, you should always check the return
 *         value.
 */
int ring_buffer_get(ring_buffer_t ring, int * item);

/**
 * Put a value into the ring buffer.
 * @param ring The ring buffer to put to.
 * @param item The item to put into the buffer.
 * @return True on success. False if the buffer was full. If you are not sure
 *         if the bfufer will be full, you should always check the return value.
 */
int ring_buffer_put(ring_buffer_t ring, int item);

/**
 * Provides the size of the ring buffer. This function doesn't assume anything
 * about the layout of the ring buffer and will always be safe to use.
 * @param ring The ring buffer to get the size of.
 * @return The size of the ring buffer.
 */
size_t ring_buffer_size(ring_buffer_t ring);

#endif

