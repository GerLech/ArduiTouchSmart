#ifndef TFTWidgetChart_h
#define TFTWidgetChart_h

#include <Arduino.h>
#include <TFTWidgets.h>

#define CTPOINT 0
#define CTLINE 1
#define CTBAR 2

class TFTWidgetChart: public Widget {
protected:
  char _unit[5] = "";
  float _value = 25;
  int8_t _chartType = CTLINE;
  float _min = -10;
  float _max = 10;
  float _minGw = -5;
  float _maxGw = 5;
  uint16_t _timerange = 60;
  float _xRes = 10;
  float _yRes = 2;
  uint16_t _nullCol = ILI9341_BLUE;
  uint16_t _gridCol = ILI9341_BLACK;
  uint16_t _valCol = ILI9341_RED;
  uint16_t _gwCol = ILI9341_GREEN;
  uint8_t _showLim = 1;


  void drawValue(Adafruit_ILI9341 * tft);
  void deleteOld(Adafruit_ILI9341 * tft);
public:
  TFTWidgetChart(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char name[], const char properties[], uint16_t yOffset, const GFXfont * font);
  TFTWidgetChart(const char properties[], uint16_t yOffset, const GFXfont * font);
  void init(const char properties[]);
  void draw(Adafruit_ILI9341 * tft);
  void drawGrid(Adafruit_ILI9341 * tft);
  void drawCurve(Adafruit_ILI9341 * tft);
  void update(Adafruit_ILI9341 * tft, const char data[], bool display);
  String getProperties();
  void everySecond(Adafruit_ILI9341 * tft, boolean display);
  String getExtraEditForm();
  void setProperties(String properties);
private:
  uint16_t _values[50];
  int16_t _x0;
  int16_t _y0;
  float _yScale;
  float _xScale;
  uint16_t _ymax;
  uint16_t _ymin;
  uint16_t _xmax;
  uint16_t _xmin;
  uint8_t _maxCnt;
  uint8_t _cnt;
  uint8_t _mw;
  uint16_t _intervall;
  uint16_t _seconds;
  float _average;
  uint16_t _cntMw;
};

#endif
