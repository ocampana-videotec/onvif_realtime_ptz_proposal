// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "onvif_realtime_ptz.h"
#include "kaitai/exceptions.h"
std::set<onvif_realtime_ptz_t::cmd_type_t> onvif_realtime_ptz_t::_build_values_cmd_type_t() {
    std::set<onvif_realtime_ptz_t::cmd_type_t> _t;
    _t.insert(onvif_realtime_ptz_t::CMD_TYPE_SEND_STATUS);
    _t.insert(onvif_realtime_ptz_t::CMD_TYPE_CONTINUOUS_MOVE);
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
    m_command_id = static_cast<onvif_realtime_ptz_t::cmd_type_t>(m__io->read_u4be());
    n_payload = true;
    switch (command_id()) {
    case onvif_realtime_ptz_t::CMD_TYPE_CONTINUOUS_MOVE: {
        n_payload = false;
        m_payload = new continuous_move_payload_t(m__io, this, m__root);
        break;
    }
    case onvif_realtime_ptz_t::CMD_TYPE_SEND_STATUS: {
        n_payload = false;
        m_payload = new send_status_payload_t(m__io, this, m__root);
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
    m_zoom_speed = 0;
    m_focus_speed = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void onvif_realtime_ptz_t::continuous_move_payload_t::_read() {
    m_timeout = m__io->read_u4be();
    m_pan_speed = m__io->read_f4be();
    m_tilt_speed = m__io->read_f4be();
    m_zoom_speed = new std::vector<float>();
    const int l_zoom_speed = 4;
    for (int i = 0; i < l_zoom_speed; i++) {
        m_zoom_speed->push_back(m__io->read_f4be());
    }
    m_focus_speed = new std::vector<float>();
    const int l_focus_speed = 4;
    for (int i = 0; i < l_focus_speed; i++) {
        m_focus_speed->push_back(m__io->read_f4be());
    }
}

onvif_realtime_ptz_t::continuous_move_payload_t::~continuous_move_payload_t() {
    _clean_up();
}

void onvif_realtime_ptz_t::continuous_move_payload_t::_clean_up() {
    if (m_zoom_speed) {
        delete m_zoom_speed; m_zoom_speed = 0;
    }
    if (m_focus_speed) {
        delete m_focus_speed; m_focus_speed = 0;
    }
}

onvif_realtime_ptz_t::send_status_payload_t::send_status_payload_t(kaitai::kstream* p__io, onvif_realtime_ptz_t* p__parent, onvif_realtime_ptz_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_zoom_position = 0;
    m_focus_position = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void onvif_realtime_ptz_t::send_status_payload_t::_read() {
    m_pan_position = m__io->read_f4be();
    m_tilt_position = m__io->read_f4be();
    m_zoom_position = new std::vector<float>();
    const int l_zoom_position = 4;
    for (int i = 0; i < l_zoom_position; i++) {
        m_zoom_position->push_back(m__io->read_f4be());
    }
    m_focus_position = new std::vector<float>();
    const int l_focus_position = 4;
    for (int i = 0; i < l_focus_position; i++) {
        m_focus_position->push_back(m__io->read_f4be());
    }
    m_timestamp_ms = m__io->read_u8be();
}

onvif_realtime_ptz_t::send_status_payload_t::~send_status_payload_t() {
    _clean_up();
}

void onvif_realtime_ptz_t::send_status_payload_t::_clean_up() {
    if (m_zoom_position) {
        delete m_zoom_position; m_zoom_position = 0;
    }
    if (m_focus_position) {
        delete m_focus_position; m_focus_position = 0;
    }
}
