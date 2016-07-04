#pragma once

#define TRIG_360 TRIG_MAX_ANGLE
#define TRIG_180 (TRIG_MAX_ANGLE / 2)
#define TRIG_120 (TRIG_MAX_ANGLE / 3)
#define TRIG_90 (TRIG_MAX_ANGLE / 4)

#define COLORS       PBL_IF_COLOR_ELSE(true, false)

#define FINAL_RADIUS 73

typedef struct {
  int hours;
  int minutes;
  bool pm;
} Time;


void date_update_proc();
void temp_update_proc();
