#ifndef TFTWidgetSwitch_h
#define TFTWidgetSwitch_h

#include <Arduino.h>
#include <TFTWidgets.h>


class TFTWidgetSwitch: public Widget {
protected:
  uint8_t _value = 0;
  uint16_t _swColor = ILI9341_LIGHTGREY;
  uint16_t _swbrdColor = ILI9341_BLACK;
  uint16_t _onColor = ILI9341_GREEN;
  uint16_t _offColor = ILI9341_RED;
  char _onLabel[5] = "on";
  char _offLabel[5] = "off";

  void drawValue(Adafruit_ILI9341 * tft);
  void deleteOld(Adafruit_ILI9341 * tft);
public:
  TFTWidgetSwitch(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char name[], const char properties[], uint16_t yOffset, const GFXfont * font);
  TFTWidgetSwitch(const char properties[], uint16_t yOffset, const GFXfont * font);
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
  bool hasTopic(char topic[]);
private:
  uint8_t  _status;
  uint16_t _dx;
  uint16_t _dy;
};

#endif
