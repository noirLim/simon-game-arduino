#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd_1(0x27, 16, 2);
const byte ledPins[] = {13, 12, 11, 10};
const byte buttonPins[] = {7, 6, 5, 4};

#define SPEAKER_PIN 2

#define MAX_GAME_LENGTH 100

int NOTE_DS5 = 622;
int NOTE_D5 = 587;

const int gameTones[] = {196, 220, 247, 262};

byte gameSequence[MAX_GAME_LENGTH] = {0};
byte gameIndex = 0;

/**
   Menyalakan led sesuai dengan ada yang diberikan
*/
void lightLedAndPlayTone(byte ledIndex)
{
  digitalWrite(ledPins[ledIndex], HIGH);
  tone(SPEAKER_PIN, gameTones[ledIndex]);
  delay(300);
  digitalWrite(ledPins[ledIndex], LOW);
  noTone(SPEAKER_PIN);
}

// mainkan sequence lagunya yang harus diikuti user
void playSequence()
{
  for (int i = 0; i < gameIndex; i++)
  {
    byte currentLed = gameSequence[i];
    lightLedAndPlayTone(currentLed);
    delay(50);
  }
}

bool checkUserSequence()
{
  for (int i = 0; i < gameIndex; i++)
  {
    byte expectedButton = gameSequence[i];
    byte actualButton = readButtons();
    lightLedAndPlayTone(actualButton);
    if (expectedButton != actualButton)
    {
      return false;
    }
  }

  return true;
}

byte readButtons()
{
  while (true)
  {
    for (byte i = 0; i < 4; i++)
    {
      byte buttonPin = buttonPins[i];
      if (digitalRead(buttonPin) == LOW)
      {
        return i;
      }
    }
    delay(1);
  }
}

// sound gameover
void gameOver()
{
  printScreen(gameIndex, false);
  gameIndex = 0;
  delay(200);

  tone(SPEAKER_PIN, NOTE_DS5);
  delay(300);
  tone(SPEAKER_PIN, NOTE_D5);
  delay(300);
  for (byte i = 0; i < 10; i++)
  {
    for (int pitch = -10; pitch <= 10; pitch++)
    {
      tone(SPEAKER_PIN, 523 + pitch);
      delay(5);
    }
  }
  noTone(SPEAKER_PIN);
  delay(500);
}

// level up sound
void playLevelUpSound()
{
  tone(SPEAKER_PIN, 330);
  delay(150);
  tone(SPEAKER_PIN, 392);
  delay(150);
  noTone(SPEAKER_PIN);
}

void setup()
{
  lcd_1.init();
  lcd_1.setCursor(0, 0);
  lcd_1.backlight();
  lcd_1.display();
  for (byte i = 0; i < 4; i++)
  {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(SPEAKER_PIN, OUTPUT);
  randomSeed(analogRead(A0));
}

void printScreen(int level, bool status)
{
  lcd_1.clear();
  lcd_1.setCursor(0, 0);
  if (status == true)
  {
    lcd_1.print("Simon Game");
  }
  else
  {
    lcd_1.print("Game over!");
  }
  lcd_1.setCursor(0, 1);
  lcd_1.print("Level:" + String(level));
  delay(300);
}

void loop()
{
  gameSequence[gameIndex] = random(0, 4);
  gameIndex++;
  if (gameIndex >= MAX_GAME_LENGTH)
  {
    gameIndex = MAX_GAME_LENGTH - 1;
  }

  playSequence();
  printScreen(gameIndex, true);

  if (!checkUserSequence())
  {
    gameOver();
  }

  delay(300);

  if (gameIndex > 0)
  {
    playLevelUpSound();
    delay(300);
  }
}