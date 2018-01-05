#include <catch/catch.hpp>

#include "ring_buffer.h"

TEST_CASE("Add values to ring buffer", "[ring_buffer]") {
    ring_buffer_t ring;

    const size_t buffer_length = 5;
    int buffer[buffer_length] = {0};
    ring_buffer_init(buffer_length, buffer, &ring);

    REQUIRE(ring_buffer_size(&ring) == buffer_length);

    for (auto i = 0; i < buffer_length - 1; i++) {
        REQUIRE(ring_buffer_put(&ring, i));
    }

    REQUIRE(!ring_buffer_put(&ring, 0));
}

TEST_CASE("Get values from the ring buffer", "[ring_buffer]") {
    ring_buffer_t ring;

    const size_t buffer_length = 5;
    int buffer[buffer_length] = {0};
    ring_buffer_init(buffer_length, buffer, &ring);

    REQUIRE(ring_buffer_size(&ring) == buffer_length);

    for (auto i = 0; i < buffer_length - 1; i++) {
        REQUIRE(ring_buffer_put(&ring, i));
    }

    for (auto i = 0; i < buffer_length - 1; i++) {
        int item;
        REQUIRE(ring_buffer_get(&ring, &item));
        REQUIRE(item == i);
    }
}

TEST_CASE("Get capacity of ring buffer", "[ring_buffer]") {
    ring_buffer_t ring;

    const size_t buffer_length = 5;
    int buffer[buffer_length] = {0};
    ring_buffer_init(buffer_length, buffer, &ring);

    REQUIRE(ring_buffer_size(&ring) == buffer_length);

    REQUIRE(ring_buffer_is_empty(&ring));
    REQUIRE(!ring_buffer_is_full(&ring));

    for (auto i = 0; i < buffer_length - 1; i++) {
        REQUIRE(ring_buffer_put(&ring, i));

        if (i > 0 && i < buffer_length - 2) {
            REQUIRE(!ring_buffer_is_empty(&ring));
            REQUIRE(!ring_buffer_is_full(&ring));
        }
    }

    REQUIRE(!ring_buffer_is_empty(&ring));
    REQUIRE(ring_buffer_is_full(&ring));

    for (auto i = 0; i < buffer_length - 1; i++) {
        int item;
        REQUIRE(ring_buffer_get(&ring, &item));
        REQUIRE(item == i);

        if (i > 0 && i < buffer_length - 2) {
            REQUIRE(!ring_buffer_is_empty(&ring));
            REQUIRE(!ring_buffer_is_full(&ring));
        }
    }

    REQUIRE(ring_buffer_is_empty(&ring));
    REQUIRE(!ring_buffer_is_full(&ring));
}
