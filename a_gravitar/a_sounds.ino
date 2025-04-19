#include <ArduboyTones.h>

BeepPin2 beep2;

ArduboyTones sound(arduboy.audio.enabled);


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

void thrustSound(){
    //Only play thrust sound if we aren't playing the tractor beam sound
    if(!arduboy.pressed(DOWN_BUTTON)){
        sound.tones(THRUST_SFX);
    }
}

void playerShootSound(){
    beep2.tone(beep2.freq(250), 8);
}

void turretShootSound(){
    beep2.tone(beep2.freq(180), 6);
}

void tractorBeamSound(){
    sound.tones(TRACTOR_BEAM_SFX); 
}