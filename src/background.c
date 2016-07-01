#include <pebble.h>
#include "background.h"

Layer* background_create(Layer *window_layer) {
  Layer *background_layer = layer_create(layer_get_bounds(window_layer));
  layer_set_update_proc(background_layer, background_update_proc);
  layer_add_child(window_layer, background_layer);
  
  return background_layer;
}

void background_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, BACKGROUND_COLOR);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

}