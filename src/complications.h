#pragma once
#include <pebble.h>

void complications_create(Layer *parent_layer);
void complications_destroy();
void complications_update_frame();
void date_update();
void temp_update();
