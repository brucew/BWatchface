#pragma once

#define BACKGROUND_COLOR GColorBlue

Layer* background_create(Layer *window_layer);
void background_update_proc(Layer *layer, GContext *ctx);
