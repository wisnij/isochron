#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID { 0x1A, 0xE8, 0x64, 0x40, 0xC5, 0x43, 0x47, 0xC1, 0x80, 0x62, 0x22, 0x70, 0xD0, 0x74, 0xEF, 0x37 }
#define MY_WATCHNAME "Isochron"
#define PEBBLE_WIDTH  144
#define PEBBLE_HEIGHT 168

PBL_APP_INFO( MY_UUID, MY_WATCHNAME,
              "Jim Wisniewski",
              1, 0,
              DEFAULT_MENU_ICON,
              APP_INFO_WATCH_FACE );


Window window;

BmpContainer background_image;

TextLayer text_time_layer;
TextLayer text_date_layer;
TextLayer text_iso_layer;


void update_display( PblTm* current_time )
{
    static char time_text[] = "HH:MM";
    static char date_text[] = "AAAAAAAAA\nbbb dd, YYYY";
    static char iso_text[]  = "YYYY-mm-dd\nYYYY-WVV-u\nYYYY-jjj";
    
    string_format_time( time_text, sizeof time_text, "%H:%M", current_time );
    string_format_time( date_text, sizeof date_text, "%A\n%b %d, %Y", current_time );
    string_format_time( iso_text,  sizeof iso_text,  "%Y-%m-%d\n%Y-W%V-%u\n%Y-%j", current_time );
    
    text_layer_set_text( &text_time_layer, time_text );
    text_layer_set_text( &text_date_layer, date_text );
    text_layer_set_text( &text_iso_layer,  iso_text );
}


void handle_init( AppContextRef ctx )
{
    (void) ctx;
    
    window_init( &window, MY_WATCHNAME );
    window_stack_push( &window, true );
    window_set_background_color( &window, GColorBlack );
    
    resource_init_current_app( &APP_RESOURCES );

    // background
    bmp_init_container( RESOURCE_ID_IMAGE_BACKGROUND, &background_image );
    layer_add_child( &window.layer, &background_image.layer.layer );
    
    // day and date text
    text_layer_init( &text_date_layer, GRect( 0, 30, PEBBLE_WIDTH, 31 ) );
    text_layer_set_text_color( &text_date_layer, GColorBlack );
    text_layer_set_background_color( &text_date_layer, GColorClear );
    text_layer_set_font( &text_date_layer, fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_LIBERATION_SANS_BOLD_14 ) ) );
    text_layer_set_text_alignment( &text_date_layer, GTextAlignmentCenter );
    layer_add_child( &window.layer, &text_date_layer.layer );
    
    // main time display
    text_layer_init( &text_time_layer, GRect( 0, 56, PEBBLE_WIDTH, 50 ) );
    text_layer_set_text_color( &text_time_layer, GColorBlack );
    text_layer_set_background_color( &text_time_layer, GColorClear );
    text_layer_set_font( &text_time_layer, fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_LIBERATION_SANS_BOLD_48 ) ) );
    text_layer_set_text_alignment( &text_time_layer, GTextAlignmentCenter );
    layer_add_child( &window.layer, &text_time_layer.layer );
    
    // year-month-day, ISO week number, and ordinal date
    text_layer_init( &text_iso_layer, GRect( 0, 110, PEBBLE_WIDTH, 45 ) );
    text_layer_set_text_color( &text_iso_layer, GColorBlack );
    text_layer_set_background_color( &text_iso_layer, GColorClear );
    text_layer_set_font( &text_iso_layer, fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_LIBERATION_MONO_14 ) ) );
    text_layer_set_text_alignment( &text_iso_layer, GTextAlignmentCenter );
    layer_add_child( &window.layer, &text_iso_layer.layer );
    
    // update once at startup
    PblTm tick_time;
    get_time( &tick_time );
    update_display( &tick_time );
}


void handle_deinit( AppContextRef ctx )
{
    (void) ctx;
    
    bmp_deinit_container( &background_image );
}


void handle_minute_tick( AppContextRef ctx, PebbleTickEvent* t )
{
    (void) ctx;
    
    update_display( t->tick_time );
}


void pbl_main( void* params )
{
    PebbleAppHandlers handlers = {
        .init_handler = &handle_init,
        .deinit_handler = &handle_deinit,
        .tick_info = {
            .tick_handler = &handle_minute_tick,
            .tick_units = MINUTE_UNIT
        }
    };
    
    app_event_loop( params, &handlers );
}
