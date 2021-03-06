#include <pebble.h>
#include "messages.h"
#include "complications.h"

extern char g_temp_now_buffer[8];
extern char g_temp_high_buffer[8];
extern char g_temp_low_buffer[8];

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATURE);
  Tuple *high_tuple = dict_find(iterator, MESSAGE_KEY_HIGH);
  Tuple *low_tuple = dict_find(iterator, MESSAGE_KEY_LOW);
//   Tuple *conditions_tuple = dict_find(iterator, MESSAGE_KEY_CONDITIONS);

  // If temp data is available, use it
  if(temp_tuple) {
    snprintf(g_temp_now_buffer, sizeof(g_temp_now_buffer), "%d°", (int)temp_tuple->value->int32);
  }
  
  // If high data is available, use it
  if(high_tuple) {
    snprintf(g_temp_high_buffer, sizeof(g_temp_high_buffer), "%d°", (int)high_tuple->value->int32);
  }
  
  // If low data is available, use it
  if(low_tuple) {
    snprintf(g_temp_low_buffer, sizeof(g_temp_low_buffer), "%d°", (int)low_tuple->value->int32);
  }
  
  temp_update();
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void messages_register() {
    // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

}

void weather_update() {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
  
    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);
  
    // Send the message!
    app_message_outbox_send();
}
