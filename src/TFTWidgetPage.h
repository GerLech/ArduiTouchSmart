#ifndef TFTWidgetPage_h
#define TFTWidgetPage_h

#include <Arduino.h>
#include <Adafruit_ILI9341.h>

#define MAXWIDGETS 20

#include <TFTWidgetGauge.h>
#include <TFTWidgetColorbar.h>
#include <TFTWidgetSimple.h>
#include <TFTWidgetSlider.h>
#include <TFTWidgetSwitch.h>
#include <TFTWidgetButton.h>
#include <TFTWidgetColorSlider.h>
#include <TFTForm.h>

class TFTWidgetPage{
public:
  TFTWidgetPage(Adafruit_ILI9341 * tft, TFTForm * conf, const GFXfont * font, bool header, bool footer);
  Error addWidget(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t type, const char name[], const char style[] );
  Error addWidget(const char style[] );
  void removeWidget(uint8_t index);
  void drawWidget(uint8_t index);
  void updateValue(uint8_t index, const char data[]);
  void updateTopic(char topic[], const char data[], bool visible);
  int16_t findTopic(char topic[]);
  void drawPage();
  bool touchEvent(int16_t x, int16_t y, EV event);
  uint8_t getWidgetCount();
  const char * getTopic(uint8_t index);
  void registerOnPublish(bool (*callback)(const char * topic, const char * message));
  void checkForPublish();
  void setFormat(bool header, bool footer);
  void endEdit(String data);
  void deleteAllWidgets();
  bool selectWidget(int16_t x, int16_t y);
  void editWidget(bool base);
  int16_t getSelectedIndex();
  uint8_t savePage(uint8_t pagenum);
  String getGeometrie(uint8_t index);
  Widget * releaseWidget(uint8_t index);
  void appendWidget(Widget * widget);



private:
  Widget * _widgets[MAXWIDGETS];
  uint8_t _cnt = 0;
  uint16_t _width;
  uint16_t _height;
  int16_t _yOffset;
  uint8_t _xBlocks;
  uint8_t _yBlocks;
  bool _header = true;
  bool _footer = true;
  bool _confMode = false;
  int8_t _onEdit = -1;
  Adafruit_ILI9341 * _tft;
  TFTForm * _conf;
  bool(*_onPublish)(const char * topic, const char * message) = NULL;
  const GFXfont * _font;
};

#endif
