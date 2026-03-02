#include <cstring>
#include <cstdio>
#include <streambuf>
#include <istream>
#include <iostream>
#include <bit>

#include <endian.h>

#include "onvif_realtime_ptz_generated.h"
#include "../onvif_realtime_ptz.h"

using namespace Onvif::Realtime;
using namespace flatbuffers;

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

    flatbuffers::FlatBufferBuilder builder(1024);
    // 1. Prepare data for the repeated f4 (Float4) fields
    // In FlatBuffers, structs are built inline.
    Float4 zoom_speeds(1.0f, 0.5f, 0.0f, -0.5f);
    Float4 focus_speeds(0.2f, 0.2f, 0.2f, 0.2f);

    // 2. Create the ContinuousMovePayload
    // We use the generated CreateContinuousMovePayload helper.
    auto move_payload = CreateContinuousMovePayload(
        builder,
        500,           // timeout
        0.8f,          // pan_speed
        -0.3f,         // tilt_speed
        &zoom_speeds,  // zoom_speed (Float4)
        &focus_speeds  // focus_speed (Float4)
    );

    // 3. Create the root PTZPacket
    // Note: 'payload_type' tells the union which table it's holding.
    // 'payload' takes the offset we just created.
    auto packet_offset = CreatePTZPacket(
        builder,
        1330664538,                    // magic ("OPTZ")
        CommandType_ContinuousMove,    // command_id
        Payload_ContinuousMovePayload, // payload_type (Union discriminator)
        move_payload.Union()           // payload (The actual table offset)
    );

    // 4. Finalize the buffer
    FinishPTZPacketBuffer(builder, packet_offset);


    // 5. Get the pointer and size
    uint8_t* buffer_ptr = builder.GetBufferPointer();
    size_t buffer_size = builder.GetSize();

    // 6. Copy into a vector
    std::vector<uint8_t> packet_data(buffer_ptr, buffer_ptr + buffer_size);

    clock_gettime (CLOCK_MONOTONIC, &start);
    for (int i = 0 ; i < ITERATIONS ; i++)
    {
        // 1. Safety First: Verify the buffer
        // This ensures the internal offsets are valid and stay within the buffer's memory.
        flatbuffers::Verifier verifier(buffer_ptr, buffer_size);
        if (!VerifyPTZPacketBuffer(verifier)) {
            std::cerr << "Invalid packet received!" << std::endl;
            break;
        }

        // 2. Map the root object
        const PTZPacket* packet = GetPTZPacket(buffer_ptr);
    
        // 3. Optional: Check for the "OPTZ" magic identifier
        if (!PTZPacketBufferHasIdentifier(buffer_ptr)) {
            std::cerr << "Not an ONVIF PTZ packet (Magic mismatch)." << std::endl;
            break;
        }
    
        // 4. Extract the payload based on the Union type
        switch (packet->payload_type()) {
            case Payload_ContinuousMovePayload: {
                auto move = packet->payload_as_ContinuousMovePayload();
                //std::cout << "--- Continuous Move Received ---" << std::endl;
                //satd::cout << "Timeout: " << move->timeout() << "ms" << std::endl;
                //std::cout << "Pan Speed: " << move->pan_speed() << std::endl;
                
                // Accessing the Float4 struct fields
                if (move->zoom_speed()) {
                    //std::cout << "Zoom Speed (idx 0): " << move->zoom_speed()->val1() << std::endl;
                }
                break;
            }
            case Payload_SendStatusPayload: {
                auto status = packet->payload_as_SendStatusPayload();
                //std::cout << "--- Status Update Received ---" << std::endl;
                //std::cout << "Timestamp: " << status->timestamp_ms() << std::endl;
                //std::cout << "Pan Position: " << status->pan_position() << std::endl;
                break;
            }
            default:
                std::cout << "Unknown or empty payload." << std::endl;
                break;
        }
    }
    clock_gettime (CLOCK_MONOTONIC, &end);

    elapsed_time_struct = (end.tv_sec - start.tv_sec) +
                          (end.tv_nsec - start.tv_nsec) / 1e9;

    std::cout << "Flatbuffers took " << elapsed_time_struct << "seconds to execute." << std::endl;
    std::cout << "Average iteation time " << (elapsed_time_struct / ITERATIONS) << std::endl;

    return 0;
}
