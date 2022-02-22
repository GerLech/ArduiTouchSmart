#ifndef TFTWidgetAlarm_h
#define TFTWidgetAlarm_h

#include <Arduino.h>
#include <TFTWidgets.h>

#define ALSTAT_IDLE 0
#define ALSTAT_WAIT 1
#define ALSTAT_ALARM 2

class TFTWidgetAlarm: public Widget {
protected:
  uint16_t _alarmColor = ILI9341_RED;
  float _min = -30;
  float _max = 50;
  int16_t  _duration = 300;
  int16_t _alarmTime = 2;
  int16_t _alarmIntervall = 3;
  uint16_t _alarmLevel = 0;
  uint8_t _compareMode = 0;
  uint8_t _toFront = 0;
  char _msg[60] = "";
  uint8_t _status = ALSTAT_IDLE;
  float _value = 25;
  void drawValue(Adafruit_ILI9341 * tft);
  void deleteOld(Adafruit_ILI9341 * tft);
public:
  TFTWidgetAlarm(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char name[], const char properties[], uint16_t yOffset, const GFXfont * font);
  TFTWidgetAlarm(const char properties[], uint16_t yOffset, const GFXfont * font);
  void init(const char properties[]);
  void draw(Adafruit_ILI9341 * tft);
  void update(Adafruit_ILI9341 * tft, const char data[], bool display);
  String getProperties();
  String getExtraEditForm();
  void setProperties(String properties);
  void everySecond(Adafruit_ILI9341 * tft, boolean display);
  uint16_t hasAlarm(uint16_t level);
  bool touchEvent(Adafruit_ILI9341 * tft, int16_t x, int16_t y, EV event);
private:
  int16_t _waitcnt = 0;
  uint16_t _blinkCnt = 0;
  bool _switchpage = false;

};

#endif
