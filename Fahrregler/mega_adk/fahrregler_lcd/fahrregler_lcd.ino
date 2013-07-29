//S65-Shield, Motor-Shield

#include <S65Display.h>
#include <RotaryEncoder.h>

#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

#define bg RGB(255, 0, 9)
#define fg  RGB(255, 255,0)
#define width 176
#define height 132
#define twoAmp ((1023 * 33) / 50)

AndroidAccessory acc("annualweekendcoding",
		     "Fahrregler",
		     "Fahrregler Arduino Mega ADK",
		     "1.0",
		     "https://github.com/annualweekendcoding",
		     "0000000012345678");

S65Display lcd;
RotaryEncoder encoder;
char charBuffer[10];
int direction = LOW;
int speed = 0;

ISR(TIMER2_OVF_vect)
{
  TCNT2 -= 250; //1000 Hz
  
  encoder.service();
}


void drawText(void)
{
  lcd.drawTextPGM(46,  70, PSTR("Richtung:"), 1, fg, bg);
  lcd.drawTextPGM(46,  84, PSTR("Strom:"), 1, fg, bg);
  lcd.drawTextPGM(46,  98, PSTR("Stufe:"), 1, fg, bg);

  //Umlaut test text (remove FONT_END7F define)
  // lcd.drawTextPGM(10, 10, PSTR("Test \x84 \x94 \x81 \x8E \x99 \x9A"), 1, RGB(  0,255,  0), RGB(255,  0,  0));
  // lcd.drawText   (10, 25,      "Test \x84 \x94 \x81 \x8E \x99 \x9A",  1, RGB(  0,255,  0), RGB(255,  0,  0));
}


void drawValue(int line, int value)
{
  const int y = 70 + line * 14;
  String s = String(value);
  s.toCharArray(charBuffer, 10);
  lcd.fillRect(126 + (s.length() * 8), y, 175, y + 11, bg);
  lcd.drawText(126, y, charBuffer, 1, fg, bg);
}


void correctSpeed()
{
  if (speed <= 0)
  {
    speed = 0;
  }
  else
  {
      if (speed >= 255)
      {
        speed = 255;
      }
  }
}


void setup()
{
  Serial.begin(9600);
  Serial.print("\r\nStart");
  
  //init LCD
  lcd.init(4); //spi-clk = Fcpu/4

  //init Rotary Encoder
  encoder.init();

  //init Timer2
  TCCR2B  = (1<<CS22); //clk=F_CPU/64
  TCNT2   = 0x00;
  TIMSK2 |= (1<<TOIE2); //enable overflow interupt

  //interrupts on
  sei();

  //clear screen
  lcd.clear(bg);
  
  drawText();

  //Setup Channel A
  pinMode(12, OUTPUT); //Initiates Motor Channel A pin
  pinMode(9, OUTPUT); //Initiates Brake Channel A pin
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);

  //forward @ but stop
  analogWrite(3, speed);   //Spins the motor on Channel A at stop
  digitalWrite(12, direction); //Establishes forward direction of Channel A
  digitalWrite(9, LOW);   //No Brake for Channel A
  
  drawValue(0, direction);
  drawValue(2, speed);
  
  acc.powerOn();
}


void loop()
{
  int8_t move, press;
  
  int current = (analogRead(0) + 5) / 10;
//  Serial.print("Current: ");
//  Serial.println(current);
  
  drawValue(1, current);
  
  move  = encoder.step();
  press = encoder.sw();
  
  if (move || press)
  {
    int oldspeed = speed;
    if (press == SW_PRESSED || press == SW_PRESSEDLONG)
    {
      speed = 0;
      direction = (direction == HIGH) ? LOW : HIGH;
      analogWrite(3, 0);
      Serial.print("Direction is ");
      Serial.println(direction);
      digitalWrite(12, direction);
      drawValue(0, direction);
    }
    else if (move)
    {
      speed += move * 10;
      correctSpeed();
      analogWrite(3, speed);
    }
      
    Serial.print("Speed is ");
    Serial.println(speed);
    drawValue(2, speed);
    
    if (oldspeed > speed)
    {
      lcd.fillRect(0, 4 + 128 - (oldspeed >> 1), 36,  4 + 128 - 1 - (speed >> 1), bg); //clear last position
    }
    
/*    
    if (speed == 0)
    {
      digitalWrite(9, HIGH);   //Brake for Channel A
    }
    else if (oldspeed == 0)
    {
      digitalWrite(9, LOW);   //No Brake for Channel A
    } 
*/    

    lcd.fillRect(0, 4 + 128 - (speed >> 1), 36, 131, fg);
  }
  
  byte msg[3];
  if (acc.isConnected())
  {
    int len = acc.read(msg, sizeof(msg), 1);
    if (len > 0) 
    {
      Serial.println("Got USB packet");
      // assumes only one command per packet
      if (msg[0] == 0x0) 
      {
        Serial.println("USB 0");
        int oldspeed = speed;
        if (msg[1] == 0x0) 
        {
          Serial.println("USB 0, 0: faster");
          speed += 10;
          correctSpeed();
          analogWrite(3, speed);
        }
        else if (msg[1] == 0x1) 
        {
          Serial.println("USB 0, 1: slower");
          speed -= 10;
          correctSpeed();
          analogWrite(3, speed);
        }
        else if (msg[1] == 0x2) 
        {
          Serial.println("USB 0, 2: reverse direction");
          speed = 0;
          analogWrite(3, 0);
          direction = (direction == HIGH) ? LOW : HIGH;
          Serial.print("Direction is ");
          Serial.println(direction);
          digitalWrite(12, direction);
          drawValue(0, direction);
        }
        else if (msg[1] == 0x3) 
        {
          Serial.println("USB 0, 3: halt");
          speed = 0;
          analogWrite(3, speed);
        }
        
        Serial.print("Speed is ");
        Serial.println(speed);
        drawValue(2, speed);
        if (oldspeed > speed)
        {
          lcd.fillRect(0, 4 + 128 - (oldspeed >> 1), 36,  4 + 128 - 1 - (speed >> 1), bg); //clear last position
        }
        lcd.fillRect(0, 4 + 128 - (speed >> 1), 36, 131, fg);
      }
    }
  }
}
