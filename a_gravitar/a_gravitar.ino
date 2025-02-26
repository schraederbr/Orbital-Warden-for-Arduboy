#include <Arduboy2.h>
#include <math.h>

Arduboy2 arduboy;

// -----------------------
// World & Camera Settings
// -----------------------
const int screenWidth  = 128;
const int screenHeight = 64;
const int planetMinRadius = 90;
const int planetMaxRadius = 180;
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

struct Turret {
  float x;
  float y;
  float angle;
};
Point2D* circle_points = nullptr;
int circle_num_points  = 0;

const int NUM_TURRETS = 5; // how many stars you want

Turret turrets[NUM_TURRETS];


// Returns a random float in [min_val, max_val].
float randomFloat(float min_val, float max_val) {
  return min_val + (max_val - min_val) * (random(0, 10001) / 10000.0f);
}

Point2D randomPointOnLine(const Point2D& p0, const Point2D& p1) {
    // Generate a random t in [0,1]. On Arduino, random(max) returns a long in [0, max).
    // Adjust the range to get a floating-point number in [0,1].
    float t = static_cast<float>(random(10000)) / 10000.0f;

    // Linearly interpolate
    Point2D result;
    result.x = p0.x + t * (p1.x - p0.x);
    result.y = p0.y + t * (p1.y - p0.y);

    return result;
}


