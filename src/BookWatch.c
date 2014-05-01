#include <pebble.h>

static Window *window;
static AppTimer *timer;

#define ANIMATION_INTERVAL 50
#define DISPLAY_BOTTOM 167
#define DISPLAY_WIDTH 145

#define PAGE_COUNT 10

static char time_text[] = "00:00";
static int current_page = 0;
static bool current_color = true, page_rolling = true;
static GPath *page_path_ptr[PAGE_COUNT];
static const GPathInfo PAGE_PATH_INFO[PAGE_COUNT] = {
{
.num_points = 3,
.points = (GPoint []) {{DISPLAY_WIDTH - 10,     DISPLAY_BOTTOM}, {DISPLAY_WIDTH, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, DISPLAY_BOTTOM - 10}}
},
{
.num_points = 4,
.points = (GPoint []) {{DISPLAY_WIDTH - 30,     DISPLAY_BOTTOM - 15}, {DISPLAY_WIDTH - 15, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, DISPLAY_BOTTOM - 30}}
},
{
.num_points = 4,
.points = (GPoint []) {{DISPLAY_WIDTH - 60,     DISPLAY_BOTTOM - 30}, {DISPLAY_WIDTH - 30, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, DISPLAY_BOTTOM - 60}}
},
{
.num_points = 4,
.points = (GPoint []) {{DISPLAY_WIDTH - 70,    DISPLAY_BOTTOM - 45}, {DISPLAY_WIDTH - 40, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, DISPLAY_BOTTOM - 90}}
},
{
.num_points = 4,
.points = (GPoint []) {{DISPLAY_WIDTH - 90,    DISPLAY_BOTTOM - 45}, {DISPLAY_WIDTH - 50, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, DISPLAY_BOTTOM - 120}}
},
{
.num_points = 5,
.points = (GPoint []) {{DISPLAY_WIDTH - 110,   DISPLAY_BOTTOM - 40}, {DISPLAY_WIDTH - 65, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, 0}, {DISPLAY_WIDTH - 14, DISPLAY_BOTTOM - 140}}
},
{
.num_points = 6,
.points = (GPoint []) {{30,                      DISPLAY_BOTTOM - 40}, {DISPLAY_WIDTH - 80, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, 0}, {DISPLAY_WIDTH, 0}, {DISPLAY_WIDTH - 30, 0}}
},
{
.num_points = 6,
.points = (GPoint []) {{5,                      DISPLAY_BOTTOM - 20}, {DISPLAY_WIDTH - 120, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, 0}, {DISPLAY_WIDTH, 0}, {DISPLAY_WIDTH - 70, 0}}
},
{
.num_points = 5,
.points = (GPoint []) {{0,                      DISPLAY_BOTTOM - 10}, {0, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, 0}, {DISPLAY_WIDTH - 120, 0}}
},
{
.num_points = 4,
.points = (GPoint []) {{0,0}, {0, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, DISPLAY_BOTTOM}, {DISPLAY_WIDTH, 0}}
}
};

void handle_time_tick(struct tm* tick_time, TimeUnits units_changed) {
    strftime(time_text, sizeof(time_text), "%T", tick_time);
    page_rolling = true;
}

void layer_update_callback(Layer *me, GContext* ctx) {
    graphics_context_set_fill_color(ctx, current_color ? GColorBlack : GColorWhite );

    if(current_page < PAGE_COUNT) {
        gpath_move_to(page_path_ptr[current_page], (GPoint){-1,0});
        gpath_draw_filled(ctx, page_path_ptr[current_page]);

        if(current_page > 5) {
            //Draw text
            graphics_context_set_text_color(ctx, current_color ? GColorWhite : GColorBlack );
            graphics_draw_text(
                ctx,
                time_text,
                fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BRANKOVIC_40)),
                GRect(0,60,DISPLAY_WIDTH, 60),
                GTextOverflowModeWordWrap,
                GTextAlignmentCenter,
                NULL
            );
        }
    }

    current_page++;
    if(current_page > PAGE_COUNT) {
        current_page = 0;
        current_color = !current_color;
        page_rolling = false;
    }
}

void timer_callback(void *context) {
    if(page_rolling) {
        Layer *window_layer = window_get_root_layer(window);
        layer_mark_dirty(window_layer);
    }
    const uint32_t timeout_ms = ANIMATION_INTERVAL;
    timer = app_timer_register(timeout_ms, timer_callback, NULL);
}

//Clicks are for debugging purposes - Don`t forget to set `"watchface":false` in `appinfo.json` file
void page_turn_handler(ClickRecognizerRef recognizer, void *context) {
    if(!page_rolling) page_rolling = true;
}
void click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, page_turn_handler);
}

void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    //Prepare pages
    for(int i=0;i<PAGE_COUNT;i++) {
        page_path_ptr[i] = gpath_create(&PAGE_PATH_INFO[i]);
    }

    //Draw call
    layer_set_update_proc(window_layer, layer_update_callback);

    //First roll
    const uint32_t timeout_ms = ANIMATION_INTERVAL;
    timer = app_timer_register(timeout_ms, timer_callback, NULL);

    //Time fetch
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    handle_time_tick(current_time, MINUTE_UNIT);
    tick_timer_service_subscribe(MINUTE_UNIT, &handle_time_tick);
}

void window_unload(Window *window) {
}

void init(void) {
    window = window_create();
    window_set_click_config_provider(window, click_config_provider);
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    const bool animated = true;
    window_stack_push(window, animated);
}

void deinit(void) {
    window_destroy(window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
