#include <cstring>
#include <streambuf>
#include <istream>
#include <iostream>
#include <bit>

#include <endian.h>

#include "kaitai/kaitaistream.h"
#include "../onvif_realtime_ptz.h"
#include "onvif_realtime_ptz.h"

#define ITERATIONS 1

int main (int argc, char *argv[])
{
    realtime_message_t message;

    memset (&message, 0, sizeof (realtime_message_t));

    if constexpr (std::endian::native == std::endian::big)
    {
        message.magic = REALTIME_PTZ_MAGIC_NUMBER;
        message.message_type = REALTIME_PTZ_CONTINUOUS_MOVE;
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
        message.message_type = htobe16 (REALTIME_PTZ_CONTINUOUS_MOVE);
        message.u.continuous_move.pan_speed = htobe32 (0.1f);
        message.u.continuous_move.tilt_speed = htobe32 (0.2f);
        message.u.continuous_move.zoom_speed[0] = htobe32 (0.3f);
        message.u.continuous_move.zoom_speed[1] = htobe32 (0.4f);
        message.u.continuous_move.zoom_speed[2] = htobe32 (0.5f);
        message.u.continuous_move.zoom_speed[3] = htobe32 (0.6f);
        message.u.continuous_move.focus_speed[0] = htobe32 (0.7f);
        message.u.continuous_move.focus_speed[1] = htobe32 (0.8f);
        message.u.continuous_move.focus_speed[2] = htobe32 (0.9f);
        message.u.continuous_move.focus_speed[3] = htobe32 (1.0f);
    }

    for (int i = 0 ; i < ITERATIONS ; i++)
    {
        struct membuf: std::streambuf
        {
            membuf (char* base, std::ptrdiff_t n)
            {
                this->setg(base, base, base + n);
            }
        };
        membuf sbuf ((char*) &message, sizeof (realtime_message_t));
        std::istream is (&sbuf);

	kaitai::kstream ks (&is);

        // The constructor automatically reads and parses the data by default
        onvif_realtime_ptz_t data (&ks);

        std::cout << "Header: " << data.magic () << std::endl;
        std::cout << "Message type: " << data.command_id () << std::endl;
    }

    return 0;
}
