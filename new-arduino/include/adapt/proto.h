#ifndef ADAPT_PROTO_H
#define ADAPT_PROTO_H

#define ADPT_PROTO_SERIAL_BAUD 115200l

#define adpt_proto_RC_ExtendLinAct8 0
#define adpt_proto_RC_ExtendLinAct512 1
#define adpt_proto_RC_StopLinAct 2
#define adpt_proto_RC_SetMeasuring 3
#define adpt_proto_RC_RetractLinAct8 4
#define adpt_proto_RC_RetractLinAct512 5
#define adpt_proto_RC_Reserved6 6
#define adpt_proto_RC_Reserved7 7

#define adpt_proto_SC_ForceMeasurement 0x23 // '#'
#define adpt_proto_SC_RequestReceived 0x25  // '%'
#define adpt_proto_SC_Done 0x2e             // '.'
#define adpt_proto_SC_Ready 0x3e            // '>'
#define adpt_proto_SC_SettingUpLinAct 0x41  // 'A'
#define adpt_proto_SC_SettingUpHx711 0x42   // 'B'

#endif // ADAPT_PROTO_H
