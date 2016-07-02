#include <pebble.h>
#include "main.h"
#include "accel.h"

void accel_data_handler(AccelData *data, uint32_t num_samples) {
  // Activate backlight if tilted toward the wearer
  if (data[1].x < -400 || data[1].x > 400) return;
  if (data[1].y > 0) return;
  if (data[1].did_vibrate) return;
  if (((data[1].y * 1.2) - data[0].y) > -200) return;
  
  light_enable_interaction();
  
}

void accel_subscribe() {
  accel_data_service_subscribe(2, accel_data_handler);

}


void accel_unsubscribe() {
  accel_data_service_unsubscribe();

}
