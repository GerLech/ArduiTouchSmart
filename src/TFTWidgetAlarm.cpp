#include <TFTWidgetAlarm.h>
#include <ArduinoJson.h>

String alarmextraform = "["
  "{"
  "'name':'msg',"
  "'label':'Anzeigetext',"
  "'type':'text',"
  "'default':''"
  "},"
  "{"
  "'name':'min',"
  "'label':'Minimalwert',"
  "'type':'float'"
  "},"
  "{"
  "'name':'max',"
  "'label':'Maximalwert',"
  "'type':'float'"
  "},"
  "{"
  "'name':'compareMode',"
  "'label':'Vergleichsart',"
  "'type':'select',"
  "'options':["
  "{'l':'< min','v':'0'},"
  "{'l':'> max','v':'1'},"
  "{'l':'> min < max','v':'2'},"
  "{'l':'< min > max','v':'3'}"
  "]"
  "},"
  "{"
  "'name':'duration',"
  "'label':'stabil für s',"
  "'type':'number',"
  "'min':0,'max':3600"
  "},"
  "{"
  "'name':'alarmCol',"
  "'label':'Alarmfarbe',"
  "'type':'color'"
  "},"
  "{"
  "'name':'alarmTime',"
  "'label':'Alarm ein',"
  "'type':'number',"
  "'min':1,'max':200"
  "},"
  "{"
  "'name':'alarmInter',"
  "'label':'Wiederholung',"
  "'type':'number',"
  "'min':5,'max':300"
  "},"
  "{"
  "'name':'alarmLevel',"
  "'label':'Alarmlevel',"
  "'type':'number',"
  "'min':0,'max':15"
  "},"
  "{"
  "'name':'toFront',"
  "'label':'Vordergrund',"
  "'type':'check'"
  "}"
  "]";

TFTWidgetAlarm::TFTWidgetAlarm(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char name[], const char properties[], uint16_t yOffset, const GFXfont * font):Widget(properties){
  strlcpy(_name,name,15);
  _font = font;
  _yOffset = yOffset;
  _posx = x; _posy = y; _width = w; _height = h;
  init(properties);
}

TFTWidgetAlarm::TFTWidgetAlarm(const char properties[], uint16_t yOffset, const GFXfont * font):Widget(properties){
  _font = font;
  _yOffset = yOffset;
  init(properties);
}


void TFTWidgetAlarm::init(const char properties[]){
  _x=_posx * BLOCKSIZE; _y=_posy * BLOCKSIZE + _yOffset;
  if (_width<4) _width=4; if (_width>12) _width=12;
  if (_height<3) _height=3; if (_height>12) _height=12;
  _w = _width*BLOCKSIZE; _h = _height*BLOCKSIZE;
  Serial.print("New Alarm:");
  Serial.println(properties);
  StaticJsonDocument <1500>doc;
  DeserializationError   error = deserializeJson(doc,properties);
  if (error ) {
    Serial.println("JSON new alarm: ");
    Serial.println(properties);
    Serial.println(error.c_str());
  }
  if (strcmp(_label,"")==0) strlcpy(_label,"Alarm",50);
  if (doc.containsKey("alarmCol")) _alarmColor = doc["alarmCol"];
  if (doc.containsKey("min")) _min = doc["min"];
  if (doc.containsKey("max")) _max = doc["max"];
  if (doc.containsKey("default")) _value = doc["default"];
  if (doc.containsKey("duration")) _duration = doc["duration"];
  if (doc.containsKey("alarmTime")) _alarmTime = doc["alarmTime"];
  if (doc.containsKey("alarmInter")) _alarmIntervall = doc["alarmInter"];
  if (doc.containsKey("alarmLevel")) _alarmLevel = doc["alarmLevel"];
  if (doc.containsKey("compareMode")) _compareMode = atoi(doc["compareMode"]);
  if (doc.containsKey("toFront")) _toFront = doc["toFront"];
  if (doc.containsKey("msg")) strlcpy(_msg,doc["msg"],60);

  _status = ALSTAT_IDLE;

}

void TFTWidgetAlarm::draw(Adafruit_ILI9341 * tft) {
  drawBase(tft,20);
  drawValue(tft);
}

void TFTWidgetAlarm::update(Adafruit_ILI9341 * tft, const char data[], bool display) {
  uint8_t oldStatus = _status;
  char buf[200];
  strlcpy(buf,data,200);
  StaticJsonDocument <200>doc;
  DeserializationError   error = deserializeJson(doc,buf);
  if (error ) {
    Serial.println("JSON update alarm: ");
    Serial.println(error.c_str());
  }
  //Serial.println("Data");
  //Serial.println(data);
  //Serial.println(buf);
  //Serial.printf("Doc size = %i\n",doc.size());
  if (doc.containsKey(_valName)) {
     _value = doc[_valName];
    // Serial.printf("Value = %f\n",v);
   }
  //Serial.printf("New alarm value for %s = %f mode = %i\n",_valName, _value, _compareMode);
  switch (_compareMode) {
    case 0: _status = (_value < _min)?ALSTAT_WAIT:ALSTAT_IDLE; _waitcnt = _duration; break;
    case 1: _status = (_value > _max)?ALSTAT_WAIT:ALSTAT_IDLE; _waitcnt = _duration; break;
    case 2: _status = ((_value > _min) && (_value < _max))?ALSTAT_WAIT:ALSTAT_IDLE; _waitcnt = _duration; break;
    case 3: _status = ((_value < _min) || (_value > _max))?ALSTAT_WAIT:ALSTAT_IDLE; _waitcnt = _duration; break;
  }
  //Serial.printf("%f %f Status = %i\n",_value,_min,_status );
  if (display && (_status != oldStatus)) {
    deleteOld(tft);
    drawValue(tft);
    //Serial.printf("New alarm status = %i old satus = %i \n",_status,oldStatus);
  }
}

void TFTWidgetAlarm::deleteOld(Adafruit_ILI9341 * tft){
  tft->fillRect(_x+1,_y+21,_w-2,_h-22,_bgColor);
}


void TFTWidgetAlarm::drawValue(Adafruit_ILI9341 * tft){
  if ((_status == ALSTAT_ALARM) && (_blinkCnt < _alarmTime)) {
    tft->setTextColor(_alarmColor);
    tft->setTextWrap(true);
    String tmp;
    tmp = unicodeToDisplay(String(_msg));
    tft->setCursor(_x+1,_y+34);
    tft->print(tmp);
    tft->setTextWrap(false);
  } else {
    deleteOld(tft);
  }

}

String TFTWidgetAlarm::getProperties() {
  char buffer[1500];
  char tmp[10];
  Serial.println("Start get properties");
  StaticJsonDocument<1500> doc;
  DeserializationError   error = deserializeJson(doc,getBaseProperties());
  if (error ) {
    Serial.println("JSON set alarm properties: ");
    Serial.println(error.c_str());
  }
  doc["type"] = WT_ALARM;
  doc["alarmCol"] = _alarmColor;
  doc["min"] = _min;
  doc["max"] = _max;
  doc["msg"] = _msg;
  doc["duration"] = _duration;
  doc["alarmTime"] = _alarmTime;
  doc["alarmInter"] = _alarmIntervall;
  doc["alarmLevel"] = _alarmLevel;
  itoa(_compareMode,tmp,10);
  doc["compareMode"] = tmp;
  doc["toFront"] = _toFront;
  serializeJson(doc,buffer);
  Serial.println(String(buffer));
  return String(buffer);
}

void TFTWidgetAlarm::setProperties(String properties){
  setBaseProperties(properties);
  init(properties.c_str());
}

String TFTWidgetAlarm::getExtraEditForm() {
  return alarmextraform;
}

void TFTWidgetAlarm::everySecond(Adafruit_ILI9341 * tft, boolean display) {
  if (display) drawValue(tft);
  if (_status == ALSTAT_WAIT) {
    _waitcnt--;
    if (_waitcnt < 0) {
      _status = ALSTAT_ALARM;
      if (_toFront) _switchpage = true;
      Serial.printf("New alarm status = %i\n",_status);
    }
  }
  _blinkCnt++;
  if (_blinkCnt > _alarmIntervall) _blinkCnt = 0;
}

bool TFTWidgetAlarm::touchEvent(Adafruit_ILI9341 * tft, int16_t x, int16_t y, EV event) {
  if (isInside(x,y)) {
    Serial.printf("Pos. x=%i y=%i event = %i\n",x,y,event);
    _status = ALSTAT_IDLE;
    deleteOld(tft);
    return true;
  } else {
    return false;
  }
};

uint16_t TFTWidgetAlarm::hasAlarm(uint16_t level) {
  uint16_t lv = _alarmLevel & 0xf;
  if (_switchpage) lv += 16;
  _switchpage = false; //only once!
  if ((_status == ALSTAT_ALARM) && (lv > level) ) {
    return lv;
  } else {
    return level;
  }
}
