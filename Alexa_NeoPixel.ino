
#include <Adafruit_NeoPixel.h>
    
#define DATA_PIN 13
#define SIZE 144 // You can use number of LEDs, but Echo Dot has only 12 LEDs, so the code performs better with multiples of 12 (24,36,48,...)

const int DURATION = 1000; // animation duration

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

// states already implemented
enum states 
{
  START_LISTENING,
  LISTENING,
  END_LISTENING,
} state;


void listening_start(int width=ceil(SIZE/12.0))
{
  if (millis() - last_increment > STEP_SIZE) 
  {
    last_increment = millis();
    if (counter>=(SIZE/2))
    {
      state = LISTENING;
      counter = (SIZE/2)-width;
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
}

void listening_active()
{
  delay(3000);
  state = END_LISTENING;
  return;
}

void listening_end(int width=ceil(SIZE/12.0))
{
  if (millis() - last_increment > STEP_SIZE) 
  {
    last_increment = millis();

    if (counter<0)
    {
      delay(2000);
      state = START_LISTENING;
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
}

void thinking()
{
  if (millis() - last_increment > STEP_SIZE) 
  {
    last_increment = millis();
    if (counter>=(SIZE/2))
    {
      state = LISTENING;
      counter = (SIZE/2)-width;
      return;
    }
}

void speaking(){}

void end_speaking(){}

void Microphone_on_to_off(){}

void microphone_off(){}

void Microphone_off_to_on(){}

void timer(){}

void timer_short(){}

void alarm(){}

void alarm_short(){}

void reminder(){}

void reminder_short(){}

void incoming_notification(){}

void queued_notification(){}

void enable_do_not_disturb(){}

void do_not_disturb_enabled(){}

void disable_do_not_disturb(){}

void bluetooth_connected(){}

void bluetooth_disconnected(){}

void error(){}

/*** THE REAL STUFF HAPPEN HERE ***/

void setup() 
{
  ring.begin();
  ring.show();
  ring.setBrightness(200);
  state = START_LISTENING;
}

void loop() 
{   
  switch (state) 
  {
    case START_LISTENING: listening_start();   break;
    case LISTENING:       listening_active();         break;
    case END_LISTENING:   listening_end();     break;
    default:              listening_active();         break;
  }
  ring.show();
}