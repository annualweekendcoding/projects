#include <SPI.h>
#include "Adafruit_GFX.h" //Grafik Bibliothek
#include "Adafruit_ILI9341.h" // Display Treiber
#include <XPT2046_Touchscreen.h> // Touchscreen Treiber

//Verwendete Schriften
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans24pt7b.h>

#define _debug 0 //Anzeige von Meldunge am Seriellen Monitor

//Verwendete Pins am Display
#define TFT_CS   5
#define TFT_DC   4
#define TFT_MOSI 23
#define TFT_CLK  18
#define TFT_RST  22
#define TFT_MISO 19
#define TFT_LED  15
#define TFT_LED_CHANNEL  3

#define HAVE_TOUCHPAD
#define TOUCH_CS 14
#define TOUCH_IRQ 13

// Parameter für Touchscreen
#define ILI9341_ULTRA_DARKGREY    0x632C
#define MINPRESSURE 10
#define MAXPRESSURE 2000
//Messbereich muss eventuell kalibriert werden
#define TS_MINX 230
#define TS_MINY 350
#define TS_MAXX 3700
#define TS_MAXY 3900

//Treiber Instanzen
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

//aktuelle Positionen
int tsx, tsy, tsxraw, tsyraw;
//aktueller Touch Zustand
bool tsdown = false;

const uint8_t tftLedLow = 10;
const uint8_t tftLedHigh = 255;
uint8_t tftLedLevel = tftLedLow;
unsigned long tftLedHighTime = 0;

//Vorbereitung
void displaySetup() {
  pinMode(TFT_LED, OUTPUT);
  ledcSetup(TFT_LED_CHANNEL, 1000, 8);
  ledcAttachPin(TFT_LED, TFT_LED_CHANNEL);
  ledcWrite(TFT_LED_CHANNEL, tftLedLevel); // Display-Beleuchrung abgedunkelt einschalten
  //Treiber starten
  tft.begin();
  touch.begin();
#ifdef _debug
  //Auflösung des Displays
  Serial.print("tftx ="); Serial.print(tft.width()); Serial.print(" tfty ="); Serial.println(tft.height());
#endif
  //aktuelle Werte zurücksetzen
  tsx = 0;
  tsy = 0;
  tsxraw = 0;
  tsyraw = 0;
  tsdown = false;
  //Anzeigen
  draw_screen();
}

unsigned long ledLastTime = 0;

void displayLoop() {
  //auf Berührung reagieren
  handleTouch();
  if (ledLastTime == 0 || millis() - ledLastTime >= 1000)
  {
    ledLastTime = millis();
    // Uhrzeit ausgeben
    draw_footer();
  }
  if (tsdown) {
    // Beim Antippen Beleuchtung hell einschalten
    tftLedLevel = tftLedHigh;
    tftLedHighTime = millis();
    ledcWrite(TFT_LED_CHANNEL, tftLedLevel);
  }
  if (tftLedLevel > tftLedLow && millis() - tftLedHighTime > 30000) {
    // Display-Beleuchtung runterdimmen
    tftLedLevel--;
    ledcWrite(TFT_LED_CHANNEL, tftLedLevel);
  }
}

//aktuelle Position und Berührungszustand
//vom Touchscreen ermitteln
void handleTouch() {
  TS_Point p;
  p = touch.getPoint(); //aktuelle Daten lesen
  tsxraw = p.x; //x und y als Rohwerte merken
  tsyraw = p.y;
  delay(1);
  tsx = map(tsyraw, TS_MINY, TS_MAXY, 240, 0);
  tsy = map(tsxraw, TS_MINX, TS_MAXX, 0, 320);
  //Berührungszustand ermitteln
  if ((p.z > MINPRESSURE) && (!tsdown)) {
    // Es wurde gerade gedrückt
  }
  tsdown = (p.z > MINPRESSURE);
}

//Hauptbildschirm anzeigen
void draw_screen() {
  draw_footer();
}

void draw_koord() {
  uint16_t w, h;
  //Ausrichtung Farben und Schrift auswählen
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setFont(&FreeSans9pt7b);
  w = tft.width();
  h = tft.height();
  //grünes Rechteck in der Mitte zeichnen
  drawPosition2(tsxraw, tsyraw, tsx, tsy, tsdown);
  tft.fillRect(w / 2 - 20, h / 2 - 10, 40, 20, ILI9341_GREEN);
  //Pfeile zu den Eckpunkten anzeigen
  tft.drawLine(0, 0, 20, 0, ILI9341_WHITE);
  tft.drawLine(0, 0, 0, 20, ILI9341_WHITE);
  tft.drawLine(0, 0, 40, 40, ILI9341_WHITE);
  tft.drawLine(w - 1, 0, w - 20, 0, ILI9341_WHITE);
  tft.drawLine(w - 1, 0, w - 1, 20, ILI9341_WHITE);
  tft.drawLine(w - 1, 0, w - 40, 40, ILI9341_WHITE);
  tft.drawLine(w - 1, h - 1, w - 40, h, ILI9341_WHITE);
  tft.drawLine(w - 1, h - 1, w, h - 40, ILI9341_WHITE);
  tft.drawLine(w - 1, h - 1, w - 40, h - 40, ILI9341_WHITE);
  tft.drawLine(0, h - 1, 20, h - 1, ILI9341_WHITE);
  tft.drawLine(0, h - 1, 0, h - 20, ILI9341_WHITE);
  tft.drawLine(0, h - 1, 40, h - 40, ILI9341_WHITE);
}

struct tm oldt;
uint16_t xh = 0, xm = 0, xs = 0, hhms = 0, whms = 0;
const uint16_t yhms = 299;

void printhms(int newval, int oldval, uint16_t x) {
  if (newval != oldval) {
    char s[3];
    snprintf(s, 3, "%02d", newval);
    tft.setCursor(x, yhms);
    tft.fillRect(x, yhms - hhms, whms + 1, hhms + 2, ILI9341_WHITE);
    tft.print(s);
  }
}

void draw_footer() {
  int16_t  x1, y1;
  uint16_t w, h;
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
  tft.setFont(&FreeSans24pt7b);
  if (xs == 0) {
    oldt.tm_hour = 25;
    oldt.tm_min = 60;
    oldt.tm_sec = 60;
    oldt.tm_mday = 0;
    tft.fillRect(0, 260, 240, 60, ILI9341_WHITE);
    // Breite und Höhe der Ziffern ermitteln
    /*
       höchste Breite experimentell ermitteln
      for (char c='0'; c<='9'; c++) {
      char s[2]; s[0]=c; s[1]=0;
      tft.getTextBounds(s, 0, 0, &x1, &y1, &w, &h);
      Serial.print(s); Serial.print(" "); Serial.println(w);
      }
    */
    // Die 4 und die 3 sind die breitesten Zeichen
    tft.getTextBounds("44", 0, 0, &x1, &y1, &whms, &hhms);
    // maximale Größe ermitteln
    tft.getTextBounds("44:44:44", 0, 0, &x1, &y1, &w, &h);
    xh = 120 - w / 2; // Position der Stunde
    xm = 120 - whms / 2; // Position der Minute
    xs = 120 + w / 2 - whms; // Position der Sekunde
    // Doppelpunkte ausgeben
    uint16_t dw;
    tft.getTextBounds(":", 0, 0, &x1, &y1, &dw, &h);
    tft.setCursor(120 - w / 6 - dw / 2 - 2, yhms);
    tft.print(":");
    tft.setCursor(120 + w / 6 - dw / 2, yhms);
    tft.print(":");
  }
  struct tm t;
  if (getLocalTime(&t)) {
    printhms(t.tm_hour, oldt.tm_hour, xh);
    printhms(t.tm_min, oldt.tm_min, xm);
    printhms(t.tm_sec, oldt.tm_sec, xs);
    if (t.tm_mday != oldt.tm_mday)
    {
      // Datum nur neu zeichnen, wenn sich der Tag ändert
      char s[11];
      snprintf(s, 11, "%02d.%02d.%04d", t.tm_mday, t.tm_mon+1, t.tm_year + 1900);
      tft.setFont(&FreeSans9pt7b);
      tft.getTextBounds(s, 0, 0, &x1, &y1, &w, &h);
      tft.setCursor(120 - w / 2, 317);
      tft.fillRect(0, 301, 240, 19, ILI9341_WHITE);
      tft.print(s);
    }
    oldt = t; // Zeit für Vergleich merken
  }
  else {
    tft.fillRect(0, 260, 240, 60, ILI9341_WHITE);
  }
}

//Messwerte für Querformat anzeigen
void drawPosition1(uint16_t xraw, uint16_t yraw, uint16_t x, uint16_t y, bool down) {
  tft.setCursor(20, 60);
  tft.print("X = ");
  display_right(110, 60, String(x));
  tft.setCursor(180, 60);
  tft.print("Y = ");
  display_right(270, 60, String(y));

  tft.setCursor(20, 180);
  tft.print("Xraw = ");
  display_right(120, 180, String(xraw));
  tft.setCursor(180, 180);
  tft.print("Yraw = ");
  display_right(280, 180, String(yraw));
  if (down) tft.fillCircle(160, 160, 10, ILI9341_RED); else tft.fillCircle(160, 160, 10, ILI9341_YELLOW);
}

//Messwerte für Hochformat anzeigen
void drawPosition2(uint16_t xraw, uint16_t yraw, uint16_t x, uint16_t y, bool down) {
  tft.setCursor(20, 60);
  tft.print("X = ");
  display_right(110, 60, String(x));
  tft.setCursor(20, 100);
  tft.print("Y = ");
  display_right(110, 100, String(y));

  tft.setCursor(20, 240);
  tft.print("Xraw = ");
  display_right(120, 240, String(xraw));
  tft.setCursor(20, 280);
  tft.print("Yraw = ");
  display_right(120, 280, String(yraw));
  if (down) tft.fillCircle(120, 200, 10, ILI9341_RED); else tft.fillCircle(120, 200, 10, ILI9341_YELLOW);
}

//Eine Zahl rechtsbündig ausgeben
void display_right(int x, int y, String val) {
  int16_t x1, y1;
  uint16_t w, h;
  int str_len =  val.length() + 1;
  char char_array[str_len];
  val.toCharArray(char_array, str_len);
  tft.getTextBounds(char_array, x, y, &x1, &y1, &w, &h);
  tft.setCursor(x - w, y);
  tft.print(char_array);
}
