#ifndef TFTWidgetColorbar_h
#define TFTWidgetColorbar_h

#include <Arduino.h>
#include <TFTWidgets.h>


class TFTWidgetColorbar: public Widget {
protected:
  uint16_t _minColor = ILI9341_BLUE;
  uint16_t _maxColor = ILI9341_RED;
  float _min = -30;
  float _max = 50;
  float _value = 25;
  uint8_t _decimals = 2;
  char _unit[5] = "";
  uint8_t _barwidth = 6;
  uint16_t _scColor = ILI9341_BLACK;
  uint8_t _scaleTics = 8;
  void drawValue(Adafruit_ILI9341 * tft);
  void deleteOld(Adafruit_ILI9341 * tft);
public:
  TFTWidgetColorbar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char name[], const char properties[], uint16_t yOffset, const GFXfont * font);
  TFTWidgetColorbar(const char properties[], uint16_t yOffset, const GFXfont * font);
  void init(const char properties[]);
  void draw(Adafruit_ILI9341 * tft);
  void update(Adafruit_ILI9341 * tft, const char data[], bool display);
  String getProperties();
  String getExtraEditForm();
  void setProperties(String properties);

private:
  uint8_t _rmin;
  uint8_t _gmin;
  uint8_t _bmin;
  uint8_t _rmax;
  uint8_t _gmax;
  uint8_t _bmax;
  uint8_t _bx,_by,_bh;
};

#endif
