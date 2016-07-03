#include <pebble.h>
#include "main.h"
#include "complications.h"

#define COMPLICATION_MARGIN 20
#define COMPLICATION_CENTER_RADIUS 56
#define COMPLICATION_L_FONT_H 30
#define COMPLICATION_S_FONT_H 16
#define COMPLICATION_ORIGIN_OFFSET_X 36
#define COMPLICATION_ORIGIN_OFFSET_Y 23
#define COMPLICATION_W 73
#define COMPLICATION_H 45

extern GPoint g_center;
extern float s_complication_angles[2];
extern char g_date_d_buffer[4];
extern char g_date_a_buffer[6];
extern char g_temp_now_buffer[8];
extern char g_temp_high_buffer[8];

static GFont s_l_font;
static GFont s_s_font;


static TextLayer *s_date_d_text_layer;
static TextLayer *s_date_a_text_layer;
static Layer *s_date_layer;

static TextLayer *s_temp_now_text_layer;
static TextLayer *s_temp_high_text_layer;
static Layer *s_temp_layer;

static void date_create(Layer *parent_layer) {
  s_date_layer = layer_create(GRect(0, 0, 0, 0));
  layer_add_child(parent_layer, s_date_layer);  

  s_date_d_text_layer = text_layer_create((GRect) {
    .origin = GPoint(0, COMPLICATION_S_FONT_H-1),
    .size = (GSize) {
      .w = COMPLICATION_W,
      .h = COMPLICATION_L_FONT_H
    },
  });
  text_layer_set_background_color(s_date_d_text_layer, GColorClear);
  text_layer_set_text_color(s_date_d_text_layer, GColorWhite);
  text_layer_set_font(s_date_d_text_layer, s_l_font);
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
  text_layer_set_font(s_date_a_text_layer, s_s_font);
  text_layer_set_text_alignment(s_date_a_text_layer, GTextAlignmentCenter);
  layer_add_child(s_date_layer, text_layer_get_layer(s_date_a_text_layer));

}

static void temp_create(Layer *parent_layer) {
  s_temp_layer = layer_create(GRect(0, 0, 0, 0));
  layer_add_child(parent_layer, s_temp_layer);  

  s_temp_now_text_layer = text_layer_create((GRect) {
    .origin = GPoint(0, COMPLICATION_S_FONT_H-1),
    .size = (GSize) {
      .w = COMPLICATION_W,
      .h = COMPLICATION_L_FONT_H
    },
  });
  text_layer_set_background_color(s_temp_now_text_layer, GColorClear);
  text_layer_set_text_color(s_temp_now_text_layer, GColorWhite);
  text_layer_set_font(s_temp_now_text_layer, s_l_font);
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
  text_layer_set_font(s_temp_high_text_layer, s_s_font);
  text_layer_set_text_alignment(s_temp_high_text_layer, GTextAlignmentCenter);
  layer_add_child(s_temp_layer, text_layer_get_layer(s_temp_high_text_layer));  

}

void complications_create(Layer *parent_layer) {
  s_l_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_COMPLICATION_FONT_30));
  s_s_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_COMPLICATION_FONT_16));
  date_create(parent_layer);
  temp_create(parent_layer);

}


void complications_destroy() {
  layer_destroy(text_layer_get_layer(s_date_a_text_layer));
  layer_destroy(text_layer_get_layer(s_date_d_text_layer));
  layer_destroy(s_date_layer);
  layer_destroy(text_layer_get_layer(s_temp_now_text_layer));
  layer_destroy(text_layer_get_layer(s_temp_high_text_layer));
  layer_destroy(s_temp_layer);
  fonts_unload_custom_font(s_s_font);
  fonts_unload_custom_font(s_l_font);

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
