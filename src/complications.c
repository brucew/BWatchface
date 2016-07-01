#include <pebble.h>
#include "main.h"
#include "complications.h"

#define COMPLICATION_MARGIN 20
#define COMPLICATION_CENTER_RADIUS 48
#define COMPLICATION_FONT FONT_KEY_BITHAM_34_MEDIUM_NUMBERS  
#define COMPLICATION_ORIGIN_OFFSET_X 36
#define COMPLICATION_ORIGIN_OFFSET_Y 18
#define COMPLICATION_W 74
#define COMPLICATION_H 34

extern GPoint g_center;
extern float s_complication_angles[2];
extern char g_date_buffer[4];
extern char g_temp_buffer[16];


static GRect s_date_frame;
static TextLayer *s_date_text_layer;
static Layer *s_date_layer;

static GRect s_temp_frame;
static TextLayer *s_temp_text_layer;
static Layer *s_temp_layer;


Layer* date_create(Layer *parent_layer) {
  s_date_frame = GRect(g_center.x + COMPLICATION_MARGIN, g_center.y + COMPLICATION_MARGIN, g_center.x - COMPLICATION_MARGIN, g_center.y - COMPLICATION_MARGIN);
  s_date_text_layer = text_layer_create(s_date_frame);
  
  text_layer_set_background_color(s_date_text_layer, GColorClear);
  text_layer_set_text_color(s_date_text_layer, GColorWhite);
  text_layer_set_font(s_date_text_layer, fonts_get_system_font(COMPLICATION_FONT));
  text_layer_set_text_alignment(s_date_text_layer, GTextAlignmentCenter);

  s_date_layer = text_layer_get_layer(s_date_text_layer);
  layer_add_child(parent_layer, s_date_layer);  

  return s_date_layer;
}

void date_update() {
  float date_angle = s_complication_angles[0];

  GRect date_frame = (GRect) {
    .origin = (GPoint) {
      .x = (int16_t)(sin_lookup(date_angle) * (int32_t)COMPLICATION_CENTER_RADIUS / TRIG_MAX_RATIO) + g_center.x - COMPLICATION_ORIGIN_OFFSET_X,
      .y = (int16_t)(-cos_lookup(date_angle) * (int32_t)COMPLICATION_CENTER_RADIUS / TRIG_MAX_RATIO) + g_center.y - COMPLICATION_ORIGIN_OFFSET_Y
    },
    .size = (GSize) {
      .w = COMPLICATION_W,
      .h = COMPLICATION_H
    },
  };
  layer_set_frame(s_date_layer, date_frame);
  text_layer_set_text(s_date_text_layer, g_date_buffer);

}

Layer* temp_create(Layer *parent_layer) {
  s_temp_frame = GRect(g_center.x + COMPLICATION_MARGIN, g_center.y + COMPLICATION_MARGIN, g_center.x - COMPLICATION_MARGIN, g_center.y - COMPLICATION_MARGIN);
  s_temp_text_layer = text_layer_create(s_temp_frame);
  
  text_layer_set_background_color(s_temp_text_layer, GColorClear);
  text_layer_set_text_color(s_temp_text_layer, GColorWhite);
  text_layer_set_font(s_temp_text_layer, fonts_get_system_font(COMPLICATION_FONT));
  text_layer_set_text_alignment(s_temp_text_layer, GTextAlignmentCenter);

  s_temp_layer = text_layer_get_layer(s_temp_text_layer);
  layer_add_child(parent_layer, s_temp_layer);  

  return s_temp_layer;
}

void temp_update() {
  float temp_angle = s_complication_angles[1];

  GRect temp_frame = (GRect) {
    .origin = (GPoint) {
      .x = (int16_t)(sin_lookup(temp_angle) * (int32_t)COMPLICATION_CENTER_RADIUS / TRIG_MAX_RATIO) + g_center.x - COMPLICATION_ORIGIN_OFFSET_X,
      .y = (int16_t)(-cos_lookup(temp_angle) * (int32_t)COMPLICATION_CENTER_RADIUS / TRIG_MAX_RATIO) + g_center.y - COMPLICATION_ORIGIN_OFFSET_Y
    },
    .size = (GSize) {
      .w = COMPLICATION_W,
      .h = COMPLICATION_H
    },
  };
  layer_set_frame(s_temp_layer, temp_frame);
  text_layer_set_text(s_temp_text_layer, g_temp_buffer);

}


