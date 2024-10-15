#ifndef TOF_SENSOR_H
#define TOF_SENSOR_H

#include <VL53L0X.h>

void measure_distance();
void sensor_soft_reset(void);
int8_t try_recovering_sensor();

class ToF_Sensor {
   private:
    VL53L0X distance_sensor;

   public:
    ToF_Sensor();
    // ~ToF_Sensor();

    void setup();
    uint16_t get_distance();
    void start_distance_sensor_process(int* LR_results, uint16_t LR_threshold);
    // void loop();
    void measure_distance();
    int8_t try_recovering_sensor();
    void sensor_soft_reset();
};

#endif  // TOF_SENSOR_H