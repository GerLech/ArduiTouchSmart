/*

File TFTWidgets.h
Version 1.0
Author Gerald Lechner
contakt lechge@gmail.com

Description
This library extends the library Adafruit_ILI9341 and Adafruit_GFX. Around an
abstract base class several widget classes several widget classesa have been
designed.
For easier handling the screen with 240 x 320 pixels is devided into blocks
40x40 pixels. Every widget consists out of one or more blocks. Every widget
class has a list of allowed sizes.

Dependencies:
  Adafruit_ILI9341.h
  Adafruit_GFX.h

*/

#ifndef TFTWidgets_h
#define TFTWidgets_h


#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <ArduinoJson.h>
#include "fonts/AT_Bold12pt7b.h"
#include <TouchEvent.h>

#define WT_SIMPLE 0
#define WT_GAUGE 1
#define WT_COLORBAR 2
#define WT_BUTTON 3
#define WT_SLIDER 4
#define WT_SWITCH 5
#define WT_COLORSLIDER 6
#define WT_ALARM 7


enum class Error : uint8_t {
    NO_ERROR          = 0,
    NO_MORE_WIDGETS   = 1,
    WIDGET_OUTSIDE    = 2,
    WRONG_SIZE        = 3,
    NO_MORE_PAGES     = 4,
    NO_TYPE           = 5,
    FS_CANNOT_WRITE   = 6
};

enum class VA : uint8_t {
  ALIGN_TOP    = 0,
  ALIGN_MIDDLE = 1,
  ALIGN_BOTTOM = 2,
  ALIGN_FIX    = 3
};

enum class HA : uint8_t {
  ALIGN_LEFT   = 0,
  ALIGN_CENTER = 1,
  ALIGN_RIGHT  = 2,
};

#define BLOCKSIZE 20
#define ST_NEW 0
#define ST_CHANGED 1
#define ST_STATIC 2
#define HEADER 20
#define FOOTER 20

//abstract base class
class Widget{
protected:
  uint16_t _w; //with in pixels
  uint16_t _h; //height in pixels
  uint16_t _x; //x-position upper left corner
  uint16_t _y; //y-position upper left corner
  uint16_t _width = 2; //with in pixels
  uint16_t _height = 2; //height in pixels
  uint16_t _posx = 0; //x-position upper left corner
  uint16_t _posy = 0; //y-position upper left corner
  uint16_t _bgColor = ILI9341_WHITE;
  uint16_t _borderColor = ILI9341_BLACK;
  uint16_t _fontColor = ILI9341_BLACK;
  const GFXfont * _font;
  char _valName[15]="value";
  char _unitName[15]="unit";
  char _name[15] = "";
  char _label[50] = "";
  char _topic[128] = "";
  bool _selected = false;
  int16_t _yOffset = 0;
  void drawBase(Adafruit_ILI9341 * tft,uint16_t hl, HA h_align = HA::ALIGN_CENTER, bool withLabel = true);
  void centerText(Adafruit_ILI9341 * tft, char text[], int16_t x, int16_t y, int16_t w, int16_t h, HA h_align, VA v_align);
  void centerValue(Adafruit_ILI9341 * tft, int16_t value, int16_t x, int16_t y, int16_t w, int16_t h, const char * unit );
  void centerValue(Adafruit_ILI9341 * tft, float value, int16_t x, int16_t y, int16_t w, int16_t h, uint8_t decimals, const char * unit);
  bool isInside(int16_t x, int16_t y);
  String unicodeToDisplay(String text);
public:
  //the position and size of a widget is set. Units are blocks of 40 pixels
  //every widget defines the height based on width. If the width is to small
  //or to big or does not fit on screen false will be returned
  Widget(const char json[]);
  virtual void draw(Adafruit_ILI9341 * tft)=0;
  virtual void update(Adafruit_ILI9341 * tft, const char data[], bool display)=0;
  virtual void init( const char properties[])=0;
  virtual String getProperties()=0;
  virtual void setProperties(String properties)=0;
  virtual String getExtraEditForm()=0;
  virtual const char * getPubTopic() {
    strcpy(_topicBuffer,_topic);
    strcat(_topicBuffer,"/");
    strcat(_topicBuffer,_valName);
    return _topicBuffer;
  };
  virtual const char * getTopic() {return _topic;}
  virtual const char * getMessage() {return "";}
  virtual bool touchEvent(Adafruit_ILI9341 * tft, int16_t x, int16_t y, EV event)  {return false;};
  virtual bool toPublish() {return false;};
  virtual int16_t getValue() {return 0;};
  virtual void everySecond(Adafruit_ILI9341 * tft, boolean display) {};
  virtual uint16_t hasAlarm(uint16_t level) {return level;};
  virtual void setStatus(uint8_t status){};
  virtual bool hasTopic(char topic[]);
  String getBaseEditForm();
  String getBaseProperties();
  String getGeometrie();
  void setBaseProperties(String properties);
  void select(int16_t x, int16_t y, Adafruit_ILI9341 * tft);
  bool isSelected();
private:
  char _topicBuffer[128];



};


#endif
