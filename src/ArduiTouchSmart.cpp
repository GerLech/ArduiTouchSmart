#include <TFTWidgets.h>
#include <TFTWidgetPage.h>
#include <ArduiTouchSmart.h>
#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <stdlib.h> // for malloc and free
//void* operator new(size_t size) { return malloc(size); }
//void operator delete(void* ptr) { free(ptr); }

String newwidgetform = "["
  "{"
  "'name':'type',"
  "'label':'Widget Typ',"
  "'type':'select',"
  "'default':'0',"
  "'options':["
  "{'v':'0','l':'Nur Messwert'},"
  "{'v':'1','l':'Messgerät'},"
  "{'v':'2','l':'Farbbalken'},"
  "{'v':'3','l':'Knopf'},"
  "{'v':'4','l':'Schieber'},"
  "{'v':'5','l':'Schalter'},"
  "{'v':'6','l':'Farb Wähler'}"
  "]"
  "},"
  "{"
  "'name':'name',"
  "'label':'Name',"
  "'type':'text',"
  "'default':''"
  "},"
  "{"
  "'name':'xposition',"
  "'label':'X-Pos',"
  "'type':'number',"
  "'default':'0',"
  "'min':0,'max':10"
  "},"
  "{"
  "'name':'yposition',"
  "'label':'Y-Pos',"
  "'type':'number',"
  "'default':'0',"
  "'min':0,'max':13"
  "},"
  "{"
  "'name':'width',"
  "'label':'Breite',"
  "'type':'number',"
  "'default':'2',"
  "'min':2,'max':12"
  "},"
  "{"
  "'name':'height',"
  "'label':'Höhe',"
  "'type':'number',"
  "'default':'2',"
  "'min':2,'max':14"
  "},"
  "{"
  "'name':'background',"
  "'label':'Füllung',"
  "'type':'color',"
  "'default':'65535'"
  "},"
  "{"
  "'name':'border',"
  "'label':'Rahmen',"
  "'type':'color',"
  "'default':'0'"
  "},"
  "{"
  "'name':'fColor',"
  "'label':'Schrift',"
  "'type':'color',"
  "'default':'0'"
  "},"
  "{"
  "'name':'label',"
  "'label':'Beschriftung',"
  "'type':'text',"
  "'default':''"
  "},"
  "{"
  "'name':'topic',"
  "'label':'Thema',"
  "'type':'text',"
  "'default':''"
  "},"
  "{"
  "'name':'valname',"
  "'label':'Wert-Bez.',"
  "'type':'text',"
  "'default':'wert'"
  "},"
  "{"
  "'name':'unitname',"
  "'label':'Einheit-Bez.',"
  "'type':'text',"
  "'default':'einheit'"
  "}"
  "]";

String changepageform = "["
"{"
"'name':'xposition',"
"'label':'X-Pos',"
"'type':'number',"
"'default':'0',"
"'min':0,'max':10"
"},"
"{"
"'name':'yposition',"
"'label':'Y-Pos',"
"'type':'number',"
"'default':'0',"
"'min':0,'max':13"
"},"
"{"
"'name':'pag',"
"'label':'Zielseite',"
"'type':'select',"
"'default':'0'"
"}"
"]";

#define TOOL_BACKGROUND 0xc638

const int8_t mainmenu[12] = {
  CMD_SCREENOFF,
  CMD_PAUSE,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  CMD_LEFT,
  CMD_RIGHT
};

ArduiTouchSmart::ArduiTouchSmart(Adafruit_ILI9341 * tft, TFTForm * conf, const GFXfont * font, bool header, bool footer) {
  _tft = tft;
  _conf = conf;
  _font = font;
  _header = header;
  _footer = footer;
  addMenu(mainmenu);
}


Error ArduiTouchSmart::addPage(){
  Error err = Error::NO_ERROR;
  if (_cnt >= MAXPAGES) return Error::NO_MORE_PAGES;
  _pages[_cnt]= new(TFTWidgetPage)(_tft, _conf, _font, _header, _footer);
  _pages[_cnt]->registerOnPublish(_onPublish);
  _pages[_cnt]->drawPage();
  _currentPage = _cnt;
  _cnt++;
  if (_footer) drawFootBar();
  return err;

}

void ArduiTouchSmart::nextPage() {
  if ((_currentPage+1) < (_cnt)) {
    _currentPage++;
    _pages[_currentPage]->drawPage();
    if (_footer) drawFootBar();
  }
}

void ArduiTouchSmart::previousPage() {
  if ((_currentPage) > 0) {
    _currentPage--;
    _pages[_currentPage]->drawPage();
    if (_footer) drawFootBar();
  }
}

void ArduiTouchSmart::removePage(uint8_t index){
  if (index >= _cnt) return;
  _pages[index]->deleteAllWidgets();
  if (_cnt > 1) {
    delete(_pages[index]);
    _cnt--;
    for (uint8_t i = index; i<_cnt; i++) _pages[i] = _pages[i+1];
  }
  if (_currentPage >= _cnt) _currentPage = _cnt-1;
  _pages[_currentPage]->drawPage();
  if (_footer) drawFootBar();
}


uint8_t ArduiTouchSmart::getPageCount(){
  return _cnt;
}

uint8_t ArduiTouchSmart::getCurrentPage(){
  return _currentPage;
}

//functions for current page
void ArduiTouchSmart::drawPage(){
  if (_header) drawToolbar();
  if (_footer) drawFootBar();
  Serial.printf("Seite %i von %i \n",_currentPage,_cnt);
  if ((_cnt > 0) && (_currentPage < _cnt)) _pages[_currentPage]->drawPage();
}

Error ArduiTouchSmart::addWidget(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t type, const char name[], const char style[] ){
  if ((_cnt > 0) && (_currentPage < _cnt)) _pages[_currentPage]->addWidget(x,y,width,height,type,name,style);
}

Error ArduiTouchSmart::addWidget(const char style[] ){
  if ((_cnt > 0) && (_currentPage < _cnt)) _pages[_currentPage]->addWidget(style);
}


void ArduiTouchSmart::removeWidget(uint8_t index){
  if ((_cnt > 0) && (_currentPage < _cnt)) _pages[_currentPage]->removeWidget(index);
}
void ArduiTouchSmart::drawWidget(uint8_t index){
  if ((_cnt > 0) && (_currentPage < _cnt)) _pages[_currentPage]->drawWidget(index);
}
void ArduiTouchSmart::updateValue(uint8_t index, const char data[]){
  if ((_cnt > 0) && (_currentPage < _cnt)) _pages[_currentPage]->updateValue(index, data);
}
//for all pages!
void ArduiTouchSmart::updateTopic(char topic[], const char data[]){
  for (uint8_t i = 0; i<_cnt; i++) {
    _pages[i]->updateTopic(topic,data,((i == _currentPage) && (_currentForm == FRM_NONE)));
  }
}

int16_t ArduiTouchSmart::findTopic(char topic[]) {
  if ((_cnt > 0) && (_currentPage < _cnt)){
    return _pages[_currentPage]->findTopic(topic);
  } else {
    return -1;
  }
}

int16_t ArduiTouchSmart::getSelectedIndex() {
  if ((_cnt > 0) && (_currentPage < _cnt)){
    return _pages[_currentPage]->getSelectedIndex();
  } else {
    return -1;
  }
}

/*
bool ArduiTouchSmart::touchEvent(int16_t x, int16_t y, EV event) {
  if ((_cnt > 0) && (_currentPage < _cnt)) {
    return _pages[_currentPage]->touchEvent(x,y,event);
  } else {
    return false;
  }
}
*/


bool ArduiTouchSmart::touchEvent(int16_t x, int16_t y, EV event) {
  Serial.printf("Position: x=%i  y=%i\n",x,y);
  bool flag = false;
  uint16_t height = _tft->height();
  if (_header && (y<HEADER)){
    if(event == EV::EVT_CLICK) flag = toolbarEvent(x,y);
  } else if (_footer && (y > (height-FOOTER))) {
    if(event == EV::EVT_CLICK) flag = footbarEvent(x,y);
  } else {
    if (_confMode) {
      if (event == EV::EVT_CLICK) _pages[_currentPage]->selectWidget(x,y);
    } else {
      flag = _pages[_currentPage]->touchEvent(x,y,event);
    }
  }
  return flag;
}

bool ArduiTouchSmart::toolbarEvent(int16_t x, int16_t y) {
  bool flag = false;
  int8_t block = x/20;
  if (block < 0) block = 0;
  if (block > 11) block = 11;
  int8_t cmd = _menu[_confMode][block];
  Serial.printf("Block = %i CMD = %i \n",block,cmd);
  switch (cmd) {
    case CMD_SCREENOFF :
    case CMD_CALIBRATE :
    case CMD_WLAN :
    case CMD_INFO :
    case CMD_RESET :
    case CMD_CONDITION :
    case CMD_FOLDER:
    case CMD_LIST : if (_onExternCommand) {
       _onExternCommand(cmd);
       _currentForm = FRM_EDITEXTERN;
     }
      break;
    case CMD_DOWN :
    case CMD_CONF : if (_confMode < (_menuCnt-1)) {
        _confMode++;
        drawToolbar();
      }
      break;
    case CMD_PLAY : _confMode = 0;
      _pages[_currentPage]->selectWidget(0,0);
      drawPage();
      break;
    case CMD_UP :
    case CMD_BACK : if (_confMode > 0) {
        _confMode--;
        drawToolbar();
      }
      break;
    case CMD_PAUSE : if (_menuCnt > 1) {
        _confMode = 1;
        drawToolbar();
      }
      break;
    case CMD_EDITPAGE : _pages[_currentPage]->editWidget(true);
      _currentForm = FRM_WIDGETEDIT;
      break;
    case CMD_EDIT : _pages[_currentPage]->editWidget(false);
      _currentForm = FRM_WIDGETEDIT;
      break;
    case CMD_RIGHT : nextPage(); break;
    case CMD_LEFT : previousPage(); break;
    case CMD_PAGEPLUS : addPage(); break;
    case CMD_PAGEMINUS : _conf->showDialog("Wollen Sie die Seite wirklich löschen?");
      _currentForm = FRM_PAGEDELETE;
      break;
    case CMD_WDGPLUS : _conf->setDescription(newwidgetform);
      _conf->showForm();
      _currentForm = FRM_WIDGETNEW;
      break;
    case CMD_WDGMINUS : if (getSelectedIndex() >= 0) {
        _conf->showDialog("Wollen Sie das Widget wirklich löschen?");
        _currentForm = FRM_WIDGETDELETE;
      }
      break;
    case CMD_SAVE: _conf->showDialog("Wollen Sie die aktuelle Konfiguration speichern?");
      _currentForm = FRM_SAVEALL;
      break;
    case CMD_READ: _conf->showDialog("Wollen Sie die aktuelle Konfiguration durch Daten aus dem Konfigurationsfile ersetzen?");
      _currentForm = FRM_LOADALL;
      break;
    case CMD_EXCHANGE: showPageChangeDialog(); break;
  }
}

bool ArduiTouchSmart::footbarEvent(int16_t x, int16_t y) {
  setError("");
  bool flag = false;
  return flag;
}

uint8_t ArduiTouchSmart::getWidgetCount(){
  if ((_cnt > 0) && (_currentPage < _cnt)) {
    return _pages[_currentPage]->getWidgetCount();
  } else {
    return 0;
  }
}
const char * ArduiTouchSmart::getTopic(uint8_t index) {
  if ((_cnt > 0) && (_currentPage < _cnt)) {
    return _pages[_currentPage]->getTopic(index);
  } else {
    return "";
  }
}
void ArduiTouchSmart::registerOnPublish(bool (*callback)(const char * topic, const char * message)){
  _onPublish = callback;
}

void ArduiTouchSmart::registerOnExternCommand(void(*callback)(uint8_t button)){
  _onExternCommand = callback;
}


void ArduiTouchSmart::timeEvent(bool connected, bool showTime){
  if ((millis() - _lts) > 100) {
    _lts = millis();
    if ((_cnt > 0) && (_currentPage < _cnt)) _pages[_currentPage]->checkForPublish();
    _timcnt++;
    if (connected && _header && showTime && (_timcnt > 10)) displayTime();
  }
}

void ArduiTouchSmart::deleteAllWidgets(){
  if ((_cnt > 0) && (_currentPage < _cnt)) _pages[_currentPage]->deleteAllWidgets();
}

void ArduiTouchSmart::setFormat(bool header, bool footer) {
  if ((_cnt > 0) && (_currentPage < _cnt)) _pages[_currentPage]->setFormat(header, footer);
}
void ArduiTouchSmart::endEdit(String data){
  switch (_currentForm) {
    case FRM_WIDGETEDIT:    if ((_cnt > 0) && (_currentPage < _cnt)) {
        _pages[_currentPage]->endEdit(data);
      }
      break;
    case FRM_PAGEDELETE : if (data != "") removePage(_currentPage); break;
    case FRM_WIDGETDELETE : if (data != "") {
        int16_t index = getSelectedIndex();
        if (index >= 0) removeWidget(index);
      }
      break;
    case FRM_WIDGETNEW : if (data != ""){
        addWidget(data.c_str());
        Serial.print("Neues Wiodget: ");
        Serial.println(data);
      }
      break;
    case FRM_SAVEALL : if (data != ""){
        saveAllPages();
      }
      break;
    case FRM_LOADALL : if (data != ""){
        loadAllPages();
      }
      break;
    case FRM_CHANGEPAGE : if (data != ""){
        changePage(data);
      }
      break;

  }
  drawPage();
  _currentForm = FRM_NONE;
}

void ArduiTouchSmart::drawToolbar() {
  int index;
  for (uint8_t i = 0; i<12; i++) {
    index = _menu[_confMode][i];
    if (index < 0){
      _tft->fillRect(i*20,0,20,20,TOOL_BACKGROUND);
    } else {
      _tft->drawRGBBitmap(i*20,0,icons[index],20,20);
    }
  }
}

void ArduiTouchSmart::drawFootBar(){
  uint16_t h = _tft->height();
  uint16_t w = _tft->width();
  _tft->fillRect(0,h-FOOTER,w,FOOTER,TOOL_BACKGROUND);
  _tft->setFont(_font);
  _tft->setCursor(w-40,h-4);
  _tft->setTextColor(ILI9341_BLACK);
  _tft->setTextSize(1);
  _tft->printf("%i/%i",_currentPage+1,_cnt);
  if (strlen(_error)>0){
    _tft->setCursor(4,h-4);
    _tft->print(_error);
  }
}

void ArduiTouchSmart::updatePages() {
  for (uint8_t i = 0; i<_cnt; i++) {
    _pages[i]->setFormat(_header, _footer);
    _pages[i]->registerOnPublish(_onPublish);
  }
  drawPage();
}

uint8_t ArduiTouchSmart::savePage(uint8_t pagenum) {
  if (pagenum < _cnt) {
    return _pages[pagenum]->savePage(pagenum);
  } else {
    return -1;
  }
}

uint8_t ArduiTouchSmart::saveAllPages() {
  //first remove all old data
  File dir,f;
  dir = SPIFFS.open("/wdgconf","r");
  if (dir){
    f=dir.openNextFile();
    while (f) {
      SPIFFS.remove(f.name());
      f=dir.openNextFile();
    }
  }
  for (uint8_t i = 0 ; i<_cnt; i++) savePage(i);

}

uint8_t ArduiTouchSmart::loadAllPages() {
  //first remove all old data
  for (uint8_t i = 0; i<_cnt; i++) removePage(i);
  File dir,f;
  uint8_t page,widget;
  String fName, props;
  dir = SPIFFS.open("/wdgconf","r");
  if (dir) {
    f = dir.openNextFile();
    while (f) {
      fName = f.name();
      sscanf(fName.c_str(),"/wdgconf/p %i /w %i",&page,&widget);
      Serial.printf("Seite %i Widget % i\n",page,widget);
      props = f.readString();
      Serial.print("Props:");
      Serial.println(props);
      if (page > (_cnt-1)) addPage();
      _pages[_currentPage]->addWidget(props.c_str());
      f = dir.openNextFile();
    }
    _currentPage = 0;
    drawPage();
  }
}

void ArduiTouchSmart::showPageChangeDialog(){
  int16_t widget = getSelectedIndex();
  char buf[30];
  if (widget < 0) return;
  _conf->setDescription(changepageform);
  _conf->setValues(_pages[_currentPage]->getGeometrie(widget));
  _conf->clearOptions("pag");
  uint8_t ix = _conf->findName("pag");
  for (uint8_t i = 0;i<_cnt;i++){
    sprintf(buf,"%i=Seite %i",i,i+1);
    _conf->addOption(ix,String(buf));
  }
  _conf->showForm();
  _currentForm = FRM_CHANGEPAGE;
}

void ArduiTouchSmart::changePage(String data){
  StaticJsonDocument<300> doc;
  deserializeJson(doc,data);
  uint8_t ziel = doc["pag"];
  if ((ziel != _currentPage) && (_pages[ziel]->getWidgetCount()<MAXWIDGETS)) {
    int16_t ix = _pages[_currentPage]->getSelectedIndex();
    if (ix >= 0) {
      Widget * w = _pages[_currentPage]->releaseWidget(ix);
      _pages[ziel]->appendWidget(w);
      w->setProperties(data);
      _currentPage = ziel;
    }
  }
}

void ArduiTouchSmart::displayTime() {
  _timcnt = 0;
  if (_confMode == 0) {
    char sttime[50];
    struct tm timeinfo;
    if(getLocalTime(&timeinfo)){
      strftime(sttime, sizeof(sttime), _timeFormat, &timeinfo);
      _tft->setFont(_font);
      _tft->setTextColor(ILI9341_BLACK);
      _tft->setTextSize(1);
      _tft->setCursor(60,16);
      _tft->fillRect(42,2,156,18,TOOL_BACKGROUND);
      _tft->print(sttime);
    }
  }
}

void ArduiTouchSmart::setError(const char error[]) {
  strlcpy(_error,error,24);
  if (_footer) drawFootBar();
}

void ArduiTouchSmart::addMenu(const int8_t menu[12]){
  if (_menuCnt < MAXMENUS) {
    for (uint8_t i = 0;i<12;i++) _menu[_menuCnt][i] = menu[i];
    _menuCnt++;
  }
}

void ArduiTouchSmart::setEditForm(uint8_t form) {
  _currentForm = form;
}
