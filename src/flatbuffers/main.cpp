#include <cstring>
#include <cstdio>
#include <streambuf>
#include <istream>
#include <iostream>
#include <bit>

#include <endian.h>

#include "onvif_realtime_ptz_generated.h"

#define ITERATIONS 1000000
#define VERBOSE 0

static void float_to_be (_Float32 * f)
{
    uint32_t raw_bits;

    std::memcpy(&raw_bits, f, sizeof (_Float32));

    uint32_t swapped_bits = htobe32 (raw_bits);

    std::memcpy(f, &swapped_bits, sizeof (_Float32));
}

int main (int argc, char *argv[])
{
    realtime_message_t message;
    struct timespec start, end;
    double elapsed_time_struct;
    double elapsed_time_kaitai;

    memset (&message, 0, sizeof (realtime_message_t));

    if constexpr (std::endian::native == std::endian::big)
    {
        message.magic = REALTIME_PTZ_MAGIC_NUMBER;
        message.message_type = REALTIME_PTZ_CONTINUOUS_MOVE;
        message.u.continuous_move.timeout = 0;
        message.u.continuous_move.pan_speed = 0.1f;
        message.u.continuous_move.tilt_speed = 0.2f;
        message.u.continuous_move.zoom_speed[0] = 0.3f;
        message.u.continuous_move.zoom_speed[1] = 0.4f;
        message.u.continuous_move.zoom_speed[2] = 0.5f;
        message.u.continuous_move.zoom_speed[3] = 0.6f;
        message.u.continuous_move.focus_speed[0] = 0.7f;
        message.u.continuous_move.focus_speed[1] = 0.8f;
        message.u.continuous_move.focus_speed[2] = 0.9f;
        message.u.continuous_move.focus_speed[3] = 1.0f;
    }
    else
    {
        message.magic = htobe32 (REALTIME_PTZ_MAGIC_NUMBER);
        message.message_type = htobe32 (REALTIME_PTZ_CONTINUOUS_MOVE);
        message.u.continuous_move.timeout = htobe32 (0);
        message.u.continuous_move.pan_speed = 0.1f;
	float_to_be (&message.u.continuous_move.pan_speed);
        message.u.continuous_move.tilt_speed = 0.2f;
	float_to_be (&message.u.continuous_move.tilt_speed);
        message.u.continuous_move.zoom_speed[0] = 0.3f;
	float_to_be (&message.u.continuous_move.zoom_speed[0]);
        message.u.continuous_move.zoom_speed[1] = 0.4f;
	float_to_be (&message.u.continuous_move.zoom_speed[1]);
        message.u.continuous_move.zoom_speed[2] = 0.5f;
	float_to_be (&message.u.continuous_move.zoom_speed[2]);
        message.u.continuous_move.zoom_speed[3] = 0.6f;
	float_to_be (&message.u.continuous_move.zoom_speed[3]);
        message.u.continuous_move.focus_speed[0] = 0.7f;
	float_to_be (&message.u.continuous_move.focus_speed[0]);
        message.u.continuous_move.focus_speed[1] = 0.8f;
	float_to_be (&message.u.continuous_move.focus_speed[1]);
        message.u.continuous_move.focus_speed[2] = 0.9f;
	float_to_be (&message.u.continuous_move.focus_speed[2]);
        message.u.continuous_move.focus_speed[3] = 1.0f;
	float_to_be (&message.u.continuous_move.focus_speed[3]);
    }

    clock_gettime (CLOCK_MONOTONIC, &start);
    for (int i = 0 ; i < ITERATIONS ; i++)
    {
        _Float32 f;
	uint32_t u;

	u = be32toh (message.message_type);
	(void) u;
	u = be32toh (message.u.continuous_move.timeout);
	(void) u;

	f = message.u.continuous_move.pan_speed;
	float_to_be (&f);
	f = message.u.continuous_move.tilt_speed;
	float_to_be (&f);
	f = message.u.continuous_move.zoom_speed[0];
	float_to_be (&f);
	f = message.u.continuous_move.zoom_speed[1];
	float_to_be (&f);
	f = message.u.continuous_move.zoom_speed[2];
	float_to_be (&f);
	f = message.u.continuous_move.zoom_speed[3];
	float_to_be (&f);
	f = message.u.continuous_move.focus_speed[0];
	float_to_be (&f);
	f = message.u.continuous_move.focus_speed[1];
	float_to_be (&f);
	f = message.u.continuous_move.focus_speed[2];
	float_to_be (&f);
	f = message.u.continuous_move.focus_speed[3];
	float_to_be (&f);
    }
    clock_gettime (CLOCK_MONOTONIC, &end);

    elapsed_time_struct = (end.tv_sec - start.tv_sec) +
                          (end.tv_nsec - start.tv_nsec) / 1e9;

    std::cout << "Struct took " << elapsed_time_struct << "seconds to execute." << std::endl;
    std::cout << "Average iteation time " << (elapsed_time_struct / ITERATIONS) << std::endl;

    return 0;
}
