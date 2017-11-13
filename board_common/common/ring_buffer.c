#include "ring_buffer.h"

void ring_buffer_init(size_t size, int * buffer, ring_buffer_t * out) {
	out->head = 0;
	out->tail = 0;
	out->size = size;
	out->buffer = buffer;
}

/**
 * Given a position and a ring, return the next position in the ring. This
 * handles wrapping around correctly.
 *  @param ring The ring that position is relative to.
 *  @param position The position to advance from.
 *  @return The next position in the ring.
 */
static size_t next_position(ring_buffer_t * ring, size_t position) {
	return (position + 1) % ring->size;
}

int ring_buffer_get(ring_buffer_t * ring, int * item) {
	if (ring_buffer_is_empty(ring)) {
		return 0;
	} else {
		*item = ring->buffer[ring->tail];
		ring->tail = next_position(ring, ring->tail);
		return 1;
	}
}

int ring_buffer_put(ring_buffer_t * ring, int item) {
	if (ring_buffer_is_full(ring)) {
		return 0;
	} else {
		ring->buffer[ring->head] = item;
		ring->head = next_position(ring, ring->head);
		return 1;
	}
}

size_t ring_buffer_size(ring_buffer_t * ring) {
	return ring->size;
}

bool ring_buffer_is_empty(ring_buffer_t * ring) {
	return (ring->head == ring->tail);
}

bool ring_buffer_is_full(ring_buffer_t * ring) {
	return (next_position(ring, ring->head) == ring->tail);
}
