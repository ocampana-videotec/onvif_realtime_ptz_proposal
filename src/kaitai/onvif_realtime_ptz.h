#ifndef ONVIF_REALTIME_PTZ_H_
#define ONVIF_REALTIME_PTZ_H_

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

class onvif_realtime_ptz_t;

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <set>

#if KAITAI_STRUCT_VERSION < 11000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.11 or later is required"
#endif

/**
 * Realtime ONVIF PTZ Specification
 * Author: ONVIF Technical Specification https://www.onvif.org/profiles/specifications/
 * License: ONVIF Contributor License Agreement https://github.com/onvif/specs/blob/development/LICENSE.md
 * Version: 26.12
 */

class onvif_realtime_ptz_t : public kaitai::kstruct {

public:
    class continuous_move_payload_t;
    class get_status_payload_t;

    enum cmd_type_t {
        CMD_TYPE_CONTINUOUS_MOVE = 1,
        CMD_TYPE_GET_STATUS = 2
    };
    static bool _is_defined_cmd_type_t(cmd_type_t v);

private:
    static const std::set<cmd_type_t> _values_cmd_type_t;
    static std::set<cmd_type_t> _build_values_cmd_type_t();

public:

    onvif_realtime_ptz_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, onvif_realtime_ptz_t* p__root = 0);

private:
    void _read();
    void _clean_up();

public:
    ~onvif_realtime_ptz_t();

    class continuous_move_payload_t : public kaitai::kstruct {

    public:

        continuous_move_payload_t(kaitai::kstream* p__io, onvif_realtime_ptz_t* p__parent = 0, onvif_realtime_ptz_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~continuous_move_payload_t();

    private:
        uint16_t m_timeout;
        float m_velocity_pan;
        float m_velocity_tilt;
        float m_velocity_zoom;
        onvif_realtime_ptz_t* m__root;
        onvif_realtime_ptz_t* m__parent;

    public:
        uint16_t timeout() const { return m_timeout; }
        float velocity_pan() const { return m_velocity_pan; }
        float velocity_tilt() const { return m_velocity_tilt; }
        float velocity_zoom() const { return m_velocity_zoom; }
        onvif_realtime_ptz_t* _root() const { return m__root; }
        onvif_realtime_ptz_t* _parent() const { return m__parent; }
    };

    class get_status_payload_t : public kaitai::kstruct {

    public:

        get_status_payload_t(kaitai::kstream* p__io, onvif_realtime_ptz_t* p__parent = 0, onvif_realtime_ptz_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~get_status_payload_t();

    private:
        float m_pan_position;
        float m_tilt_position;
        float m_zoom_position;
        uint64_t m_timestamp_ms;
        onvif_realtime_ptz_t* m__root;
        onvif_realtime_ptz_t* m__parent;

    public:
        float pan_position() const { return m_pan_position; }
        float tilt_position() const { return m_tilt_position; }
        float zoom_position() const { return m_zoom_position; }

        /**
         * Milliseconds since 1970-01-01 00:00:00
         */
        uint64_t timestamp_ms() const { return m_timestamp_ms; }
        onvif_realtime_ptz_t* _root() const { return m__root; }
        onvif_realtime_ptz_t* _parent() const { return m__parent; }
    };

private:
    std::string m_magic;
    cmd_type_t m_command_id;
    kaitai::kstruct* m_payload;
    bool n_payload;

public:
    bool _is_null_payload() { payload(); return n_payload; };

private:
    onvif_realtime_ptz_t* m__root;
    kaitai::kstruct* m__parent;

public:
    std::string magic() const { return m_magic; }
    cmd_type_t command_id() const { return m_command_id; }
    kaitai::kstruct* payload() const { return m_payload; }
    onvif_realtime_ptz_t* _root() const { return m__root; }
    kaitai::kstruct* _parent() const { return m__parent; }
};

#endif  // ONVIF_REALTIME_PTZ_H_
