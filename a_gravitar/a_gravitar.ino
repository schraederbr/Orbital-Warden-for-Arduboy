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

const int MAX_TURRETS = 5; // how many stars you want
int turretCount = 0;

Turret turrets[MAX_TURRETS];


// Maximum number of on-screen bullets
static const int MAX_BULLETS = 5;

// Simple bullet structure
struct Bullet {
  bool active;  // Is this bullet slot in use?
  float x;      // World position
  float y;      // World position
  float vx;     // Velocity X
  float vy;     // Velocity Y
};

// The global array of bullets
Bullet bullets[MAX_BULLETS];

void spawnBullet(float x, float y, float angle) {
  // Look for an inactive bullet slot
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (!bullets[i].active) {
      // Activate and position it
      bullets[i].active = true;
      bullets[i].x      = x;
      bullets[i].y      = y;

      // Decide bullet speed
      float bulletSpeed = 1.0f;  // tweak as you wish

      // We want bullet direction to match the ship’s facing
      // Remember your code does "shipAngle = 0 => up," so we use (angle - PI/2)
      bullets[i].vx = cos(angle - PI / 2) * bulletSpeed;
      bullets[i].vy = sin(angle - PI / 2) * bulletSpeed;

      // Stop after spawning 1 bullet
      break;
    }
  }
}






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


