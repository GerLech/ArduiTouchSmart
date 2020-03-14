#ifndef TFTWidgetColorSlider_h
#define TFTWidgetColorSlider_h

#include <Arduino.h>
#include <TFTWidgets.h>


class TFTWidgetColorSlider: public Widget {
protected:
  int16_t _min = 0;
  int16_t _max = 255;
  uint16_t _red = 0;
  uint16_t _green = 0;
  uint16_t _blue = 0;
  void drawValue(Adafruit_ILI9341 * tft);
  void drawSlider(Adafruit_ILI9341 * tft, uint8_t val,uint16_t y, uint16_t color);
  void deleteOld(Adafruit_ILI9341 * tft);
public:
  TFTWidgetColorSlider(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char name[], const char properties[], uint16_t yOffset, const GFXfont * font);
  TFTWidgetColorSlider(const char properties[], uint16_t yOffset, const GFXfont * font);
  void init(const char properties[]);
  void draw(Adafruit_ILI9341 * tft);
  void update(Adafruit_ILI9341 * tft, const char data[], bool display);
  bool touchEvent(Adafruit_ILI9341 * tft, int16_t x, int16_t y, EV event);
  int16_t getValue();
  void setStatus(uint8_t status);
  bool toPublish();
  String getProperties();
  String getExtraEditForm();
  void setProperties(String properties);
  const char * getMessage();
private:
  uint16_t _range;
  uint16_t _by;
  uint8_t  _status;
  char _msgBuffer[128];
};

#endif
