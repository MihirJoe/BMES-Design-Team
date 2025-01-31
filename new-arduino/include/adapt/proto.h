#ifndef ADAPT_PROTO_H
#define ADAPT_PROTO_H

#define ADPT_PROTO_SERIAL_BAUD 115200l

// Request codes.

#define adpt_proto_rc_extend_lin_act_8 0
#define adpt_proto_rc_extend_lin_act_512 1
#define adpt_proto_rc_stop_lin_act 2
#define adpt_proto_rc_set_measuring 3
#define adpt_proto_rc_retract_lin_act_8 4
#define adpt_proto_rc_retract_lin_act_512 5

// Status codes.

#define adpt_proto_sc_force_measurement 0x23  // '#'
#define adpt_proto_sc_request_received 0x25   // '%'
#define adpt_proto_sc_done 0x2e               // '.'
#define adpt_proto_sc_ready 0x3e              // '>'
#define adpt_proto_sc_setting_up_lin_act 0x41 // 'A'
#define adpt_proto_sc_setting_up_hx711 0x42   // 'B'

#endif // ADAPT_PROTO_H
