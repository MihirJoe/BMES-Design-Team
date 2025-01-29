#ifndef ADAPT_PROTO_H
#define ADAPT_PROTO_H

#define ADAPT_SERIAL_BAUD 115200

enum AdaptRequestCode {
  AdaptRC_ExtendLinAct16 = 0,
  AdaptRC_ExtendLinAct512 = 1,
  AdaptRC_StopLinAct = 2,
  AdaptRC_SetMeasuring = 3,
  AdaptRC_RetractLinAct16 = 4,
  AdaptRC_RetractLinAct512 = 5,
};

enum AdaptStatusCode {
  AdaptSC_Done = 0x2e,
  AdaptSC_Ready = 0x3e,
  AdaptSC_SettingUpLinAct = 0x41,
  AdaptSC_SettingUpHx711 = 0x42,
  AdaptSC_ForceMeasurement = 0x43,
  AdaptSC_ReceivedRequest = 0x23,
};

#endif // ADAPT_PROTO_H
