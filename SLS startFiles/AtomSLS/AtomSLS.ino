#include "M5AtomS3.h"
#include <lvgl.h>
#include <SPI.h>
#include <Wire.h>
#include "ui.h"

// init the tft espi
static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;  // Descriptor of a display driver
static lv_indev_drv_t indev_drv; // Descriptor of a touch driver

#define LV_HOR_RES_MAX 128
#define LV_VER_RES_MAX 128
M5GFX *tft;


void tft_lv_initialization() {
  auto cfg = M5.config();
  AtomS3.begin(cfg);
  AtomS3.Display.setBrightness(9);
  lv_init();
  static lv_color_t buf1[(LV_HOR_RES_MAX * LV_VER_RES_MAX) / 10];  // Declare a buffer for 1/10 screen siz
  static lv_color_t buf2[(LV_HOR_RES_MAX * LV_VER_RES_MAX) / 10];  // second buffer is optionnal
  lv_disp_draw_buf_init(&draw_buf, buf1, buf2, (LV_HOR_RES_MAX * LV_VER_RES_MAX) / 10);
  tft=&AtomS3.Lcd;
}

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  tft->startWrite();
  tft->setAddrWindow(area->x1, area->y1, w, h);
  tft->pushColors((uint16_t *)&color_p->full, w * h, true);
  tft->endWrite();
  lv_disp_flush_ready(disp);
}

void init_disp_driver() {
  lv_disp_drv_init(&disp_drv);  // Basic initialization
  disp_drv.flush_cb = my_disp_flush;  // Set your driver function
  disp_drv.draw_buf = &draw_buf;      // Assign the buffer to the display
  disp_drv.hor_res = LV_HOR_RES_MAX;  // Set the horizontal resolution of the display
  disp_drv.ver_res = LV_VER_RES_MAX;  // Set the vertical resolution of the display
  lv_disp_drv_register(&disp_drv);                   // Finally register the driver
  lv_disp_set_bg_color(NULL, lv_color_hex3(0x000));  // Set default background color to black
}

void setup()
{
  Serial.begin(115200);
  tft_lv_initialization();
  init_disp_driver();
  ui_init();
}

void loop()
{
  AtomS3.update();
    auto imu_update = M5.Imu.update();
    if (imu_update) {
      auto data = M5.Imu.getImuData();
      lv_label_set_text(ui_Label1,String(data.accel.x).c_str());
    }
  lv_task_handler();
  delay(5);
}

