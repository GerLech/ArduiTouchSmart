#include <TFTWidgets.h>
#include <TFTWidgetPage.h>
#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

#include <stdlib.h> // for malloc and free

TFTWidgetPage::TFTWidgetPage(Adafruit_ILI9341 * tft, TFTForm * conf, const GFXfont * font, bool header, bool footer) {
  _header = header;
  _footer = footer;
  _tft = tft;
  _conf = conf;
  _font = font;
  _width = tft->width();
  _xBlocks = _width/BLOCKSIZE;
  _height = tft->height();
  if (_header) _height -= HEADER;
  if (_footer) _height -= FOOTER;
  _yBlocks = _height/BLOCKSIZE;
  _yOffset = (_header)?HEADER:0;
}


Error TFTWidgetPage::addWidget(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t type, const char name[], const char style[] ){
  Error err = Error::NO_ERROR;
  if (_cnt >= MAXWIDGETS) err = Error::NO_MORE_WIDGETS;
  if ((x+width > _xBlocks) || (y >= _yBlocks)) err = Error::WIDGET_OUTSIDE;
  uint8_t last = _cnt;
  if (err == Error::NO_ERROR){
    switch (type) {
      case WT_GAUGE : _widgets[_cnt] = new TFTWidgetGauge(x,y,width,name,style,_yOffset,_font) ; _cnt++; break;
      case WT_COLORBAR: _widgets[_cnt] = new TFTWidgetColorbar(x,y,width,height,name,style,_yOffset,_font) ; _cnt++; break;
      case WT_SIMPLE: _widgets[_cnt] = new TFTWidgetSimple(x,y,width,height,name,style,_yOffset,_font) ; _cnt++; break;
      case WT_SLIDER: _widgets[_cnt] = new TFTWidgetSlider(x,y,width,height,name,style,_yOffset,_font) ; _cnt++; break;
      case WT_SWITCH: _widgets[_cnt] = new TFTWidgetSwitch(x,y,width,height,name,style,_yOffset,_font) ; _cnt++; break;
      case WT_BUTTON: _widgets[_cnt] = new TFTWidgetButton(x,y,width,height,name,style,_yOffset,_font) ; _cnt++; break;
      case WT_COLORSLIDER: _widgets[_cnt] = new TFTWidgetColorSlider(x,y,width,height,name,style,_yOffset,_font) ; _cnt++; break;
      case WT_ALARM: _widgets[_cnt] = new TFTWidgetAlarm(x,y,width,height,name,style,_yOffset,_font) ; _cnt++; break;
    }
  }
  return err;
}

Error TFTWidgetPage::addWidget(const char style[] ){
  Error err = Error::NO_ERROR;
  if (_cnt >= MAXWIDGETS) err = Error::NO_MORE_WIDGETS;
  StaticJsonDocument<1500> doc;
  DeserializationError   error = deserializeJson(doc,style);
  if (error ) {
    Serial.println("JSON Page add widget: ");
    Serial.println(style);
    Serial.println(error.c_str());
  }
  if (!doc.containsKey("type"))  {
    err = Error::NO_TYPE;
  } else {
    uint8_t type = doc["type"];
    Serial.print("Add to page ");
    Serial.println(style);
    Serial.printf("Type %i added\n",type );
    uint8_t last = _cnt;
    if (err == Error::NO_ERROR){
      switch (type) {
        case WT_GAUGE : _widgets[_cnt] = new TFTWidgetGauge(style,_yOffset,_font) ; _cnt++; break;
        case WT_COLORBAR: _widgets[_cnt] = new TFTWidgetColorbar(style,_yOffset,_font) ; _cnt++; break;
        case WT_SIMPLE: _widgets[_cnt] = new TFTWidgetSimple(style,_yOffset,_font) ; _cnt++; break;
        case WT_SLIDER: _widgets[_cnt] = new TFTWidgetSlider(style,_yOffset,_font) ; _cnt++; break;
        case WT_SWITCH: _widgets[_cnt] = new TFTWidgetSwitch(style,_yOffset,_font) ; _cnt++; break;
        case WT_BUTTON: _widgets[_cnt] = new TFTWidgetButton(style,_yOffset,_font) ; _cnt++; break;
        case WT_COLORSLIDER: _widgets[_cnt] = new TFTWidgetColorSlider(style,_yOffset,_font) ; _cnt++; break;
        case WT_ALARM: _widgets[_cnt] = new TFTWidgetAlarm(style,_yOffset,_font) ; _cnt++; break;
      }
    }
  }
  return err;
}


void TFTWidgetPage::removeWidget(uint8_t index){
  if (index >= _cnt) return;
  delete(_widgets[index]);
  _cnt--;
  for (uint8_t i = index; i<_cnt; i++) _widgets[i] = _widgets[i+1];
}


void TFTWidgetPage::drawWidget(uint8_t index) {
  if (index < _cnt) _widgets[index]->draw(_tft);
}

void TFTWidgetPage::updateValue(uint8_t index, const char data[]){
  if (index < _cnt) _widgets[index]->update(_tft, data, (_onEdit < 0));
}

void TFTWidgetPage::drawPage(){
  _tft->fillRect(0,_yOffset,_width,_height,ILI9341_BLACK);
  for (uint8_t i = 0; i<_cnt; i++) drawWidget(i);
}

uint8_t TFTWidgetPage::getWidgetCount(){
  return _cnt;
}

bool TFTWidgetPage::touchEvent(int16_t x, int16_t y, EV event) {
  bool flag = false;
  uint8_t i = 0;
  while ((i<_cnt) && (!flag)) {
    flag = _widgets[i]->touchEvent(_tft, x, y, event);
    i++;
  }
  return flag;
}

bool TFTWidgetPage::selectWidget(int16_t x, int16_t y) {
  for (uint8_t i = 0; i<_cnt; i++) _widgets[i]->select(x,y,_tft);
}

int16_t TFTWidgetPage::getSelectedIndex() {
  int16_t i = _cnt-1;
  while ((i >= 0 ) && (!_widgets[i]->isSelected())) i--;
  return i;
}


void TFTWidgetPage::editWidget(bool base){
  uint8_t i = 0;
  while ((i<_cnt) && (!_widgets[i]->isSelected())) i++;
  if (i<_cnt) {
    if (base) {
      _conf->setDescription(_widgets[i]->getBaseEditForm());
    } else {
      _conf->setDescription(_widgets[i]->getExtraEditForm());
    }
    _conf->setValues(_widgets[i]->getProperties());
    Serial.println("set props done");
    _conf->showForm();
    _onEdit = i;
  }
}


void TFTWidgetPage::updateTopic(char topic[], const char data[], bool visible){
  //remove suffix RESULT if exist
  char tp[128];
  strlcpy(tp,topic,128);
  char * pch = strrchr(tp,'/');
  if ((pch != NULL) && (strcmp(pch+1,"RESULT")==0)) * pch = 0;
  for (uint8_t i = 0; i<_cnt; i++) {
    if (_widgets[i]->hasTopic(tp)) _widgets[i]->update(_tft, data,((_onEdit < 0)&& visible));
  }
}

int16_t TFTWidgetPage::findTopic(char topic[]) {
  int16_t index = _cnt-1;
  while ((index >= 0) && (!_widgets[index]->hasTopic(topic))) index--;
  return index;
}

const char * TFTWidgetPage::getTopic(uint8_t index){
  if (index < _cnt)  {
    return _widgets[index]->getTopic();
  } else {
    return(NULL);
  }
}

void TFTWidgetPage::registerOnPublish(bool (*callback)(const char * topic, const char * message)) {
  _onPublish = callback;
}


void TFTWidgetPage::checkForPublish(){
  const char * topic;
  for (uint8_t i = 0;i<_cnt;i++){
    topic = _widgets[i]->getPubTopic();
    if (_widgets[i]->toPublish() && _onPublish && (strlen(topic)>0)) {
      if (_onPublish(topic ,_widgets[i]->getMessage())) {
        _widgets[i]->setStatus(ST_STATIC);
      }
    }
  }
}

void TFTWidgetPage::endEdit(String data) {
  if (_onEdit >= 0) {
    uint16_t yoff = (_header)?HEADER:0;
    Serial.print("Return: ");
    Serial.println(data);
    if (data != "") {
      _widgets[_onEdit]->setProperties(data);
    }
    drawPage();
    _onEdit = -1;
  }
}

void TFTWidgetPage::deleteAllWidgets() {
  for (uint8_t i = 0; i<_cnt; i++) delete(_widgets[i]);
  _cnt = 0;
}

void TFTWidgetPage::setFormat(bool header, bool footer) {
  _header = header;
  _footer = footer;
}

uint8_t TFTWidgetPage::savePage(uint8_t pagenum){
  char buf[30];
  File file;
  for (uint8_t i = 0; i< _cnt; i++){
    sprintf(buf,"/wdgconf/p%i/w%i",pagenum,i);
    file = SPIFFS.open(buf,"w+");
    file.print(_widgets[i]->getProperties());
    file.close();
  }
}

void TFTWidgetPage::everySecond(boolean isCurrent) {
  for (uint8_t i = 0; i< _cnt; i++){
    _widgets[i]->everySecond(_tft,((_onEdit < 0) && isCurrent));
  }
}

uint16_t TFTWidgetPage::hasAlarm(uint16_t level) {
  for (uint8_t i = 0; i< _cnt; i++){
    level = _widgets[i]->hasAlarm(level);
  }
  return level;
}


String TFTWidgetPage::getGeometrie(uint8_t index) {
  if (index < _cnt) {
    return _widgets[index]->getGeometrie();
  } else {
    return "";
  }
}

Widget * TFTWidgetPage::releaseWidget(uint8_t index) {
  if (index < _cnt) {
    Widget * w = _widgets[index];
    _cnt--;
    for (uint8_t i = index; i<_cnt; i++) _widgets[i] = _widgets[i+1];
    return w;
  } else {
    return NULL;
  }

}
void TFTWidgetPage::appendWidget(Widget * widget) {
  if (_cnt < MAXWIDGETS) {
    _widgets[_cnt]=widget;
    _cnt++;
  }
}
