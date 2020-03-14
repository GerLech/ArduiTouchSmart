#ifndef TFTWidgetButton_h
#define TFTWidgetButton_h

#include <Arduino.h>
#include <TFTWidgets.h>


class TFTWidgetButton: public Widget {
protected:
  uint8_t _value = 0;
  uint16_t _brdColor = ILI9341_BLUE;
  uint16_t _onColor = ILI9341_YELLOW;
  uint16_t _offColor = ILI9341_LIGHTGREY;
  uint8_t _switch = 0;

  void drawValue(Adafruit_ILI9341 * tft);
public:
  TFTWidgetButton(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char name[], const char properties[], uint16_t yOffset, const GFXfont * font);
  TFTWidgetButton(const char properties[],uint16_t yOffset, const GFXfont * font);
  void draw(Adafruit_ILI9341 * tft);
  void update(Adafruit_ILI9341 * tft, const char data[], bool display);
  void init( const char properties[]);
  bool touchEvent(Adafruit_ILI9341 * tft, int16_t x, int16_t y, EV event);
  String getProperties();
  String getExtraEditForm();
  void setProperties(String properties);
  int16_t getValue();
  void setStatus(uint8_t status);
  bool toPublish();
  const char * getMessage();
  bool hasTopic(char topic[]);
private:
  uint8_t  _status;
  uint16_t _bh;
  uint16_t _bw;
};

#endif
