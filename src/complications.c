#include <pebble.h>
#include "main.h"
#include "complications.h"

#define COMPLICATION_MARGIN 20
#define COMPLICATION_CENTER_RADIUS 60
#define COMPLICATION_L_FONT FONT_KEY_BITHAM_34_MEDIUM_NUMBERS
#define COMPLICATION_L_FONT_H 34
#define COMPLICATION_S_FONT FONT_KEY_GOTHIC_24_BOLD  
#define COMPLICATION_S_FONT_H 24
#define COMPLICATION_ORIGIN_OFFSET_X 36
#define COMPLICATION_ORIGIN_OFFSET_Y 28
#define COMPLICATION_W 73
#define COMPLICATION_H 58

extern GPoint g_center;
extern float s_complication_angles[2];
extern char g_date_d_buffer[4];
extern char g_date_a_buffer[6];
extern char g_temp_now_buffer[8];
extern char g_temp_high_buffer[8];

static TextLayer *s_date_d_text_layer;
static TextLayer *s_date_a_text_layer;
static Layer *s_date_layer;

static TextLayer *s_temp_now_text_layer;
static TextLayer *s_temp_high_text_layer;
static Layer *s_temp_layer;


Layer* date_create(Layer *parent_layer) {
  s_date_layer = layer_create(GRect(0, 0, 0, 0));
  layer_add_child(parent_layer, s_date_layer);  

  s_date_d_text_layer = text_layer_create((GRect) {
    .origin = GPoint(0, COMPLICATION_S_FONT_H),
    .size = (GSize) {
      .w = COMPLICATION_W,
      .h = COMPLICATION_L_FONT_H
    },
  });
  text_layer_set_background_color(s_date_d_text_layer, GColorClear);
  text_layer_set_text_color(s_date_d_text_layer, GColorWhite);
  text_layer_set_font(s_date_d_text_layer, fonts_get_system_font(COMPLICATION_L_FONT));
  text_layer_set_text_alignment(s_date_d_text_layer, GTextAlignmentCenter);
  layer_add_child(s_date_layer, text_layer_get_layer(s_date_d_text_layer));

  s_date_a_text_layer = text_layer_create((GRect) {
    .origin = GPoint(0, 0),
    .size = (GSize) {
      .w = COMPLICATION_W,
      .h = COMPLICATION_S_FONT_H
    },
  });
  text_layer_set_background_color(s_date_a_text_layer, GColorClear);
  text_layer_set_text_color(s_date_a_text_layer, GColorWhite);
  text_layer_set_font(s_date_a_text_layer, fonts_get_system_font(COMPLICATION_S_FONT));
  text_layer_set_text_alignment(s_date_a_text_layer, GTextAlignmentCenter);
  layer_add_child(s_date_layer, text_layer_get_layer(s_date_a_text_layer));

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
  text_layer_set_text(s_date_d_text_layer, g_date_d_buffer);
  text_layer_set_text(s_date_a_text_layer, g_date_a_buffer);

}

Layer* temp_create(Layer *parent_layer) {
  s_temp_layer = layer_create(GRect(0, 0, 0, 0));
  layer_add_child(parent_layer, s_temp_layer);  

  s_temp_now_text_layer = text_layer_create((GRect) {
    .origin = GPoint(0, COMPLICATION_S_FONT_H),
    .size = (GSize) {
      .w = COMPLICATION_W,
      .h = COMPLICATION_L_FONT_H
    },
  });
  text_layer_set_background_color(s_temp_now_text_layer, GColorClear);
  text_layer_set_text_color(s_temp_now_text_layer, GColorWhite);
  text_layer_set_font(s_temp_now_text_layer, fonts_get_system_font(COMPLICATION_L_FONT));
  text_layer_set_text_alignment(s_temp_now_text_layer, GTextAlignmentCenter);
  layer_add_child(s_temp_layer, text_layer_get_layer(s_temp_now_text_layer));  

  s_temp_high_text_layer = text_layer_create((GRect) {
    .origin = GPoint(0, 0),
    .size = (GSize) {
      .w = COMPLICATION_W,
      .h = COMPLICATION_S_FONT_H
    },
  });
  text_layer_set_background_color(s_temp_high_text_layer, GColorClear);
  text_layer_set_text_color(s_temp_high_text_layer, GColorWhite);
  text_layer_set_font(s_temp_high_text_layer, fonts_get_system_font(COMPLICATION_S_FONT));
  text_layer_set_text_alignment(s_temp_high_text_layer, GTextAlignmentCenter);
  layer_add_child(s_temp_layer, text_layer_get_layer(s_temp_high_text_layer));  

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
  text_layer_set_text(s_temp_now_text_layer, g_temp_now_buffer);
  text_layer_set_text(s_temp_high_text_layer, g_temp_high_buffer);


}


