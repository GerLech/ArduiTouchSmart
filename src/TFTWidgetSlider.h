#ifndef TFTWidgetSlider_h
#define TFTWidgetSlider_h

#include <Arduino.h>
#include <TFTWidgets.h>


class TFTWidgetSlider: public Widget {
protected:
  int16_t _min = 0;
  int16_t _max = 100;
  int16_t _value = 0;
  uint16_t _slColor = ILI9341_BLUE;
  void drawValue(Adafruit_ILI9341 * tft);
  void deleteOld(Adafruit_ILI9341 * tft);
public:
  TFTWidgetSlider(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char name[], const char properties[], uint16_t yOffset, const GFXfont * font);
  TFTWidgetSlider(const char properties[], uint16_t yOffset, const GFXfont * font);
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
  char _msgBuffer[20];
};

#endif
