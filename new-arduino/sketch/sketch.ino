#include "LinearActuator.hpp"
#include "config.hpp"

#include "../include/adapt/proto.h"

#include <HX711.h>

/// \brief The HX711 scale.
static HX711 Hx711;
/// \brief The linear actuator.
static adpts::LinearActuator LinAct;

static bool AreMeasuring = false;

static void reportStatus(AdaptStatusCode const Code) {
  Serial.write(static_cast<byte>(Code));
  Serial.flush();
}

static void setupSerial() { Serial.begin(ADPT_PROTO_SERIAL_BAUD); }

static void setupLinearActuator() {
  reportStatus(adpt_proto_sc_setting_up_lin_act);
  LinAct.begin();
  LinAct.home();
  reportStatus(adpt_proto_sc_done);
}

static void setupHx711() {
  reportStatus(adpt_proto_sc_setting_up_hx711);
  Hx711.begin(ADPTS_HX711_DOUT_PIN, ADPTS_HX711_SCK_PIN);
  Hx711.set_scale();
  Hx711.tare();                       // Reset the scale to 0
  Hx711.read_average();               // Get a baseline reading
  Hx711.set_scale(ADPTS_HX711_SCALE); // Adjust to this calibration
                                      // factor for force
  reportStatus(adpt_proto_sc_done);
}

void setup() {
  setupSerial();
  setupLinearActuator();
  setupHx711();
  reportStatus(adpt_proto_sc_ready);
}

static void measure() {
  float const Force = Hx711.get_units();

  reportStatus(adpt_proto_sc_force_measurement);
  for (size_t I = 0; I < sizeof(Force); I++)
    Serial.write(reinterpret_cast<unsigned char const *>(&Force)[I]);

  Serial.flush();

  // Angles const avgAngles = measureAverageAngles();

  // Serial.print(
  //     avgAngles.angleX); // update based on calibration factor and calcuation
  // Serial.print(',');
}

static void serveSetMeasuringRequest(byte const Body) { AreMeasuring = Body; }

static void serveStopLinActRequest(byte const Body) { LinAct.stop(); }

static unsigned long
durationMsForMoveLinActRequest(byte const ReqBody,
                               unsigned long const TimeUnit) {
  return TimeUnit * (1 + static_cast<unsigned long>(ReqBody));
}

static void serveExtendLinAct8Request(byte const Body) {
  LinAct.extend(durationMsForMoveLinActRequest(Body, 8));
}

static void serveExtendLinAct512Request(byte const Body) {
  LinAct.extend(durationMsForMoveLinActRequest(Body, 512));
}

static void serveRetractLinAct8Request(byte const Body) {
  LinAct.retract(durationMsForMoveLinActRequest(Body, 8));
}

static void serveRetractLinAct512Request(byte const Body) {
  LinAct.retract(durationMsForMoveLinActRequest(Body, 512));
}

static void serveNullRequest(byte const Body) {}

static void serveRequest(byte const Req) {
  static void (*HandlerForCode[8])(byte) = {
      /* ExtendLinAct8 */ serveExtendLinAct8Request,
      /* ExtendLinAct512 */ serveExtendLinAct512Request,
      /* StopLinAct */ serveStopLinActRequest,
      /* SetMeasuring */ serveSetMeasuringRequest,
      /* RetractLinAct8*/ serveRetractLinAct8Request,
      /* RetractLinAct512 */ serveRetractLinAct512Request,
      serveNullRequest,
      serveNullRequest,
  };

  byte const Body = Req & 0x1f;
  byte const Code = Req >> 5;

  auto const Handler = HandlerForCode[Code];
  Handler(Body);
}

void loop() {
  LinAct.update();

  if (AreMeasuring)
    measure();

  int const Ch = Serial.read();
  if (Ch >= 0) {
    reportStatus(adpt_proto_sc_request_received);
    serveRequest(Ch);
  }
}
