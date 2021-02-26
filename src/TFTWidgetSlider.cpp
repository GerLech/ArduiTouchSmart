#include <TFTWidgetSlider.h>
#include <ArduinoJson.h>

String sliderextraform = "["
  "{"
  "'name':'min',"
  "'label':'Minimalwert',"
  "'type':'number'"
  "},"
  "{"
  "'name':'max',"
  "'label':'Maximalwert',"
  "'type':'number'"
  "},"
  "{"
  "'name':'sliderCol',"
  "'label':'Farbe Slider',"
  "'type':'color'"
  "}"
  "]";


TFTWidgetSlider::TFTWidgetSlider(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char name[], const char properties[], uint16_t yOffset, const GFXfont * font):Widget(properties){
  strlcpy(_name,name,15);
  _font = font;
  _yOffset = yOffset;
  _posx = x; _posy = y; _width = w; _height = h;
  init(properties);
}

TFTWidgetSlider::TFTWidgetSlider(const char properties[], uint16_t yOffset, const GFXfont * font):Widget(properties){
  _font = font;
  _yOffset = yOffset;
  init(properties);
}

void TFTWidgetSlider::init(const char properties[]) {
  _x=_posx * BLOCKSIZE; _y=_posy * BLOCKSIZE + _yOffset;
  if (_width<5) _width=5;
  if (_height>4) _height=4;
  if (_height<3) _height=3;
  _w = _width*BLOCKSIZE; _h = _height*BLOCKSIZE;
  Serial.print("New Slider:");
  Serial.println(properties);
  StaticJsonDocument <1500>doc;
  DeserializationError   error = deserializeJson(doc,properties);
  if (error ) {
    Serial.println("JSON new slider: ");
    Serial.println(properties);
    Serial.println(error.c_str());
  }
  if (strcmp(_label,"")==0) strlcpy(_label,"Slider",50);
  if (doc.containsKey("min")) _min = doc["min"];
  if (doc.containsKey("max")) _max = doc["max"];
  if (doc.containsKey("default")) _value = doc["default"];
  if (doc.containsKey("sliderCol")) _slColor = doc["sliderCol"];
  _range = _w-40;
  _by = (_h-22)/2+20;
  _status = ST_NEW;
}

void TFTWidgetSlider::draw(Adafruit_ILI9341 * tft) {
  drawBase(tft,20,HA::ALIGN_LEFT);
  drawValue(tft);
}

void TFTWidgetSlider::update(Adafruit_ILI9341 * tft, const char data[], bool display) {
  char buf[200];
  strlcpy(buf,data,200);
  StaticJsonDocument <200>doc;
  DeserializationError   error = deserializeJson(doc,buf);
  if (error ) {
    Serial.println("JSON update slider: ");
    Serial.println(error.c_str());
  }
  if (doc.containsKey(_valName)) _value = doc[_valName];
  if (display) {
    deleteOld(tft);
    drawValue(tft);
  }
}

void TFTWidgetSlider::deleteOld(Adafruit_ILI9341 * tft){
  tft->fillRect(_x+_w/2,_y+1,_w/2-1,20,_bgColor);
  tft->fillRect(_x+1,_y+20,_w-2,_h-22,_bgColor);
}


void TFTWidgetSlider::drawValue(Adafruit_ILI9341 * tft){
  tft->setTextSize(1);
  tft->setTextColor(_fontColor);
  tft->setTextWrap(false);
  tft->setFont(_font);
  Serial.println(_value);
  centerValue(tft,_value,_x+_w/2,_y,_w/2-1,20,"");
  uint16_t p = (_value-_min)*_range/(_max-_min);
  tft->drawRoundRect(_x+1+p,_y+_by-9,38,18,8,_slColor);
  tft->fillRect(_x+19+p, _y+_by-5,2,10,_slColor);
  tft->fillRect(_x+14+p, _y+_by-5,2,10,_slColor);
  tft->fillRect(_x+23+p, _y+_by-5,2,10,_slColor);
  if (p>20) tft->fillRect(_x+20,_y+_by-2,p-19,4,_slColor);
  if (p<_range-20) tft->fillRect(_x+p+38,_y+_by-1,_range-p-19,2,_slColor);
}


bool TFTWidgetSlider::touchEvent(Adafruit_ILI9341 * tft, int16_t x, int16_t y, EV event) {
  if (isInside(x,y)) {
    if (event == EV::EVT_CLICK) {
      int16_t old = _value;
      _value = (x-_x-20)*(_max - _min)/_range + _min;
      if (_value < _min) _value = _min;
      if (_value > _max) _value = _max;
      if (_value != old) {
        _status = ST_CHANGED;
        deleteOld(tft);
        drawValue(tft);
      }
    }
    return true;
  } else {
    return false;
  }
};

int16_t TFTWidgetSlider::getValue() {
  return _value;
}
void TFTWidgetSlider::setStatus(uint8_t status) {
  _status = status;
}

bool TFTWidgetSlider::toPublish() {
  return ((_status == ST_CHANGED)) ;
}

String TFTWidgetSlider::getProperties() {
  char buffer[1000];
  StaticJsonDocument<1000> doc;
  DeserializationError   error = deserializeJson(doc,getBaseProperties());
  if (error ) {
    Serial.println("JSON set slider properties: ");
    Serial.println(error.c_str());
  }
  doc["type"] = WT_SLIDER;
  doc["min"] = _min;
  doc["max"] = _max;
  doc["sliderCol"] = _slColor;
  serializeJson(doc,buffer);
  return String(buffer);
}

void TFTWidgetSlider::setProperties(String properties){
  setBaseProperties(properties);
  init(properties.c_str());
}

String TFTWidgetSlider::getExtraEditForm() {
  return sliderextraform;
}

const char * TFTWidgetSlider::getMessage() {
  if (_status = ST_NEW) return "";
  sprintf(_msgBuffer,"%i",_value);
  return _msgBuffer;
}
