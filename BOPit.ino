#include <Adafruit_NeoPixel.h>
#include "star_trek_theme.h"

#define NUM_LEDS 8
#define PIN 0

Adafruit_NeoPixel pixels(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

int buzzer = 10;

int flexValue;
int flexPin = 11; // 11 is actually #12 on the flora

int tiltValue;
int prevTiltValue = 0;
int tiltPin = 9;
int tiltRate = 0;

int buttonValue;
int buttonPin = 6;

// game logic
int time = 5000; 
int endTime;
int action;
bool actionChose = false;
bool endGame = false;

int lights;

void setup() {
  pixels.begin(); // Initialize the NeoPixel library

  // Set the tilt and button to be input
  pinMode(tiltPin, INPUT);
  digitalWrite(tiltPin, HIGH);
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);

  endTime = millis() + 5000;
  
  showBop();
  delay(1000);
  Serial.begin(9600);
}

void loop() {
  showLeds();
  pixels.clear();
  
  // choose our action
  if (!actionChose){
    action = random(3); // bend = 0, shake = 1, bop = 2
    Serial.print("Action: ");
    Serial.println(action);
    actionChose = true;
    action = 1;
    playActionSound();

    // reduce time for next action
    time = time - 400;
    endTime = millis() + time;
  }

  // ran out of time
  if (millis() >= endTime){
    endGame = true;
  }

  // end condition
  if(time < 0 || endGame){
    Serial.println("YOU WIN");
    playWinningSong();
    return;
  }

  bend();
  shake();
  bop();

  // delay(500);
}

void showLeds(){
  lights = map(endTime - millis(), 0, time, 0, 8);

  Serial.print("LIGHTS: ");
  Serial.println(lights);

  for(int i = 0; i < lights; i++){
    pixels.setPixelColor(i, pixels.Color(0, 120, 0));
  }

  pixels.show();
}

void bend(){
  flexValue = analogRead(flexPin);

  // trigger
  if (flexPin > 700){

    if(action == 0){
      showBend();
    }      
    else{
      // end game, wrong action
      endGame = true;
    }
    actionChose = false;
  }
}

void shake(){
  // calculate rate and get vals
  tiltValue = digitalRead(tiltPin);
  if (tiltValue != prevTiltValue){
    tiltRate += 1;
  }
  prevTiltValue = tiltValue;

  // triggered
  if (tiltRate > 5){
    tiltRate = 0;
    if(action == 1){
      showShake();
    }
    else{
      endGame = true;
    }
    actionChose = false;
  }
}

void bop(){
  buttonValue = digitalRead(buttonPin);
  Serial.print("Bop value: ");
  Serial.println(buttonValue);

  // triggered
  if(buttonValue == LOW){
    if(action == 2){
      showBop();
    }
    else{
      endGame = true;
    }
    actionChose = false;
  }
}

void showBend(){
  // fill green
  for(int i = 8; i >= 0; i--){
    pixels.setPixelColor(i, pixels.Color(0, 120, 0));
    pixels.show();
    delay(150);
  }

  // blink purple 3 times
  for(int j = 0; j < 3; j++){
    for(int i = 0; i < 8; i++){
      pixels.setPixelColor(i, pixels.Color(128, 0, 128));
    }
    pixels.show();
    delay(200);
    pixels.fill(0,0,0);
    pixels.show();
    delay(200);
  }
}

void showShake(){
  // go one way
  for(int i=0;i<8;i++){
    pixels.setPixelColor(i, pixels.Color(250, 0, 0));
    for(int j=0;j<8;j++){
      if(i!=j){
	      pixels.setPixelColor(j, pixels.Color(0, 250, 0));
      }	
		}
		pixels.show();
		delay(200);
	} 

  // go the other
	for(int i=8;i>0;i--){
    pixels.setPixelColor(i, pixels.Color(250, 0, 0));
    for(int j=0;j<8;j++){
      if(i!=j){
	      pixels.setPixelColor(j, pixels.Color(0, 250, 0));
      }	
    }
    pixels.show();
    delay(200);
  } 
}

void showBop(){
  //  flash green 3 times quickly
  for(int i = 0; i < 3; i++){
    pixels.fill(pixels.Color(0, 255, 0));
    pixels.show();
    delay(200);
    pixels.fill(pixels.Color(0,0,0));
    pixels.show();
    delay(200);
  }
}

void playActionSound(){
  if (action == 0) {  // Bend
    tone(buzzer, 1000, 150);
    delay(170);
    tone(buzzer, 700, 150);
    delay(170);
    tone(buzzer, 500, 150);
    delay(170);
    tone(buzzer, 300, 200);
    delay(220);
    noTone(buzzer);
  } 
  else if (action == 1) {  // Shake
    for (int i = 0; i < 4; i++) {
      tone(buzzer, 660 + (i % 2) * 100, 60);
      delay(70);
    }
    noTone(buzzer);
  } 
  else {  // Bop

    tone(buzzer, 880, 100);
    delay(120);
    tone(buzzer, 440, 100);
    delay(120);
    noTone(buzzer);
  }

}

void playWinningSong(){// star trek jingle
  int wholenote = (60000 * 4) / starTrekTempo;
  int divider, noteDuration;

  for (int thisNote = 0; thisNote < starTrekNoteCount * 2; thisNote += 2) {
    divider = starTrekMelody[thisNote + 1];
    if (divider > 0) {
      noteDuration = wholenote / divider;
    } else {
      noteDuration = (wholenote / abs(divider)) * 1.5;
    }

    tone(buzzer, starTrekMelody[thisNote], noteDuration * 0.9);

    // Light up pixels with random colorssssss
    for (int i = 0; i < 8; i++) {
      pixels.setPixelColor(i, pixels.Color(random(100,255), random(100,255), random(100,255)));
    }

    pixels.show();
    delay(noteDuration);
    noTone(buzzer);
  }
  while(1){
    //hang the program
  }
}

