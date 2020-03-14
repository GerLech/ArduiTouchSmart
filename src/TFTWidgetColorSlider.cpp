#include <TFTWidgetColorSlider.h>
#include <ArduinoJson.h>

String colorsliderextraform = "["
  "{"
  "'name':'min',"
  "'label':'Minimalwert',"
  "'type':'number'"
  "},"
  "{"
  "'name':'max',"
  "'label':'Maximalwert',"
  "'type':'number'"
  "}"
  "]";


TFTWidgetColorSlider::TFTWidgetColorSlider(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char name[], const char properties[], uint16_t yOffset, const GFXfont * font):Widget(properties){
  strlcpy(_name,name,15);
  _font = font;
  _yOffset = yOffset;
  _posx = x; _posy = y; _width = w; _height = h;
  init(properties);
}

TFTWidgetColorSlider::TFTWidgetColorSlider(const char properties[], uint16_t yOffset, const GFXfont * font):Widget(properties){
  _font = font;
  _yOffset = yOffset;
  init(properties);
}

void TFTWidgetColorSlider::init(const char properties[]) {
  _x=_posx * BLOCKSIZE; _y=_posy * BLOCKSIZE + _yOffset;
  if (_width<5) _width=5;
  if (_height>6) _height=6;
  if (_height<5) _height=5;
  _w = _width*BLOCKSIZE; _h = _height*BLOCKSIZE;
  StaticJsonDocument <1500>doc;
  Serial.println(properties);
  deserializeJson(doc, properties);
  if (strcmp(_label,"")==0) strlcpy(_label,"ColorSlider",50);
  if (doc.containsKey("red")) _red = doc["red"];
  if (doc.containsKey("green")) _green = doc["green"];
  if (doc.containsKey("blue")) _blue = doc["blue"];
  _range = _w-40;
  _by = (_h-22)/2+20;
  _status = ST_NEW;
}

void TFTWidgetColorSlider::draw(Adafruit_ILI9341 * tft) {
  drawBase(tft,20,HA::ALIGN_LEFT);
  drawValue(tft);
}

void TFTWidgetColorSlider::update(Adafruit_ILI9341 * tft, const char data[], bool display) {
  //char buf[200];
  //strlcpy(buf,data,200);
  char msg[30];
  uint16_t r,g,b;
  StaticJsonDocument <400>doc;
  DeserializationError   error = deserializeJson(doc,data);
  if (error ) {
    Serial.print("JSON: ");
    Serial.println(error.c_str());
  }
  if (doc.containsKey(_valName)) {
    strlcpy(msg,doc[_valName],30);
    sscanf(msg,"%i,%i,%i",&r,&g,&b);
    _red = r; _green = g; _blue = b;
  }
  if (display) {
    deleteOld(tft);
    drawValue(tft);
  }
}

void TFTWidgetColorSlider::deleteOld(Adafruit_ILI9341 * tft){
  tft->fillRect(_x+1,_y+20,_w-2,_h-22,_bgColor);
}


void TFTWidgetColorSlider::drawSlider(Adafruit_ILI9341 * tft, uint8_t val,uint16_t y, uint16_t color) {
  uint16_t p = (val-_min)*_range/(_max-_min);
  tft->drawRoundRect(_x+1+p,y-9,38,18,8,color);
  tft->fillRect(_x+19+p, y-5,2,10,color);
  tft->fillRect(_x+14+p, y-5,2,10,color);
  tft->fillRect(_x+23+p, y-5,2,10,color);
  if (p>20) tft->fillRect(_x+20,y-2,p-19,4,color);
  if (p<_range-20) tft->fillRect(_x+p+38,y-1,_range-p-19,2,color);
}

void TFTWidgetColorSlider::drawValue(Adafruit_ILI9341 * tft){
  tft->setTextSize(1);
  tft->setTextColor(_fontColor);
  tft->setTextWrap(false);
  tft->setFont(_font);
  drawSlider(tft,_red,_y+_by-24,ILI9341_RED);
  drawSlider(tft,_green,_y+_by,ILI9341_GREEN);
  drawSlider(tft,_blue,_y+_by+24,ILI9341_BLUE);
}


bool TFTWidgetColorSlider::touchEvent(Adafruit_ILI9341 * tft, int16_t x, int16_t y, EV event) {
  if (isInside(x,y)) {
    if (event == EV::EVT_CLICK) {
      int16_t v = 0;
      v = (x-_x-20)*(_max - _min)/_range + _min;
      if (v < _min) v = _min;
      if (v > _max) v = _max;
      if (y < (_y+_by-12)) {
        if (v != _red ) {
          _red = v;
          _status = ST_CHANGED;
        }
      } else if (y < (_y+_by+12)) {
        if (v != _green ) {
          _green = v;
          _status = ST_CHANGED;
        }
      } else {
        if (v != _blue ) {
          _blue = v;
          _status = ST_CHANGED;
        }
      }
      if (_status == ST_CHANGED) {
        deleteOld(tft);
        drawValue(tft);
      }
    }
    return true;
  } else {
    return false;
  }
};

int16_t TFTWidgetColorSlider::getValue() {
  return _red*65536+_green*256+_blue;
}
void TFTWidgetColorSlider::setStatus(uint8_t status) {
  _status = status;
}

bool TFTWidgetColorSlider::toPublish() {
  return ((_status == ST_CHANGED) || (_status == ST_NEW)) ;
}


String TFTWidgetColorSlider::getProperties() {
  char buffer[1000];
  StaticJsonDocument<1000> doc;
  deserializeJson(doc,getBaseProperties());
  doc["type"] = WT_COLORSLIDER;
  doc["min"] = _min;
  doc["max"] = _max;
  serializeJson(doc,buffer);
  return String(buffer);
}

void TFTWidgetColorSlider::setProperties(String properties){
  setBaseProperties(properties);
  init(properties.c_str());
}


String TFTWidgetColorSlider::getExtraEditForm() {
  return colorsliderextraform;
}

const char * TFTWidgetColorSlider::getMessage() {
  if (_status = ST_NEW) return "";
  sprintf(_msgBuffer,"%i,%i,%i",_red,_green,_blue);
  return _msgBuffer;
}
