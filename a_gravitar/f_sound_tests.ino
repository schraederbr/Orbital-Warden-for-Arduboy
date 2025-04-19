
// #include <ArduboyTones.h>

// BeepPin2 beep2;

// ArduboyTones sound(arduboy.audio.enabled);

// bool takeoffLoopStarted = false;

// PROGMEM const uint16_t SFX_LASER_300[] = {
//   270,  8,  320,  7,  295,  9,  255, 10,  335,  6,
//   285,  8,  310,  7,  260,  9,  330,  6,  300, 12,
//   245,  8,  345,  7,  275,  9,  315,  8,  265, 10,
//   340,  7,  290,  8,  250,  9,  325,  6,  305, 12,
//   TONES_REPEAT
// };

// PROGMEM const uint16_t THRUST_SFX[] = {
//   180,  20,  
//   TONES_REPEAT
// };

// PROGMEM const uint16_t PlAYER_BULLET_SFX[] = {
//   400,  20,  
//   TONES_REPEAT
// };

// void setup() {
//   arduboy.begin();
// }

// void loop() {
//   if (!arduboy.nextFrame()) return;
//   beep2.timer();
//   arduboy.pollButtons();

//   /* ---------- handle button ---------- */
//   if (arduboy.justPressed(A_BUTTON)) {         // reset flag
//     sound.tones(THRUST_SFX);       // play the spool‑up once
//     // beep2.tone(beep2.freq(400), 20);
//   }

//   if (arduboy.justPressed(B_BUTTON)) {         // reset flag
//     // sound.tones(PlAYER_BULLET_SFX);       // play the spool‑up once
//     //Frequency seems to be half what it comes out as. 100 sounds like 200 in ardens
//     beep2.tone(beep2.freq(100), 10);
//   }


//   // stop everything the moment A is released
//   if (arduboy.justReleased(A_BUTTON)) {
//     sound.noTone();
//   }

//   /* ---------- draw / update game ---------- */
//   arduboy.clear();
//   arduboy.print(F("Press A to launch!"));
//   arduboy.display();
// }