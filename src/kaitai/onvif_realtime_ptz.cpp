// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "onvif_realtime_ptz.h"
#include "kaitai/exceptions.h"
std::set<onvif_realtime_ptz_t::cmd_type_t> onvif_realtime_ptz_t::_build_values_cmd_type_t() {
    std::set<onvif_realtime_ptz_t::cmd_type_t> _t;
    _t.insert(onvif_realtime_ptz_t::CMD_TYPE_CONTINUOUS_MOVE);
    _t.insert(onvif_realtime_ptz_t::CMD_TYPE_GET_STATUS);
    return _t;
}
const std::set<onvif_realtime_ptz_t::cmd_type_t> onvif_realtime_ptz_t::_values_cmd_type_t = onvif_realtime_ptz_t::_build_values_cmd_type_t();
bool onvif_realtime_ptz_t::_is_defined_cmd_type_t(onvif_realtime_ptz_t::cmd_type_t v) {
    return onvif_realtime_ptz_t::_values_cmd_type_t.find(v) != onvif_realtime_ptz_t::_values_cmd_type_t.end();
}

onvif_realtime_ptz_t::onvif_realtime_ptz_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, onvif_realtime_ptz_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root ? p__root : this;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void onvif_realtime_ptz_t::_read() {
    m_magic = m__io->read_bytes(4);
    if (!(m_magic == std::string("\x4F\x50\x54\x5A", 4))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x4F\x50\x54\x5A", 4), m_magic, m__io, std::string("/seq/0"));
    }
    m_command_id = static_cast<onvif_realtime_ptz_t::cmd_type_t>(m__io->read_u2be());
    n_payload = true;
    switch (command_id()) {
    case onvif_realtime_ptz_t::CMD_TYPE_CONTINUOUS_MOVE: {
        n_payload = false;
        m_payload = new continuous_move_payload_t(m__io, this, m__root);
        break;
    }
    case onvif_realtime_ptz_t::CMD_TYPE_GET_STATUS: {
        n_payload = false;
        m_payload = new get_status_payload_t(m__io, this, m__root);
        break;
    }
    }
}

onvif_realtime_ptz_t::~onvif_realtime_ptz_t() {
    _clean_up();
}

void onvif_realtime_ptz_t::_clean_up() {
    if (!n_payload) {
        if (m_payload) {
            delete m_payload; m_payload = 0;
        }
    }
}

onvif_realtime_ptz_t::continuous_move_payload_t::continuous_move_payload_t(kaitai::kstream* p__io, onvif_realtime_ptz_t* p__parent, onvif_realtime_ptz_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void onvif_realtime_ptz_t::continuous_move_payload_t::_read() {
    m_timeout = m__io->read_u2be();
    m_velocity_pan = m__io->read_f4be();
    m_velocity_tilt = m__io->read_f4be();
    m_velocity_zoom = m__io->read_f4be();
}

onvif_realtime_ptz_t::continuous_move_payload_t::~continuous_move_payload_t() {
    _clean_up();
}

void onvif_realtime_ptz_t::continuous_move_payload_t::_clean_up() {
}

onvif_realtime_ptz_t::get_status_payload_t::get_status_payload_t(kaitai::kstream* p__io, onvif_realtime_ptz_t* p__parent, onvif_realtime_ptz_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void onvif_realtime_ptz_t::get_status_payload_t::_read() {
    m_pan_position = m__io->read_f4be();
    m_tilt_position = m__io->read_f4be();
    m_zoom_position = m__io->read_f4be();
    m_timestamp_ms = m__io->read_u8be();
}

onvif_realtime_ptz_t::get_status_payload_t::~get_status_payload_t() {
    _clean_up();
}

void onvif_realtime_ptz_t::get_status_payload_t::_clean_up() {
}
