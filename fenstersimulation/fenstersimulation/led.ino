class TLed {
  public: 
  // the number of the LED pin
  const int bluePin = 25;  // GPIO25
  const int redPin = 26;  // GPIO26
  const int greenPin = 27;  // GPIO27
  
  // setting PWM properties
  const int freq = 1000;
  const int blueChannel = 0;
  const int redChannel = 1;
  const int greenChannel = 2;
  const int resolution = 8; // 8 Bit
  const int interval = 100; // ms
   
  void begin(){
    // configure LED PWM functionalitites
    // attach the channel to the GPIO to be controlled
    // set to 255 because output is low-active
    ledcSetup(blueChannel, freq, resolution);
    ledcAttachPin(bluePin, blueChannel);
    ledcWrite(blueChannel, 255);
    ledcSetup(redChannel, freq, resolution);
    ledcAttachPin(redPin, redChannel);
    ledcWrite(redChannel, 255);
    ledcSetup(greenChannel, freq, resolution);
    ledcAttachPin(greenPin, greenChannel);
    ledcWrite(greenChannel, 255);
  }
  
  int channel = blueChannel;
  int level = 0;
  int d = 1;
  
  void run() {
    static unsigned long lastTime = 0;
    if (lastTime!=0 && millis()-lastTime<interval) return; 
    lastTime = millis(); 
    level = getSunlevel();
    ledcWrite(redChannel, 256-level);
    if (level>0) {
      int blueLevel = pow(level,3)/pow(255,3)*255;
      ledcWrite(blueChannel, 256-blueLevel);
      // Grün steigt schneller als blau
      int greenLevel = pow(level,2)/pow(255,2)*255;
      ledcWrite(greenChannel, 256-greenLevel);
    }
    else {
      ledcWrite(blueChannel, 256-level);
      ledcWrite(greenChannel, 256-level);
    }
/*   
    level+=d;
    if ((d>0 && level>255) || (d<0 && level<0))
    {
      d = -d;
      level+=d;
      ledcWrite(channel, 255-level);
      if (d>0) {
        if (channel==blueChannel) channel=redChannel; 
        else if (channel==redChannel) channel=greenChannel; 
        else if (channel==greenChannel) channel=blueChannel; 
        ledcWrite(channel, 255-level);
      }
    }  
    ledcWrite(channel, 255-level);
*/  
  }
};

// global Variable
TLed led;

void ledSetup() { led.begin(); }
void ledLoop() { led.run(); }
