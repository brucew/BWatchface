#pragma once
#include <pebble.h>

#define TRIG_360 TRIG_MAX_ANGLE
#define TRIG_180 (TRIG_MAX_ANGLE / 2)
#define TRIG_120 (TRIG_MAX_ANGLE / 3)
#define TRIG_90 (TRIG_MAX_ANGLE / 4)

#define COLORS PBL_IF_COLOR_ELSE(true, false)
#define ANTIALIASING true
#define FINAL_RADIUS 70

typedef struct {
  int hours;
  int minutes;
} Time;
