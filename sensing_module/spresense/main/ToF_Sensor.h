#ifndef TOF_SENSOR_H
#define TOF_SENSOR_H

#include <VL53L0X.h>
#include "define.h"//#includeで呼び出し
#include "measure_distance.h"
#include "try_recovering_sensor.h"
#include "sensor_soft_reset.h"


class ToF_Sensor
{
private:
    VL53L0X distance_sensor;
public:

    ToF_Sensor();
    // ~ToF_Sensor();

    void setup();
    uint16_t get_distance();
    // void loop();
    void measure_distance();
    int8_t try_recovering_sensor();
    void sensor_soft_reset();

};


#endif // TOF_SENSOR_H