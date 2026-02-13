#ifndef _realtime_ptz_h_
#define _realtime_ptz_h_

#include <stdint.h>

#if defined (__has_include)
#if __has_include(<stdfloat.h>)
#include <stdfloat.h>
#endif
#endif

#define REALTIME_PTZ_MAGIC_NUMBER 0x4F50545A

#define REALTIME_PTZ_MAX_MOTORIZED_LENSES 4

enum _realtime_message_type_t
{
    REALTIME_PTZ_SEND_STATUS = 1,
    REALTIME_PTZ_CONTINUOUS_MOVE,
};

typedef enum _realtime_message_type_t realtime_message_type_t;

typedef struct _realtime_message_t realtime_message_t;

typedef struct _send_status_t send_status_t;

typedef struct _continuous_move_t continuous_move_t;

#pragma pack (push, 1)

struct _continuous_move_t
{
    uint32_t timeout;
    _Float32 pan_speed;
    _Float32 tilt_speed;
    _Float32 zoom_speed[REALTIME_PTZ_MAX_MOTORIZED_LENSES];
    _Float32 focus_speed[REALTIME_PTZ_MAX_MOTORIZED_LENSES];
};

struct _send_status_t
{
    /** @brief Milliseconds since the EPOCH, UTC timezone */
    uint64_t timestamp;
    _Float32 pan_position;
    _Float32 tilt_position;
    _Float32 zoom_position[REALTIME_PTZ_MAX_MOTORIZED_LENSES];
    _Float32 focus_position[REALTIME_PTZ_MAX_MOTORIZED_LENSES];
};

struct _realtime_message_t
{
    uint32_t magic;
    uint32_t message_type;
    union 
    {
        send_status_t send_status;
	continuous_move_t continuous_move;
    } u;
};

#pragma pack (pop)

#endif /* _realtime_ptz_h_ */
