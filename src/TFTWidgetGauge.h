#ifndef TFTWidgetGauge_h
#define TFTWidgetGauge_h

#include <Arduino.h>
#include <TFTWidgets.h>


class TFTWidgetGauge: public Widget {
protected:
  uint16_t _pointerColor = ILI9341_RED;
  uint16_t _bowColor = ILI9341_BLUE;
  float _min = 0;
  float _max = 100;
  float _value = 50;
  uint8_t _decimals = 2;
  char _unit[5] = "";
  void drawValue(Adafruit_ILI9341 * tft);
  void deleteOld(Adafruit_ILI9341 * tft);
public:
  TFTWidgetGauge(uint8_t x, uint8_t y, uint8_t w, const char name[], const char properties[], uint16_t yOffset, const GFXfont * font);
  TFTWidgetGauge(const char properties[], uint16_t yOffset, const GFXfont * font);
  void init(const char properties[]);
  void draw(Adafruit_ILI9341 * tft);
  void update(Adafruit_ILI9341 * tft, const char data[], bool display);
  void setDimensions(uint16_t yOffset);
  String getProperties();
  String getExtraEditForm();
  void setProperties(String properties);
private:
  int16_t _xl = 0;
  int16_t _yl = 0;
  uint16_t _radius;
  uint16_t _margin;
  uint16_t _textwidth;
  int16_t _mid;
};

#endif
