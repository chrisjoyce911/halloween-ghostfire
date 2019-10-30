// Halloween Ghost Fire
//
// When a Ghost turns Red you can shoot it
// The more you hit the shorter the time you have to shoot the rest
//
// The game ends when you run out of time to shoot the Ghosts

#include "pitches.h"
#include "StensTimer.h"
/* stensTimer variable to be used later in the code */
StensTimer *stensTimer;

/* define some custom Action codes */
#define ROUND_TIMER 1
#define GAME_TIMER 2

#define ROUND_TIME 5000
#define GAME_TIME 60000
#define LAST_ROUND 20

#include "FastLED.h"
// Number of RGB LEDs in the strand
#define NUM_LEDS 45

// Define the array of leds
CRGB pixels[NUM_LEDS];
CRGB pixelsA[NUM_LEDS];
CRGB pixelsB[NUM_LEDS];
CRGB pixelsC[NUM_LEDS];

int pixelsBottom[] = {34, 36, 1, 37, 38, 3, 39, 40, 5, 41, 42, 7, 43, 10}; //Blue
int pixelsRight[] = {11, 12, 13, 14, 15, 16, 17};                          //Blue
int pixelsTop[] = {18, 19, 20, 21, 22, 23, 24, 25, 26};                    //Blue
int pixelsLeft[] = {27, 28, 29, 30, 31, 32, 33};                           //Blue

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

AceButton buttons[20];

void buttonEvent(AceButton *, uint8_t, uint8_t);
#define START_PIN 13
#define START_BUTTON 15

// the round of the game you are in
int gameRound;

int buzzerPin = 7;

// notes in the melody:
int melody[] = {
  NOTE_B5, NOTE_B4, NOTE_D5, NOTE_DS5, NOTE_F5S, NOTE_A5, 0, NOTE_B4, NOTE_GS5, 0, NOTE_B4,
  NOTE_B5, NOTE_B4, NOTE_D5, NOTE_DS5, NOTE_F5S, NOTE_A5, 0, NOTE_B4, NOTE_GS5, 0, NOTE_B4,
  NOTE_B5, NOTE_B4, NOTE_D5, NOTE_DS5, NOTE_F5S, NOTE_A5, 0, NOTE_B4, NOTE_GS5, 0, NOTE_B4,
  NOTE_B5, NOTE_B4, NOTE_D5, NOTE_DS5, NOTE_F5S, NOTE_A5, 0, NOTE_B4, NOTE_GS5, 0, NOTE_B4, //1st
  NOTE_B4, NOTE_B4, NOTE_DS5, NOTE_B4, NOTE_CS5, NOTE_A4, 0, 0,
  NOTE_B4, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_A4, NOTE_B4, 0
  NOTE_B4, NOTE_B4, NOTE_DS5, NOTE_B4, NOTE_CS5, NOTE_A4, 0, 0,
  NOTE_B4, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_A4, NOTE_CS5, NOTE_B4, 0, 0, //2nd
  NOTE_D5, NOTE_B4, 0, 0, 0, 0, NOTE_D5, NOTE_B4, 0, 0, 0, NOTE_D5, NOTE_B4, 0, 0, 0, NOTE_D5, NOTE_B4, 0, 0, 0, 0,
  NOTE_D5, NOTE_B4, 0, 0, NOTE_A4, NOTE_AS4, NOTE_B4, 0,
  NOTE_D5, NOTE_B4, 0, 0, 0, 0, NOTE_D5, NOTE_B4, 0, 0, 0, NOTE_D5, NOTE_B4, 0, 0, 0, NOTE_D5, NOTE_B4, 0, 0, 0, 0,
  NOTE_D5, NOTE_B4, 0, 0, NOTE_A4, NOTE_AS4, NOTE_B4, 0,
  NOTE_D5, NOTE_B4, 0, 0, 0, 0, NOTE_D5, NOTE_B4, 0, 0, 0, NOTE_D5, NOTE_B4, 0, 0, 0, NOTE_D5, NOTE_B4, 0, 0, 0, 0,
  NOTE_D5, NOTE_B4, 0, 0, NOTE_A4, NOTE_AS4, NOTE_B4, 0,
};

//translate
//1 = 1
//2 = 2
//3 = 4
//4 = 8
//5 = 16
//6 = 32
//7 = 64

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  8, 8, 16, 8, 8, 16, 16, 8, 16, 16, 8,
  8, 8, 16, 8, 8, 16, 16, 8, 16, 16, 8,
  8, 8, 16, 8, 8, 16, 16, 8, 16, 16, 8,
  8, 8, 16, 8, 8, 16, 16, 8, 16, 16, 8, //1st
  16, 8, 8, 8, 8, 8, 4,
  16, 16, 16, 16, 8, 8, 2,
  16, 8, 8, 8, 8, 8, 4,
  16, 16, 16, 16, 8, 8, 4, 8, //2nd
  8, 64, 64, 32, 32, 32, 8, 64, 64, 32, 16, 8, 64, 64, 32, 16, 8, 64, 64, 32, 32, 32,
  8, 32, 32, 16, 16, 16, 16, 16,
  8, 64, 64, 32, 32, 32, 8, 64, 64, 32, 16, 8, 64, 64, 32, 16, 8, 64, 64, 32, 32, 32,
  8, 32, 32, 16, 16, 16, 16, 16,
  8, 64, 64, 32, 32, 32, 8, 64, 64, 32, 16, 8, 64, 64, 32, 16, 8, 64, 64, 32, 32, 32,
  8, 32, 32, 16, 16, 16, 16, 16,
};

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // wait for serial attach

  FastLED.addLeds<WS2812, DATA_PIN, RGB>(pixels, NUM_LEDS);

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

  // Setup start button
  pinMode(13, INPUT_PULLUP);
  buttons[5].init(13, HIGH, 5);

  pinMode(buzzerPin, OUTPUT);

  for (byte i = 0; i < (sizeof(pixelsBottom) / sizeof(pixelsBottom[0])); i++)
  {
    pixels[pixelsBottom[i]] = CRGB::Blue;
    FastLED.show();
    delay(100);
    pixels[pixelsBottom[i]] = CRGB::Black;
    FastLED.show();
  }

  for (byte i = 0; i < (sizeof(pixelsRight) / sizeof(pixelsRight[0])); i++)
  {
    pixels[pixelsRight[i]] = CRGB::Blue;
    FastLED.show();
    delay(100);
    pixels[pixelsRight[i]] = CRGB::Black;
    FastLED.show();
  }

  for (byte i = 0; i < (sizeof(pixelsTop) / sizeof(pixelsTop[0])); i++)
  {
    pixels[pixelsTop[i]] = CRGB::Blue;
    FastLED.show();
    delay(100);
    pixels[pixelsTop[i]] = CRGB::Black;
    FastLED.show();
  }

  for (byte i = 0; i < (sizeof(pixelsLeft) / sizeof(pixelsLeft[0])); i++)
  {
    pixels[pixelsLeft[i]] = CRGB::Blue;
    FastLED.show();
    delay(100);
    pixels[pixelsLeft[i]] = CRGB::Black;
    FastLED.show();
  }

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
  Serial.println("New Round");
  // Increment round
  gameRound++;

  // pick the poltergeist/orbs
  uint8_t g = 0;
  for (uint8_t i = 0; i < NUM_GHOSTS; i++)
  {
    Serial.print("Check ghost  :");
    Serial.println(i);
    if (GHOSTS[i].isalive)
    {
      Serial.print("Ghost is alive :");
      Serial.println(i);
      g++;
    }
  }
  Serial.print("Found alive :");
  Serial.println(g);

  randomSeed(analogRead(A0));
  uint8_t p = random(0, g);
  Serial.print("Pick as poltergeist :");
  Serial.println(p);

  for (uint8_t i = 0; i < NUM_GHOSTS; i++)
  {

    if (!GHOSTS[i].isalive)
    {
      continue;
    }

    if (GHOSTS[i].isalive)
    {
      GHOSTS[i].poltergeist = false;
      pixels[GHOSTS[i].pixel] = CRGB::Green;
    }

    if (p == 0)
    {
      GHOSTS[i].poltergeist = true;
      pixels[GHOSTS[i].pixel] = CRGB::Red;
      Serial.print("poltergeist :");
      Serial.println(i);
      Serial.print(" button :");
      Serial.println(GHOSTS[i].buttonPin);
    }

    p = --p;
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
  //  busters();
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

  for (int i = 1000; i > 200; i--)
  {
    tone(buzzerPin, i);
  }
  noTone(buzzerPin);

  noTone(buzzerPin);

  endRound();
  return;
}

void busters()
{
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < sizeof(noteDuration)/sizeof(noteDuration[0]); thisNote++)
  {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1500 / noteDurations[thisNote];
    tone(buzzerPin, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(buzzerPin);
  }
}
