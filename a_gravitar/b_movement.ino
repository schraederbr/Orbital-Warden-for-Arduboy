#include <Arduboy2.h>
#include <math.h>

// -----------------------
// Ship Settings & Movement
// -----------------------
float shipX, shipY;  // Position of the ship in "world" coordinates
float shipAngle;      // Ship’s facing angle in radians
float velX, velY;     // Velocity

const float ACCELERATION   = 0.1f;  // How fast the ship accelerates
const float ROTATION_SPEED = 0.1f;  // Radians/frame rotation
const float FRICTION       = 0.99f; // Slows the ship gradually