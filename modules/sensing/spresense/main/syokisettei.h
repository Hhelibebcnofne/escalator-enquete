// センサ初期設定
Wire.begin();
distance_sensor.setTimeout(500);
sensor_soft_reset();