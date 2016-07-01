#include "main.h"
#include "complications.h"

TextLayer *s_date_text_layer;
GRect s_date_frame;

TextLayer *s_temp_text_layer;
GRect s_temp_frame;

Layer* date_create(Layer *parent_layer) {
  GRect s_date_frame = GRect(s_center.x + COMPLICATION_MARGIN, s_center.y + COMPLICATION_MARGIN, s_center.x - COMPLICATION_MARGIN, s_center.y - COMPLICATION_MARGIN);
  s_date_text_layer = text_layer_create(s_date_frame);
  text_layer_set_background_color(s_date_text_layer, GColorClear);
  text_layer_set_text_color(s_date_text_layer, GColorWhite);
  text_layer_set_font(s_date_text_layer, fonts_get_system_font(COMPLICATION_FONT));
  text_layer_set_text_alignment(s_date_text_layer, GTextAlignmentCenter);
  layer_add_child(parent_layer, text_layer_get_layer(s_date_text_layer));  
  
  return text_layer_get_layer(s_date_text_layer);
}

void date_update_proc() {
  float date_angle = s_complication_angles[0];

  GRect date_frame = (GRect) {
    .origin = (GPoint) {
      .x = (int16_t)(sin_lookup(date_angle) * (int32_t)COMPLICATION_CENTER_RADIUS / TRIG_MAX_RATIO) + s_center.x - COMPLICATION_ORIGIN_OFFSET_X,
      .y = (int16_t)(-cos_lookup(date_angle) * (int32_t)COMPLICATION_CENTER_RADIUS / TRIG_MAX_RATIO) + s_center.y - COMPLICATION_ORIGIN_OFFSET_Y
    },
    .size = (GSize) {
      .w = COMPLICATION_W,
      .h = COMPLICATION_H
    },
  };
  layer_set_frame(text_layer_get_layer(s_date_text_layer), date_frame);
  text_layer_set_text(s_date_text_layer, s_date_buffer);

}

Layer* temp_create(Layer *parent_layer) {
  GRect s_temp_frame = GRect(s_center.x + COMPLICATION_MARGIN, s_center.y + COMPLICATION_MARGIN, s_center.x - COMPLICATION_MARGIN, s_center.y - COMPLICATION_MARGIN);
  s_temp_text_layer = text_layer_create(s_temp_frame);
  text_layer_set_background_color(s_temp_text_layer, GColorClear);
  text_layer_set_text_color(s_temp_text_layer, GColorWhite);
  text_layer_set_font(s_temp_text_layer, fonts_get_system_font(COMPLICATION_FONT));
  text_layer_set_text_alignment(s_temp_text_layer, GTextAlignmentCenter);
  layer_add_child(parent_layer, text_layer_get_layer(s_temp_text_layer));  

  return text_layer_get_layer(s_temp_text_layer);
}

void temp_update_proc() {
  float temp_angle = s_complication_angles[1];

  GRect temp_frame = (GRect) {
    .origin = (GPoint) {
      .x = (int16_t)(sin_lookup(temp_angle) * (int32_t)COMPLICATION_CENTER_RADIUS / TRIG_MAX_RATIO) + s_center.x - COMPLICATION_ORIGIN_OFFSET_X,
      .y = (int16_t)(-cos_lookup(temp_angle) * (int32_t)COMPLICATION_CENTER_RADIUS / TRIG_MAX_RATIO) + s_center.y - COMPLICATION_ORIGIN_OFFSET_Y
    },
    .size = (GSize) {
      .w = COMPLICATION_W,
      .h = COMPLICATION_H
    },
  };
  layer_set_frame(text_layer_get_layer(s_temp_text_layer), temp_frame);
  text_layer_set_text(s_temp_text_layer, s_temp_buffer);

}

