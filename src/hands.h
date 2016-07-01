#pragma once

#define HAND_MARGIN  20
#define MINUTE_HAND_COLOR GColorWhite
#define HOUR_HAND_COLOR GColorFolly

extern Time g_time;
extern GPoint s_center;
extern float s_minute_angle;
extern float s_hour_angle;
extern float s_complication_angles[2];
extern int s_radius;

Layer* hands_create(Layer *parent_layer);
void hands_update_proc(Layer *layer, GContext *ctx);
