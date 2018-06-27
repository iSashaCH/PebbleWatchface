#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void verticalAlignTextLayer(TextLayer *layer) {
    GRect frame = layer_get_frame(text_layer_get_layer(layer));
    GSize content = text_layer_get_content_size(layer);
    layer_set_frame(text_layer_get_layer(layer),
           GRect(frame.origin.x, frame.origin.y + (frame.size.h - content.h - 5) / 2, 
           frame.size.w, content.h));
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
  text_layer_set_background_color(s_time_layer, GColorBlueMoon);
  text_layer_set_text_color(s_time_layer, GColorWhite);
 
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
   // Vertical alignment
  // verticalAlignTextLayer(s_time_layer);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}


static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
   update_time();
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  window_stack_push(s_main_window, true);
  update_time();

}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
