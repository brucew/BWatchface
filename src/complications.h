#pragma once
#include <pebble.h>

Layer* date_create(Layer *parent_layer);
Layer* temp_create(Layer *parent_layer);
void date_update();
void temp_update();