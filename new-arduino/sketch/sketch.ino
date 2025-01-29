#include "LinearActuator.hpp"
#include "config.hpp"

#include "../include/adapt/proto.h"

#include <HX711.h>

/// \brief The HX711 scale.
static HX711 Hx711;
/// \brief The linear actuator.
static as::LinearActuator LinAct;

static bool AreMeasuring = false;

static void reportStatus(AdaptStatusCode const Code) {
  Serial.write(static_cast<byte>(Code));
  Serial.flush();
}

static void setupSerial() { Serial.begin(ADAPT_SERIAL_BAUD); }

static void setupLinearActuator() {
  reportStatus(AdaptSC_SettingUpLinAct);
  LinAct.begin();
  LinAct.home();
  reportStatus(AdaptSC_Done);
}

static void setupHx711() {
  reportStatus(AdaptSC_SettingUpHx711);
  Hx711.begin(ADAPT_HX711_DOUT_PIN, ADAPT_HX711_SCK_PIN);
  Hx711.set_scale();
  Hx711.tare();                       // Reset the scale to 0
  Hx711.read_average();               // Get a baseline reading
  Hx711.set_scale(ADAPT_HX711_SCALE); // Adjust to this calibration
                                      // factor for force
  reportStatus(AdaptSC_Done);
}

void setup() {
  setupSerial();
  setupLinearActuator();
  setupHx711();
  reportStatus(AdaptSC_Ready);
}

static void measure() {
  float const Force = Hx711.get_units();

  reportStatus(AdaptSC_ForceMeasurement);
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

static void serveExtendLinAct16Request(byte const Body) {
  LinAct.extend(durationMsForMoveLinActRequest(Body, 16));
}

static void serveExtendLinAct512Request(byte const Body) {
  LinAct.extend(durationMsForMoveLinActRequest(Body, 512));
}

static void serveRetractLinAct16Request(byte const Body) {
  LinAct.retract(durationMsForMoveLinActRequest(Body, 16));
}

static void serveRetractLinAct512Request(byte const Body) {
  LinAct.retract(durationMsForMoveLinActRequest(Body, 512));
}

static void serveNullRequest(byte const Body) {}

static void serveRequest(byte const Req) {
  static void (*HandlerForCode[8])(byte) = {
      /* ExtendLinAct16 */ serveExtendLinAct16Request,
      /* ExtendLinAct512 */ serveExtendLinAct512Request,
      /* StopLinAct */ serveStopLinActRequest,
      /* SetMeasuring */ serveSetMeasuringRequest,
      /* RetractLinAct16*/ serveRetractLinAct16Request,
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
    reportStatus(AdaptSC_ReceivedRequest);
    serveRequest(Ch);
  }
}
