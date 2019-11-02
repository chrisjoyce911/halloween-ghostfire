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
#define ROUND_TIME 7000
#define LAST_ROUND 8

#define TO_WIN 5

#include "FastLED.h"
// Number of RGB LEDs in the strand
#define NUM_LEDS 43

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
  // Has been reset
  bool ready;
};

Ghost GHOSTS[NUM_GHOSTS] = {
    {9, 0, false, true, true},  // 1 - 12
    {10, 2, false, true, true}, // 4 - 9
    {12, 4, false, true, true}, // 0 - 11
    {11, 6, false, true, true}, // 2 - 8
    {8, 8, false, true, true},  // 3
};

AceButton buttons[NUM_GHOSTS];

void buttonEvent(AceButton *, uint8_t, uint8_t);

// the round of the game you are in
int gameRound;
bool gameRunning = true;
int gameScore = 0;
int buzzerPin = 7;

// notes in the melody:
int melody[] = {
    NOTE_B5, NOTE_B4, NOTE_D5, NOTE_DS5, NOTE_FS5, NOTE_A5, 0, NOTE_B4, NOTE_GS5, 0, NOTE_B4,
    NOTE_B5, NOTE_B4, NOTE_D5, NOTE_DS5, NOTE_FS5, NOTE_A5, 0, NOTE_B4, NOTE_GS5, 0, NOTE_B4,
    NOTE_B5, NOTE_B4, NOTE_D5, NOTE_DS5, NOTE_FS5, NOTE_A5, 0, NOTE_B4, NOTE_GS5, 0, NOTE_B4,
    NOTE_B5, NOTE_B4, NOTE_D5, NOTE_DS5, NOTE_FS5, NOTE_A5, 0, NOTE_B4, NOTE_GS5, 0, NOTE_B4, //1st
    NOTE_B4, NOTE_B4, NOTE_DS5, NOTE_B4, NOTE_CS5, NOTE_A4, 0, 0,
    NOTE_B4, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_A4, NOTE_B4, 0,
    NOTE_B4, NOTE_B4, NOTE_DS5, NOTE_B4, NOTE_CS5, NOTE_A4, 0, 0,
    NOTE_B4, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_A4, NOTE_CS5, NOTE_B4, 0, 0, //2nd
    NOTE_D5, NOTE_B4, 0, 0, 0, 0, NOTE_D5, NOTE_B4, 0, 0, 0, NOTE_D5, NOTE_B4, 0, 0, 0, NOTE_D5, NOTE_B4, 0, 0, 0, 0,
    NOTE_D5, NOTE_B4, 0, 0, NOTE_A4, NOTE_AS4, NOTE_B4, 0,
    NOTE_D5, NOTE_B4, 0, 0, 0, 0, NOTE_D5, NOTE_B4, 0, 0, 0, NOTE_D5, NOTE_B4, 0, 0, 0, NOTE_D5, NOTE_B4, 0, 0, 0, 0,
    NOTE_D5, NOTE_B4, 0, 0, NOTE_A4, NOTE_AS4, NOTE_B4, 0,
    NOTE_D5, NOTE_B4, 0, 0, 0, 0, NOTE_D5, NOTE_B4, 0, 0, 0, NOTE_D5, NOTE_B4, 0, 0, 0, NOTE_D5, NOTE_B4, 0, 0, 0, 0,
    NOTE_D5, NOTE_B4, 0, 0, NOTE_A4, NOTE_AS4, NOTE_B4, 0};

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
    8, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8,
    8, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8,
    8, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8,
    8, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8, //1st
    16, 8, 8, 8, 8, 8, 4,
    16, 16, 16, 16, 8, 8, 2,
    16, 8, 8, 8, 8, 8, 4,
    16, 16, 16, 16, 8, 8, 4, 8, //2nd
    2, 64, 64, 32, 32, 32, 2, 64, 64, 32, 16, 2, 64, 64, 32, 16, 2, 64, 64, 32, 32, 32,
    8, 32, 32, 16, 16, 16, 16, 16,
    2, 64, 64, 32, 32, 32, 2, 64, 64, 32, 16, 2, 64, 64, 32, 16, 2, 64, 64, 32, 32, 32,
    8, 32, 32, 16, 16, 16, 16, 16,
    2, 64, 64, 32, 32, 32, 2, 64, 64, 32, 16, 2, 64, 64, 32, 16, 2, 64, 64, 32, 32, 32,
    8, 32, 32, 16, 16, 16, 16, 16};

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

  pinMode(buzzerPin, OUTPUT);

  Serial.println("Setup");
  // busters();
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
}

// The buttonCallback handler
void buttonCallback(AceButton *button, uint8_t eventType, uint8_t buttonState)
{

  // Get the button
  uint8_t id = button->getId();

  // Serial.print("Got button : ");
  // Serial.println(id);

  //case AceButton::kEventReleased:

  ghostShot(id);
}

// this function will be called whenever a timer is due
void timerCallback(Timer *timer)
{
  /* check if the timer is one we expect */
  if (ROUND_TIMER == timer->getAction())
  {
    endRound();
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
  gameRunning = true;
  gameScore = 0;

  fill_solid(pixels, NUM_LEDS, CRGB::Red);
  FastLED.show();
  delay(1000);

  fill_solid(pixels, NUM_LEDS, CRGB::Orange);
  FastLED.show();
  delay(500);

  fill_solid(pixels, NUM_LEDS, CRGB::Green);
  FastLED.show();
  delay(250);

  fill_solid(pixels, NUM_LEDS, CRGB::Purple);
  FastLED.show();

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
    if (GHOSTS[i].isalive)
    {
      g++;
    }
  }

  randomSeed(analogRead(A0));
  uint8_t p = random(0, g);

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
      Serial.print(" button pin :");
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
  Serial.println("endRound()");
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
    Serial.println("LAST_ROUND -> endGame()");
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
  Serial.println("endGame()");
  if (!gameRunning)
  {
    Serial.println("returning");
    return;
  }
  Serial.println("endGame()");
  // Stop the ROUND_TIMER
  stensTimer->deleteTimer(ROUND_TIMER);

  gameRunning = false;
  // turns the pixels off
  for (uint8_t i = 0; i < NUM_GHOSTS; i++)
  {
    pixels[GHOSTS[i].pixel] = CRGB::Black;
  }
  FastLED.show();
  finalScore();
  return;
}

// Work out if we hit a Poltergeist or Orb
void ghostShot(int g)
{
  // Serial.print("ghostShot(int g) : ");
  // Serial.println(9);
  // Is the game in progress
  if (gameRunning)
  {
    Serial.println("gameRunning");
    // print the button that triggered the event
    if (GHOSTS[g].poltergeist)
    {
      Serial.println("Poltergeist");
      GHOSTS[g].isalive = false;
      GHOSTS[g].ready = false;
      GHOSTS[g].poltergeist = false;
      uint8_t p = GHOSTS[g].pixel;
      pixels[p] = CRGB::Black;
      FastLED.show();
      gameScore++;
    }
    else
    {
      Serial.println("Orb");
      GHOSTS[g].ready = false;
      GHOSTS[g].isalive = false;
    }

    for (int i = 1000; i > 200; i--)
    {
      tone(buzzerPin, i);
    }
    noTone(buzzerPin);
    Serial.println("ghostShot(int g) ->endRound()");
    endRound();
  }
  else
  {
    resetGhost(g);
  }
  return;
}

// Reset the ghost
void resetGhost(int g)
{
  Serial.print("resetGhost() :");
  Serial.println(g);
  GHOSTS[g].isalive = true;
  GHOSTS[g].ready = true;
  GHOSTS[g].poltergeist = false;
  uint8_t p = GHOSTS[g].pixel;
  pixels[p] = CRGB::Purple;
  FastLED.show();

  for (int i = 200; i > 1500; i++)
  {
    tone(buzzerPin, i);
  }
  noTone(buzzerPin);

  // are they all ready
  uint8_t r = 0;
  for (uint8_t i = 0; i < NUM_GHOSTS; i++)
  {
    if (GHOSTS[i].ready)
    {
      r++;
    }
  }

  if (r == NUM_GHOSTS)
  {
    newGame();
  }

  return;
}

// Decide if is the game was a winner
void finalScore()
{
  Serial.println("finalScore()");
  int NOTE_SUSTAIN = 250;
  if (gameScore >= TO_WIN)
  {
    //  fill_solid(pixels, NUM_LEDS, CRGB::Green);
    // FastLED.show();
    for (uint8_t nLoop = 0; nLoop < 2; nLoop++)
    {
      tone(buzzerPin, NOTE_A5);
      delay(NOTE_SUSTAIN);
      tone(buzzerPin, NOTE_B5);
      delay(NOTE_SUSTAIN);
      tone(buzzerPin, NOTE_C5);
      delay(NOTE_SUSTAIN);
      tone(buzzerPin, NOTE_B5);
      delay(NOTE_SUSTAIN);
      tone(buzzerPin, NOTE_C5);
      delay(NOTE_SUSTAIN);
      tone(buzzerPin, NOTE_D5);
      delay(NOTE_SUSTAIN);
      tone(buzzerPin, NOTE_C5);
      delay(NOTE_SUSTAIN);
      tone(buzzerPin, NOTE_D5);
      delay(NOTE_SUSTAIN);
      tone(buzzerPin, NOTE_E5);
      delay(NOTE_SUSTAIN);
      tone(buzzerPin, NOTE_D5);
      delay(NOTE_SUSTAIN);
      tone(buzzerPin, NOTE_E5);
      delay(NOTE_SUSTAIN);
      tone(buzzerPin, NOTE_E5);
      delay(NOTE_SUSTAIN);
    }
    noTone(buzzerPin);

    //  busters();
  }
  else
  {
    //  fill_solid(pixels, NUM_LEDS, CRGB::Red);
    // FastLED.show();
    tone(buzzerPin, NOTE_G4);
    delay(250);
    tone(buzzerPin, NOTE_C4);
    delay(500);
    noTone(buzzerPin);
    delay(5000);
  }
}

void busters()
{

  bool TickTock;
  // iterate over the notes of the melody:
  // for (int thisNote = 0; thisNote < sizeof(noteDurations) / sizeof(noteDurations[0]); thisNote++)
  for (int thisNote = 0; thisNote < 162; thisNote++)
  {
    TickTock = !TickTock;
    if (TickTock)
    {
      //   fill_solid(pixels, NUM_LEDS, CRGB::Blue);
    }
    else
    {
      //   fill_solid(pixels, NUM_LEDS, CRGB::Red);
    }
    FastLED.show();

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 2000 / noteDurations[thisNote];
    tone(buzzerPin, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(buzzerPin);
  }

  fill_solid(pixels, NUM_LEDS, CRGB::Black);
  FastLED.show();
  noTone(buzzerPin);
  return;
}
