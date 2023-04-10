/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"

#include <SysTools.h>

#if CONFIG_EXAMPLE_LCD_CONTROLLER_ILI9341
#include "esp_lcd_ili9341.h"
#elif CONFIG_EXAMPLE_LCD_CONTROLLER_GC9A01
#include "esp_lcd_gc9a01.h"
#elif CONFIG_EXAMPLE_LCD_CONTROLLER_ST7789
#include "esp_lcd_st7789.h"
#endif

#if CONFIG_EXAMPLE_LCD_TOUCH_CONTROLLER_STMPE610
#include "esp_lcd_touch_stmpe610.h"
#endif

static const char *TAG = "example";

// Using SPI2 in the example
#define LCD_HOST  SPI2_HOST

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ     (20 * 1000 * 1000)
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL  1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_SCLK           18
#define EXAMPLE_PIN_NUM_MOSI           19
#define EXAMPLE_PIN_NUM_MISO           21
#define EXAMPLE_PIN_NUM_LCD_DC         5
#define EXAMPLE_PIN_NUM_LCD_RST        3
#define EXAMPLE_PIN_NUM_LCD_CS         4
#define EXAMPLE_PIN_NUM_BK_LIGHT       GPIO_NUM_2
#define EXAMPLE_PIN_NUM_TOUCH_CS       15

// The pixel number in horizontal and vertical
#define EXAMPLE_LCD_H_RES              240
#define EXAMPLE_LCD_V_RES              240

#if CONFIG_EXAMPLE_LCD_CONTROLLER_ILI9341
#define EXAMPLE_LCD_H_RES              240
#define EXAMPLE_LCD_V_RES              320
#elif CONFIG_EXAMPLE_LCD_CONTROLLER_GC9A01
#define EXAMPLE_LCD_H_RES              240
#define EXAMPLE_LCD_V_RES              240
#elif CONFIG_EXAMPLE_LCD_CONTROLLER_ST7789
#define EXAMPLE_LCD_H_RES              800
#define EXAMPLE_LCD_V_RES              480
#endif
// Bit number used to represent command and parameter
#define EXAMPLE_LCD_CMD_BITS           8
#define EXAMPLE_LCD_PARAM_BITS         8
#define EXAMPLE_LVGL_TICK_PERIOD_MS    2

#if CONFIG_EXAMPLE_LCD_TOUCH_ENABLED
esp_lcd_touch_handle_t tp = NULL;
#endif

// LVGL image declare
//LV_IMG_DECLARE ( esp_logo )
//LV_IMG_DECLARE ( esp_text )

typedef  struct  {
    lv_obj_t  * scr ;
    int  count_val ;
}  my_timer_context_t ;

//extern void example_lvgl_demo_ui(lv_disp_t *disp);
static  lv_obj_t  * arc [ 3 ];
static  lv_obj_t  * img_logo ;
static  lv_obj_t  * img_text ;
static  lv_color_t  arc_color []  =  {
    LV_COLOR_MAKE ( 232 ,  87 ,  116 ),
    LV_COLOR_MAKE ( 126 ,  87 ,  162 ),
    LV_COLOR_MAKE ( 90 ,  202 ,  228 ),
};

static bool example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    SysTools::addLog("lcdDisplayTest::Setup, example_notify_lvgl_flush_ready");

    lv_disp_drv_t *disp_driver = (lv_disp_drv_t *)user_ctx;
    lv_disp_flush_ready(disp_driver);
    return false;
}

static void example_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    // copy a buffer's content to a specific area of the display
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}

/* Rotate display and touch, when rotated screen in LVGL. Called when driver parameters are updated. */
static void example_lvgl_port_update_callback(lv_disp_drv_t *drv)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;

    switch (drv->rotated) {
    case LV_DISP_ROT_NONE:
        // Rotate LCD display
        esp_lcd_panel_swap_xy(panel_handle, false);
        esp_lcd_panel_mirror(panel_handle, true, false);
#if CONFIG_EXAMPLE_LCD_TOUCH_ENABLED
        // Rotate LCD touch
        esp_lcd_touch_set_mirror_y(tp, false);
        esp_lcd_touch_set_mirror_x(tp, false);
#endif
        break;
    case LV_DISP_ROT_90:
        // Rotate LCD display
        esp_lcd_panel_swap_xy(panel_handle, true);
        esp_lcd_panel_mirror(panel_handle, true, true);
#if CONFIG_EXAMPLE_LCD_TOUCH_ENABLED
        // Rotate LCD touch
        esp_lcd_touch_set_mirror_y(tp, false);
        esp_lcd_touch_set_mirror_x(tp, false);
#endif
        break;
    case LV_DISP_ROT_180:
        // Rotate LCD display
        esp_lcd_panel_swap_xy(panel_handle, false);
        esp_lcd_panel_mirror(panel_handle, false, true);
#if CONFIG_EXAMPLE_LCD_TOUCH_ENABLED
        // Rotate LCD touch
        esp_lcd_touch_set_mirror_y(tp, false);
        esp_lcd_touch_set_mirror_x(tp, false);
#endif
        break;
    case LV_DISP_ROT_270:
        // Rotate LCD display
        esp_lcd_panel_swap_xy(panel_handle, true);
        esp_lcd_panel_mirror(panel_handle, false, false);
#if CONFIG_EXAMPLE_LCD_TOUCH_ENABLED
        // Rotate LCD touch
        esp_lcd_touch_set_mirror_y(tp, false);
        esp_lcd_touch_set_mirror_x(tp, false);
#endif
        break;
    }
}

#if CONFIG_EXAMPLE_LCD_TOUCH_ENABLED
static void example_lvgl_touch_cb(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    uint16_t touchpad_x[1] = {0};
    uint16_t touchpad_y[1] = {0};
    uint8_t touchpad_cnt = 0;

    /* Read touch controller data */
    esp_lcd_touch_read_data(drv->user_data);

    /* Get coordinates */
    bool touchpad_pressed = esp_lcd_touch_get_coordinates(drv->user_data, touchpad_x, touchpad_y, NULL, &touchpad_cnt, 1);

    if (touchpad_pressed && touchpad_cnt > 0) {
        data->point.x = touchpad_x[0];
        data->point.y = touchpad_y[0];
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}
#endif

static void example_increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}
static  void  anim_timer_cb ( lv_timer_t  * timer )
{
    my_timer_context_t  * timer_ctx  =  ( my_timer_context_t  * )  timer -> user_data ;
    int  count  =  timer_ctx -> count_val ;
    lv_obj_t  * scr  =  timer_ctx -> scr ;

    // Play arc animation
    if  ( count  <  90 )  {
        lv_coord_t  arc_start  =  count  >  0  ?  ( 1  -  cosf ( count  /  180  *  PI ))  *  270  :  0 ;
        lv_coord_t  arc_len  =  ( sinf ( count  /  180 *  PI )  +  1 )  *  135 ;

        for  ( size_t  i  =  0 ;  i  <  sizeof ( arc )  /  sizeof ( arc [ 0 ]);  i ++ )  {
            lv_arc_set_bg_angles ( arc [ i ],  arc_start ,  arc_len );
            lv_arc_set_rotation ( arc [ i ],  ( count  +  120  *  ( i  +  1 ))  %  360 );
        }
    }

    // Delete arcs when animation finished
    if  ( count  ==  90 )  {
        for  ( size_t  i  =  0 ;  i  <  sizeof ( arc )  /  sizeof ( arc [ 0 ]);  i ++ )  {
            lv_obj_del ( arc [ i ]);
        }

        //Create new image and make it transparent
        img_text  =  lv_img_create ( scr );
        //lv_img_set_src ( img_text ,  & esp_text );
        lv_obj_set_style_img_opa ( img_text ,  0 ,  0 );
    }

    // Move images when arc animation finished
    if  (( count  >=  100 )  &&  ( count  <=  180 ))  {
        lv_coord_t  offset  =  ( sinf (( count  -  140 )  *  2.25  /  90 )  +  1 )  *  20;
        lv_obj_align ( img_logo ,  LV_ALIGN_CENTER ,  0 ,  - offset );
        lv_obj_align ( img_text ,  LV_ALIGN_CENTER ,  0 ,  2  *  offset );
        lv_obj_set_style_img_opa ( img_text ,  offset  /  40  *  255 ,  0 );
    }

    // Delete timer when all animation finished
    if  (( count  +=  5 )  ==  220 )  {
        lv_timer_del ( timer );
    }  else  {
        timer_ctx->count_val  =  count ;
    }
}

void  example_lvgl_demo_ui ( lv_obj_t  * scr )
{
    //Create image
    img_logo  =  lv_img_create ( scr );
    //lv_img_set_src ( img_logo ,  & esp_logo );
    lv_obj_center ( img_logo );

    // Create arcs
    for  ( size_t  i  =  0 ;  i  <  sizeof ( arc )  /  sizeof ( arc [ 0 ]);  i ++ )  {
        arc [ i ]  =  lv_arc_create ( scr );

        // Set arc caption
        lv_obj_set_size ( arc [ i ],  220  -  30  *  i ,  220  -  30  *  i );
        lv_arc_set_bg_angles ( arc [ i ],  120  *  i ,  10  +  120  *  i );
        lv_arc_set_value ( arc [ i ],  0 );

        // Set arc style
        lv_obj_remove_style ( arc [ i ],  NULL ,  LV_PART_KNOB );
        lv_obj_set_style_arc_width ( arc [ i ],  10 ,  0 );
        lv_obj_set_style_arc_color ( arc [ i ],  arc_color [ i ],  0 );

        // Make arc center
        lv_obj_center ( arc [ i ]);
    }

    //Create timer for animation
    static  my_timer_context_t  my_tim_ctx  =  {
        .count_val = -90,
    };
    my_tim_ctx.scr = scr;
    lv_timer_create ( anim_timer_cb ,  20 ,  & my_tim_ctx );
}

/////////////////////////////////////////////////////////////////////////
// Simple routine to generate some patterns and send them to the LCD. Because the
// SPI driver handles transactions in the background, we can calculate the next line
// while the previous one is being sent.
//static uint16_t rgblines[2] = {0,10,20,40};

//]60,80,100,120,140,180,200,210,220,240,260,280,300,320,340,380,400
//  }, {
//    100,1010,1020,1040,1060,1080,10100,10120,10140,10180,10200,10210,10220,10240,10260,10280,10300,10320,10340,10380,10400
//  }
//];

#define ROTATE_FRAME 60

static uint8_t* s_line;

static void display_pretty_colors(esp_lcd_panel_handle_t panel_handle) {
  SysTools::addLog("lcdDisplayTest::display_pretty_colors");
  //typedef unsigned char u24_t[3];
  s_line = (uint8_t*)malloc(200000);
  //s_lines[1] = (uint8_t*)malloc(256 * 3);
  int frame = 0;
  int sending_line = 0;
  int calc_line = 0;
    
    memset(s_line, 0xE0, 200000);
    //memset(s_line, 0x00, 200000);
    //memset(s_lines[1], 0x11, 240*3);

    SysTools::addLog("lcdDisplayTest::display_pretty_colors, esp_lcd_panel_draw_bitmap, [0]: 0x%X", s_line[0]);
    SysTools::addLog("lcdDisplayTest::display_pretty_colors, esp_lcd_panel_draw_bitmap: %u", esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, 241, 241, &s_line));
    return;
/*
    SysTools::addLog("lcdDisplayTest::display_pretty_colors, s_lines[0]: 0%X, 0%X, 0%X, s_lines[1]: 0%X, 0%X, 0%X", s_lines[0][0], s_lines[0][1], s_lines[0][2], s_lines[1][0], s_lines[1][1], s_lines[1][2]);

    // After ROTATE_FRAME frames, the image will be rotated
    while (frame <= ROTATE_FRAME) {
        frame++;
        for (int y = 0; y < 240; y += 240) {
            // Calculate a line
            //pretty_effect_calc_lines(s_lines[calc_line], y, frame, 240);
            sending_line = calc_line;
            calc_line = !calc_line;
            // Send the calculated data
            SysTools::addLog("lcdDisplayTest::display_pretty_colors, esp_lcd_panel_draw_bitmap, line: %u, [0]: 0x%X", sending_line, s_lines[0][0]);
            SysTools::addLog("lcdDisplayTest::display_pretty_colors, esp_lcd_panel_draw_bitmap: %u", esp_lcd_panel_draw_bitmap(panel_handle, 0, y, 0 + 240, y + 240, &s_lines[0][0]));
        }
    }
 */
}

///////////////////////////////////////////////////////////////////////////
void setup() {
    Serial.begin(115200);
    delay(4000);
    
    SysTools::addLog("%s", "lcdDisplayTest::Setup");

    static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv;      // contains callback functions
/*
    ESP_LOGI(TAG, "Turn off LCD backlight");
    gpio_config_t bk_gpio_config = {
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT,
        .mode = GPIO_MODE_OUTPUT,
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
*/
    SysTools::addLog("%s", "lcdDisplayTest::Setup, Init SPI Bus");
    spi_bus_config_t buscfg = {
      .mosi_io_num = 7,
      .miso_io_num = -1,
      .sclk_io_num = 6,
      .quadwp_io_num = -1, // Quad SPI LCD driver is not yet supported
      .quadhd_io_num = -1, // Quad SPI LCD driver is not yet supported
      .max_transfer_sz = 240 * 80 * sizeof(uint16_t), // transfer 80 lines of pixels (assume pixel is RGB565) at most in one SPI transaction
    };
    
    SysTools::addLog("lcdDisplayTest::Setup spi_bus_initialize: %i", spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO)); // Enable the DMA feature

    //ESP_LOGI(TAG, "Install panel IO");
    SysTools::addLog("%s", "lcdDisplayTest::Setup, esp_lcd_new_panel_io_spi");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = 5,
        .dc_gpio_num = 4,
        .spi_mode = 0,
        .pclk_hz = 20000000,
        .trans_queue_depth = 10,
        //.on_color_trans_done = example_notify_lvgl_flush_ready,
        //.user_ctx = &disp_drv,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    // Attach the LCD to the SPI bus
    SysTools::addLog("lcdDisplayTest::Setup, esp_lcd_new_panel_io_spi: %i", esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = 8,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };
    
    SysTools::addLog("%s", "lcdDisplayTest::Setup, Config panel IO");
    SysTools::addLog("lcdDisplayTest::Setup, esp_lcd_new_panel_st7789: %i", esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    
    if(panel_handle == NULL) {
        SysTools::addLog("lcdDisplayTest::Setup, ABORT: esp_lcd_new_panel_st7789 failed");
        return;
    }

    SysTools::addLog("lcdDisplayTest::Setup, gpio_set_level: %i", gpio_set_level(GPIO_NUM_9, 1));

    // Reset the display
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));

    // Initialize LCD panel
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
   
    // Turn on the screen
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    // Swap x and y axis (Different LCD screens may need different options)
    //ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, true));

    display_pretty_colors(panel_handle);

    return;
///////////////////////////////////////////////////////////////////////////////

    SysTools::addLog("lcdDisplayTest::Setup, Initialize LVGL library");
    lv_init();
    // alloc draw buffers used by LVGL
    // it's recommended to choose the size of the draw buffer(s) to be at least 1/10 screen sized
    lv_color_t *buf1 = (lv_color_t*)heap_caps_malloc(EXAMPLE_LCD_H_RES * 20 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1);
    lv_color_t *buf2 = (lv_color_t*)heap_caps_malloc(EXAMPLE_LCD_H_RES * 20 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2);
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, EXAMPLE_LCD_H_RES * 20);

    SysTools::addLog("lcdDisplayTest::Setup, Register display driver to LVGL");
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 240;
    disp_drv.ver_res = 240;
    disp_drv.flush_cb = example_lvgl_flush_cb;
    disp_drv.drv_update_cb = example_lvgl_port_update_callback;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle;
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

    SysTools::addLog("lcdDisplayTest::Setup, Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &example_increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));

#if CONFIG_EXAMPLE_LCD_TOUCH_ENABLED
    static lv_indev_drv_t indev_drv;    // Input device driver (Touch)
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.disp = disp;
    indev_drv.read_cb = example_lvgl_touch_cb;
    indev_drv.user_data = tp;

    lv_indev_drv_register(&indev_drv);
#endif

    ESP_LOGI(TAG, "Display LVGL Meter Widget");
    SysTools::addLog("lcdDisplayTest::Setup, example_lvgl_demo_ui");
    lv_obj_t *scr = lv_disp_get_scr_act(disp);
    example_lvgl_demo_ui(scr);

    while(1) {
      SysTools::addLog("lcdDisplayTest::Setup, loop1");    
      // raise the task priority of LVGL and/or reduce the handler period can improve the performance
      vTaskDelay(pdMS_TO_TICKS(10));
        
      SysTools::addLog("lcdDisplayTest::Setup, loop2");
      // The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
      lv_timer_handler();      
    }
}

void loop() {
}
