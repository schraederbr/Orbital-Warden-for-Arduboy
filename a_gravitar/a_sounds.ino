#include <ArduboyTones.h>

BeepPin2 beep2;

ArduboyTones sound(arduboy.audio.enabled);
inline bool audioEnabled() { return arduboy.audio.enabled(); }

PROGMEM const uint16_t SFX_LASER_300[] = {
  270,  8,  320,  7,  295,  9,  255, 10,  335,  6,
  285,  8,  310,  7,  260,  9,  330,  6,  300, 12,
  245,  8,  345,  7,  275,  9,  315,  8,  265, 10,
  340,  7,  290,  8,  250,  9,  325,  6,  305, 12,
  TONES_REPEAT
};

PROGMEM const uint16_t THRUST_SFX[] = {
  180,  20,  
  TONES_REPEAT
};

PROGMEM const uint16_t TRACTOR_BEAM_SFX[] = {
    400,  25,  
    TONES_REPEAT
  };

PROGMEM const uint16_t PlAYER_BULLET_SFX[] = {
  400,  20,  
  TONES_REPEAT
};

PROGMEM const uint16_t TURRET_DESTROYED_SFX[] = {
  NOTE_C5, 60, NOTE_E5, 60, NOTE_G5, 120,
  0, 50,
  NOTE_C6, 180,
  TONES_END
};

PROGMEM const uint16_t FUEL_PICKUP_SFX[] = {
  900, 50, 0, 20, 900, 50,
  0, 75, 
  900, 50, 0, 20, 900, 50,
  0, 75, 
  900, 50, 0, 20, 900, 50,
  0, 75, 
  TONES_END
};

bool turretJustExploded = false;
bool fuelJustPickedUp = false;
bool thrusting = false;
bool tractorHeld = false;

enum ToneState { SND_IDLE, SND_THRUST, SND_TRACTOR_BEAM, SND_FUEL_PICKUP, SND_TURRET };
ToneState toneState = SND_IDLE;

void handleSounds() {

  // 1) Fire the turret‑destroyed sound when the event occurs
  if (turretJustExploded && toneState != SND_TURRET) {
    turretJustExploded = false;
    sound.tones(TURRET_DESTROYED_SFX);   // interrupts thrust
    toneState = SND_TURRET;
  }

  // 2) When that sequence finishes, fall back to whatever the buttons say
  if (toneState == SND_TURRET && !sound.playing()) {
    toneState = SND_IDLE;               // finished
  }

  if (fuelJustPickedUp && toneState == SND_IDLE) {           // start
    fuelJustPickedUp = false;
    sound.tones(FUEL_PICKUP_SFX);
    toneState = SND_FUEL_PICKUP;

  } 
  if (toneState == SND_FUEL_PICKUP && !sound.playing()) {
    toneState = SND_IDLE;               // finished
  }

  if (tractorHeld && toneState == SND_IDLE) {           // start
    sound.tones(TRACTOR_BEAM_SFX);
    toneState = SND_TRACTOR_BEAM;

  } else if (!tractorHeld && toneState == SND_TRACTOR_BEAM) { // stop
      sound.noTone();
      toneState = SND_IDLE;
  }


  if (thrusting && toneState == SND_IDLE) {          // start
      sound.tones(THRUST_SFX);
      toneState = SND_THRUST;

  } else if (!thrusting && toneState == SND_THRUST) { // stop
      sound.noTone();                                // instant cutoff
      toneState = SND_IDLE;
  }

}



void playerShootSound(){
    if (!audioEnabled()) return;
    beep2.tone(beep2.freq(250), 8);
}

void turretShootSound(){
    if (!audioEnabled()) return;
    beep2.tone(beep2.freq(180), 6);
}

void fuelPickedUpSound(){
  if (!audioEnabled()) return;
  beep2.tone(beep2.freq(350), 12);
}