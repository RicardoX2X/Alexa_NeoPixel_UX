#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
    
#define DATA_PIN 12
#define SIZE 144 // You can use number of LEDs, but Echo Dot has only 12 LEDs, so the code performs better with multiples of 12 (24,36,48,...)

int BRIGHT = 200;

const int DURATION = 7000; // animation duration

const int STEP_SIZE = DURATION/SIZE;

Adafruit_NeoPixel ring = Adafruit_NeoPixel(SIZE, DATA_PIN, NEO_RGB + NEO_KHZ800);

// define all the colors used by Amazon for the Alexa UX as describe on README text
const uint32_t CYAN = ring.Color(0, 255, 255);
const uint32_t BLUE = ring.Color(0, 0, 255);
const uint32_t RED = ring.Color(255, 0, 0);
const uint32_t GREEN = ring.Color(0, 255, 0);
const uint32_t YELLOW = ring.Color(255, 255, 0);
const uint32_t PURPLE = ring.Color(153, 0, 255);
const uint32_t WHITE = ring.Color(255, 255, 255);
const uint32_t ORANGE = ring.Color(255, 153, 0);
const uint32_t BLANK = ring.Color(0, 0, 0);

int counter = 0;  //step counter, for the led animation
unsigned long last_increment = millis(); // time counting
int out_flag = 0;
bool up = 1;
// states already implemented
enum states 
{
  START_LISTENING,
  LISTENING,
  THINKING,
  SPEAKING,
  END_LISTENING,
} state;

bool isListening = true;
unsigned long pixelPrevious = 0;        // Previous Pixel Millis
unsigned long patternPrevious = 0;      // Previous Pattern Millis
int           patternCurrent = 0;       // Current Pattern Number
int           patternInterval = 5000;   // Pattern Interval (ms)
bool          patternComplete = false;


void listening_start(int width=ceil(SIZE/12.0))
{
  //if (!isListening) return;
  if (counter>=(SIZE/2))
  {
    counter = 0;
    isListening = true;
    patternComplete = true;
    return;
  }

  for (int i=counter; i<counter+width ; i++) 
  {
    if(i>=width)
    {
      ring.setPixelColor(i - width, BLUE); // rotate clockwise
      ring.setPixelColor(SIZE-1 - i + width, BLUE); // rotate anticlockwise
    }
    ring.setPixelColor(i, CYAN); // rotate clockwise
    ring.setPixelColor(SIZE-1 - i, CYAN); // rotate anticlockwise
  }
  counter = counter+width;


}

void listening_end(int width=ceil(SIZE/12.0))
{

  if (patternComplete == false && isListening == true)
  {
    counter = (SIZE/2)-width;
    isListening = false;
  }

  if (counter<0)
  {
    patternComplete = true;
    return;
  }
  
  for (int i=counter; i<counter+width ; i++) 
  {
    if(counter<=(SIZE/2)-width)
    {
      ring.setPixelColor(i, BLANK); // rotate clockwise
      ring.setPixelColor(SIZE-1 - i, BLANK); // rotate anticlockwise
    }
    ring.setPixelColor(i - width, CYAN); // rotate clockwise
    ring.setPixelColor(SIZE-1 - i + width, CYAN); // rotate anticlockwise
  }
  counter = counter-width;
}


void listening_active()
{
  delay(1000);
  patternComplete =true;
  return;
}

void thinking(int width=ceil(SIZE/12.0))
{
  if (counter>=SIZE)
  {
    counter = 0;
    out_flag++;
    if (out_flag>3)
    {
      patternComplete =true;
      out_flag=0;
      return;
    }
  }
  ring.fill(BLUE);
  for (int i=counter; i<counter+(2*width); i++) 
  {
    ring.setPixelColor(i, CYAN);
    ring.setPixelColor(i-(4*width), CYAN);
    ring.setPixelColor(i-(8*width), CYAN);
    ring.setPixelColor(i-(12*width), CYAN);
    ring.setPixelColor(i+(4*width), CYAN);
    ring.setPixelColor(i+(8*width), CYAN);
  }
  counter = counter+width;
}

void pulse(uint32_t c1, uint32_t c2,int width=ceil(SIZE/12.0)) 
{
  
  uint8_t r = (c1>>16 & 0xFF);
  uint8_t g = (c1>>8  & 0xFF);
  uint8_t b = (c1     & 0xFF);

  //uint8_t r2 = (c2>>16 & 0xFF);
  uint8_t g2 = (c2>>8  & 0xFF);
  //uint8_t b2 = (c2     & 0xFF);

  uint8_t gr;
  uint32_t colour;

  if(up==true)
  {
    counter=counter+15;
  }
  if(up==false)
  {
    counter=counter-15;
  }

  if (counter>=g2-g||counter<=0)
  {
    //counter = 0;
    up = !up;
    out_flag++;
    //return;
  }
  if (out_flag>6)
  {
    counter = (SIZE/2)-width;
    out_flag=0;
    patternComplete = true;
    return;
  }
  gr = g + counter;
  colour = ((uint32_t)r << 16) | ((uint32_t)gr << 8) | b;
  ring.fill(colour);
  
}

void speaking()
{
  pulse(BLUE,CYAN);
}

void setup() 
{
  Serial.begin(9600);
  ring.begin();
  ring.show();
  ring.setBrightness(BRIGHT);
  state = START_LISTENING;
}

void loop() 
{   
  if (millis() - last_increment > STEP_SIZE) 
  {
    last_increment = millis();

    switch (state) 
    {
      case START_LISTENING: listening_start();   break;
      case LISTENING:       listening_active();         break;
      case END_LISTENING:   listening_end();     break;
      case THINKING:        thinking();           break;
      case SPEAKING:        speaking();           break;
      default:              listening_active();         break;
    }
    ring.show();
  }
  unsigned long currentMillis = millis();                     //  Update current time
  if( patternComplete || (currentMillis - patternPrevious) >= patternInterval) {  //  Check for expired time
    patternComplete = false;
    patternPrevious = currentMillis;
    if (state == END_LISTENING)
      state = START_LISTENING; // Reset
    else
      state = static_cast<states>(state + 1);
  }
  
}