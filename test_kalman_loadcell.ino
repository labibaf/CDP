#include "HX711.h"

const int DOUT_PIN = 21;
const int SCK_PIN = 19;
const float CALIBRATION_FACTOR = 0.42;

HX711 scale;



// kalman
float SensorData,KalmanFilterData;
float Xt,Xt_update,Xt_prev;
float Pt,Pt_update, Pt_prev;
float Kt,R,Q;

void setup() {
  Serial.begin(115200);
  scale.begin(DOUT_PIN, SCK_PIN);
  scale.set_scale(CALIBRATION_FACTOR);
  scale.tare();
  R=100;Q=1;Pt_prev + Q;
}

void loop() {
  float weight = scale.get_units();
  Serial.print(weight);

  SensorData=weight;
  Xt_update=Xt_prev;
  Pt_update=Pt_prev+Q;

  Kt=Pt_update/(Pt_update+R);
  Xt=Xt_update+(Kt*(SensorData-Xt_update));
  Pt=(1-Kt)*Pt_update;

  Xt_prev=Xt;
  Pt_prev=Pt;
  
  KalmanFilterData=Xt;
  Serial.print(",");
  Serial.print(KalmanFilterData);
  Serial.println();
  delayMicroseconds(100);
}
