#include "ring_buffer.h"

ring_buffer_t ring_buffer_init(size_t size, int * buffer) {
	struct ring_buffer ring;
	ring.head = 0;
	ring.tail = 0;
	ring.size = 0;
	ring.buffer = buffer;
	return ring;
}

/**
 * Given a position and a ring, return the next position in the ring. This
 * handles wrapping around correctly.
 *  @param ring The ring that position is relative to.
 *  @param position The position to advance from.
 *  @return The next position in the ring.
 */
static size_t next_position(ring_buffer_t ring, size_t position) {
	return (position + 1) % ring.size;
}

int ring_buffer_get(ring_buffer_t ring, int * item); {
	if (next_position(ring.tail) == ring.head) {
		return 0;
	} else {
		*item = ring.buffer[ring.tail];
		ring.tail = next_position(ring.tail);
		return 1;
	}
}

int ring_buffer_put(ring_buffer_t ring, int item) {
	if (next_position(ring.head) == ring.tail) {
		return 0;
	} else {
		ring.buffer[ring.head] = item;
		ring.head = next_position(ring.head);
		return 1;
	}

}

size_t ring_buffer_size(ring_buffer_t ring) {
	return ring.size;
}

