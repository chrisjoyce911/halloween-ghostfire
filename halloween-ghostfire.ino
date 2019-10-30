// Halloween Ghost Fire
//
// When a Ghost turns Red you can shoot it
// The more you hit the shorter the time you have to shoot the rest
//
// The game ends when you run out of time to shoot the Ghosts

#include "StensTimer.h"
/* stensTimer variable to be used later in the code */
StensTimer *stensTimer;

/* define some custom Action codes */
#define ROUND_TIMER 1
#define GAME_TIMER 2

#define ROUND_TIME 5000
#define GAME_TIME 60000
#define LAST_ROUND 10

#include "FastLED.h"
// Number of RGB LEDs in the strand
#define NUM_LEDS 50

// Define the array of leds
CRGB pixels[NUM_LEDS];
// Arduino pin used for Data
#define DATA_PIN 6

#include <AceButton.h>
using namespace ace_button;

// The number of ghosts in the game
const uint16_t NUM_GHOSTS = 5;

// Helper struct that keeps track of the button, pixel and status.
struct Ghost
{
  // the pin use to detect if the ghost has been shot
  const uint8_t buttonPin;
  // pixel use to disply the ghost
  const uint8_t pixel;
  // can this ghost be shot 'its a poltergeist' with you
  bool poltergeist;
  // only an alive ghost in left in the game
  bool isalive;
};

Ghost GHOSTS[NUM_GHOSTS] = {
    {8, 0, false, true},
    {9, 2, false, true},
    {10, 4, false, true},
    {11, 6, false, true},
    {12, 8, false, true},
};

AceButton buttons[12];

void buttonEvent(AceButton *, uint8_t, uint8_t);
#define START_PIN 7
#define START_BUTTON 10

// the round of the game you are in
int gameRound;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // wait for serial attach

  FastLED.addLeds<WS2812, DATA_PIN, RGB>(pixels, NUM_LEDS);

  // Setup start button
  pinMode(START_PIN, INPUT_PULLUP);
  buttons[START_BUTTON].init(START_PIN, HIGH, START_BUTTON);

  // Set default order
  for (uint8_t i = 0; i < NUM_GHOSTS; i++)
  {
    // Button uses the built-in pull up register.
    pinMode(GHOSTS[i].buttonPin, INPUT_PULLUP);

    // initialize the corresponding AceButton
    buttons[i].init(GHOSTS[i].buttonPin, HIGH, i);
  }

  ButtonConfig *buttonConfig = ButtonConfig::getSystemButtonConfig();
  buttonConfig->setEventHandler(buttonCallback);

  /* Save instance of StensTimer to the tensTimer variable*/
  stensTimer = StensTimer::getInstance();

  /* Tell StensTimer which callback function to use */
  stensTimer->setStaticCallback(timerCallback);

  newGame();
}

void loop()
{

  /* let StensTimer do it's magic every time loop() is executed */
  stensTimer->run();

  // Check to see if a Ghost was hit (button released)
  for (uint8_t i = 0; i < NUM_GHOSTS; i++)
  {
    buttons[i].check();
  }

  int incomingByte = 0;
  if (Serial.available() > 0)
  {
    // read the incoming byte from serial:
    incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);

    // if (input == "NG")
    // {
    //   Serial.print("SG");
    //   newGame();
    // }
  }
}

// The buttonCallback handler
void buttonCallback(AceButton *button, uint8_t eventType, uint8_t buttonState)
{

  // Get the button
  uint8_t id = button->getId();

  Serial.print("Button : ");
  Serial.println(id);

  switch (eventType)
  {
  case AceButton::kEventReleased:
    if (id == START_BUTTON)
    {
      Serial.println("New Gane");

      newGame();
    }
    else
    {
      ghostshot(id);
    }
    break;
  }
}

// this function will be called whenever a timer is due
void timerCallback(Timer *timer)
{
  /* check if the timer is one we expect */
  if (ROUND_TIMER == timer->getAction())
  {
    endRound();
  }
  if (GAME_TIMER == timer->getAction())
  {
    endGame();
  }
}

// Start a new game, reset rounds and ghosts
void newGame()
{
  for (uint8_t i = 0; i < NUM_GHOSTS; i++)
  {
    GHOSTS[i].isalive = true;
    GHOSTS[i].poltergeist = false;
  }
  gameRound = 0;
  stensTimer->setTimer(GAME_TIMER, GAME_TIME);
  newRound();
  return;
}

// Start a new round if the game, picks poltergeist and increments round
void newRound()
{
  Serial.println("R");
  // Increment round
  gameRound++;

  // pick the poltergeist/orbs
  uint8_t g = 0;
  for (uint8_t i = 0; i < NUM_GHOSTS; i++)
  {
    if (GHOSTS[i].isalive)
    {
      g++;
    }
  }
  randomSeed(analogRead(A0));
  uint8_t p = random(0, g);
  for (uint8_t i = 0; i < NUM_GHOSTS; i++)
  {
    if ((GHOSTS[i].isalive) && (i == p))
    {
      GHOSTS[i].poltergeist = true;
      pixels[GHOSTS[i].pixel] = CRGB::Red;
      Serial.print("poltergeist :");
      Serial.println(i);
      Serial.print(" button :");
      Serial.println(GHOSTS[i].buttonPin);
    }
    else if (GHOSTS[i].isalive)
    {
      GHOSTS[i].poltergeist = false;
      pixels[GHOSTS[i].pixel] = CRGB::Green;
    }
  }
  FastLED.show();
  // start the round timer
  stensTimer->setTimer(ROUND_TIMER, ROUND_TIME);
  return;
}

// End the round if ist the last end the game
void endRound()
{
  // Stop the timmer
  stensTimer->deleteTimer(ROUND_TIMER);

  // turns the pixels off
  for (uint8_t i = 0; i < NUM_GHOSTS; i++)
  {
    pixels[GHOSTS[i].pixel] = CRGB::Black;
  }
  FastLED.show();

  if (gameRound > LAST_ROUND)
  {
    endGame();
  }
  else
  {
    newRound();
  }
  return;
}

// End the game
void endGame()
{
  // Stop the ROUND_TIMER
  stensTimer->deleteTimer(ROUND_TIMER);
  // Stop the GAME_TIMER
  stensTimer->deleteTimer(GAME_TIMER);

  // turns the pixels off
  for (uint8_t i = 0; i < NUM_GHOSTS; i++)
  {
    pixels[GHOSTS[i].pixel] = CRGB::Black;
  }
  FastLED.show();

  Serial.println("E");
  return;
}

// Work out if we hit a Poltergeist or Orb
void ghostshot(int g)
{
  // print the button that triggered the event
  Serial.print("ghostshot : ");
  Serial.println(g);

  if (GHOSTS[g].poltergeist)
  {
    Serial.println("Poltergeist");
    GHOSTS[g].isalive = false;
    GHOSTS[g].poltergeist = false;
    uint8_t p = GHOSTS[g].pixel;
    pixels[p] = CRGB::Black;
    FastLED.show();
  }
  else
  {
    Serial.println("Orb");
  }
  endRound();
  return;
}
