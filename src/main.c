#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer;

// BMO
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;


static void update_time(){
  // Get a tm struct
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // Write current time into buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                       "%H:%M" : "%I:%M", tick_time);
  
  // Write current date to buffer
  static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), "%a %d %b", tick_time);
   
  // Display this time + date on the textlayer
  text_layer_set_text(s_time_layer, s_buffer);
  text_layer_set_text(s_date_layer, date_buffer);
  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  update_time();
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Add background layer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BMO_GRAY);
  s_background_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
  
  
  // Custom font
  static GFont s_time_font;
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_40));
  
  
  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, 10, 144, 50));
  // Date layer
  s_date_layer = text_layer_create(
    GRect(0, 55, 144, 30));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
}


static void main_window_unload(Window *window){
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
}


static void init() {
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  // Register with TickTImerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  update_time();
}


static void deinit(){
  
  window_destroy(s_main_window);
}

int main(){
  // Initialize the app
  init();
  
  // Wait for app events
  app_event_loop();
  
  // Deinitialize the app
  deinit();
  
  return 0;
}