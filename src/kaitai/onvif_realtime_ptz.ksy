meta:
  id: onvif_realtime_ptz
  title: Realtime ONVIF PTZ Specification
  license: LicenseRef-ONVIF-Contributor-License-Agreement
  endian: be # Define network byte order (Big Endian)

doc: |
  Realtime ONVIF PTZ Specification
  Author: ONVIF Technical Specification https://www.onvif.org/profiles/specifications/
  License: ONVIF Contributor License Agreement https://github.com/onvif/specs/blob/development/LICENSE.md
  Version: 26.12

enums:
  cmd_type:
    1: continuous_move
    2: get_status

seq:
  - id: magic
    contents: [ 0x4F, 0x50, 0x54, 0x5A] # Validates the "OPTZ" header
  - id: command_id
    type: u2
    enum: cmd_type
  - id: payload
    type:
      switch-on: command_id
      cases:
        'cmd_type::continuous_move': continuous_move_payload
        'cmd_type::get_status': get_status_payload

types:
  continuous_move_payload:
    seq:
      - id: timeout
        type: u2
      - id: velocity_pan
        type: f4
      - id: velocity_tilt
        type: f4
      - id: velocity_zoom
        type: f4

  get_status_payload:
    seq:
      - id: pan_position
        type: f4
      - id: tilt_position
        type: f4
      - id: zoom_position
        type: f4
      - id: timestamp_ms
        type: u8
        doc: Milliseconds since 1970-01-01 00:00:00
