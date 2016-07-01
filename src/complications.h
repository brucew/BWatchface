#pragma once

#define COMPLICATION_MARGIN 20
#define COMPLICATION_CENTER_RADIUS 48
#define COMPLICATION_FONT FONT_KEY_BITHAM_34_MEDIUM_NUMBERS  
#define COMPLICATION_ORIGIN_OFFSET_X 36
#define COMPLICATION_ORIGIN_OFFSET_Y 18
#define COMPLICATION_W 74
#define COMPLICATION_H 34

extern GPoint s_center;
extern float s_complication_angles[2];
extern char s_date_buffer[4];
extern char s_temp_buffer[16];

Layer* date_create(Layer *parent_layer);
void date_update_proc();
Layer* temp_create(Layer *parent_layer);
void temp_update_proc();
