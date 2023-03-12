#define MLX90640_DEBUG

#include <Arduino.h>

#include <Wire.h>
#include "MLX.h"
#define ADDR 0x33
MLX mlx(Wire);

void setup()
{
  Serial.begin(1500000);
  Serial2.begin(115200);
  mlx.init();
}

void push_serial()
{
  Serial2.printf("%.5f,%.5f\n", mlx.analysis.ex, mlx.analysis.ey);
}

void loop()
{
  static long last_img = millis();

  mlx.read_img();
  mlx.preprocess();
  mlx.detect_centroid();
  mlx.tx_current_image();
  mlx.tx_timings();
  mlx.tx_analysis();

  push_serial();

  if (Serial.available())
  {
    mlx.tx_debugf("Serial Byte RXd!! %d", Serial.available());
  }

  while (Serial.available())
  {
    mlx.rx(Serial.read());
  }
}