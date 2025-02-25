#include <Arduboy2.h>
#include <math.h>

Arduboy2 arduboy;

// -----------------------
// World & Camera Settings
// -----------------------
const int screenWidth  = 128;
const int screenHeight = 64;
const int planetMinRadius = 60;
const int planetMaxRadius = 140;
const int planetStepAngle = 25;

const int worldWidth  = 512;
const int worldHeight = 512;

// Recalculate the world center
const int worldCenterX = worldWidth  / 2; 
const int worldCenterY = worldHeight / 2; 

// Camera variables (centered on the ship)
float cameraX = 0;
float cameraY = 0;

// -----------------------
// Circle / Background
// -----------------------
struct Point2D {
  float x;
  float y;
};

Point2D* circle_points = nullptr;
int circle_num_points  = 0;

// Returns a random float in [min_val, max_val].
float randomFloat(float min_val, float max_val) {
  return min_val + (max_val - min_val) * (random(0, 10001) / 10000.0f);
}



