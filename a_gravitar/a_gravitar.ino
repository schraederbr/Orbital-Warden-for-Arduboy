#include <Arduboy2.h>
#include <math.h>
#include "Font3x5.h"

Arduboy2 arduboy;
Font3x5 font3x5 = Font3x5();

//Frame count since starting life
int frames_alive = 0;

// -----------------------
// World & Camera Settings
// -----------------------
const int screenWidth  = 128;
const int screenHeight = 64;
const int planetMinRadius = 40;
const int planetMaxRadius = 150;
const int planetStepAngle = 25;

const int worldWidth  = 512;
const int worldHeight = 512;

// Recalculate the world center
const int worldCenterX = worldWidth  / 2; 
const int worldCenterY = worldHeight / 2; 

// Camera variables (centered on the ship)
float cameraX = 0;
float cameraY = 0;

const int TURRET_ACTIVE_DISTANCE = 60;
const int TURRET_SCORE = 250;
const int FRAME_RATE = 60;
const int DEFAULT_LIVES = 3;
int lives = DEFAULT_LIVES;
const int NUM_FUEL_PICKUPS = 3;
const int DEFAULT_FUEL = 20000;
const int FUEL_PER_PICKUP = 2500;
const int FUEL_PICKUP_SCORE = 100;
const int THRUST_FUEL_BURN_RATE = 1000; //per second when thrusting
const int TRACTOR_FUEL_BURN_RATE = 2000; //extra fuel burned per second when tractor beam is active
int currentFuel = DEFAULT_FUEL;
int score = 0;
float startX = worldWidth / 2;
float startY = 256 - (planetMaxRadius + 10);  
const float turretWidth = 3.0;
const float turretHeight = 6.0;
struct FuelPickup {
    float x;
    float y;
    float angle;
    int fuelAmount = FUEL_PER_PICKUP;
};

FuelPickup fuelPickups[NUM_FUEL_PICKUPS];
int pickupCount = 0;
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
  int   fireTimer;  // counts down; when <= 0, shoot
  //Points for each corner
  Point2D p1;
  Point2D p2;
  Point2D p3;
  Point2D p4;
};
// Point2D* circle_points = nullptr;
static Point2D circle_points[360 / planetStepAngle];
const int circle_num_points  = (int)(360 / planetStepAngle);
//Might want these to be float
int circleCenterX = 0;
int circleCenterY = 0;
const int MAX_TURRETS = 5; // how many stars you want
bool gameOver = false;

// Simple bullet structure
struct Bullet {
  bool active;  // Is this bullet slot in use?
  float x;      // World position
  float y;      // World position
  float vx;     // Velocity X
  float vy;     // Velocity Y
  int framesAlive = 0;
};

// Turret bullet data
static const int MAX_TURRET_BULLETS = 15;
static const int MAX_TURRET_BULLET_FRAMES_ALIVE = 120;


Bullet turretBullets[MAX_TURRET_BULLETS];

// Each turret also needs a timer for shooting, say it fires every N frames
// We'll store this in your Turret struct, or a parallel array of timers.
static const int TURRET_FIRE_DELAY = 90; 
//Frames before turrets start firing when you start the game. 
static const int TURRET_START_DELAY = 180;


Turret turrets[MAX_TURRETS];
int turretCount = 0;


// Maximum number of on-screen bullets
static const int MAX_BULLETS = 5;
static const float MAX_BULLET_FRAMES_ALIVE = 120;


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


void spawnTurretBullet(float x, float y, float targetX, float targetY) {
  //Do short delay so player doesn't get immediately shot
  if(frames_alive > TURRET_START_DELAY){
    // Find an inactive slot
    for (int i = 0; i < MAX_TURRET_BULLETS; i++) {
      if (!turretBullets[i].active) {
        turretBullets[i].active = true;

        // Start at turret position
        turretBullets[i].x = x;
        turretBullets[i].y = y;

        // Aim at (targetX, targetY)
        float dx = targetX - x;
        float dy = targetY - y;
        float length = sqrt(dx*dx + dy*dy);
        if (length < 0.001f) {
          length = 0.001f; // avoid divide by zero
        }

        float speed = 0.5f; // adjust bullet speed as desired
        turretBullets[i].vx = (dx / length) * speed;
        turretBullets[i].vy = (dy / length) * speed;

        // Done
        break;
      }
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

bool pointInTriangle(float px, float py, float x1, float y1, float x2, float y2, float x3, float y3) {
    // Compute the area of the triangle using the determinant formula
    float denominator = ((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3));

    // Avoid division by zero (if the triangle is degenerate)
    if (denominator == 0.0) return false;

    // Compute barycentric coordinates
    float alpha = ((y2 - y3) * (px - x3) + (x3 - x2) * (py - y3)) / denominator;
    float beta  = ((y3 - y1) * (px - x3) + (x1 - x3) * (py - y3)) / denominator;
    float gamma = 1.0 - alpha - beta;

    // Check if the point is inside the triangle (all barycentric coordinates between 0 and 1)
    return (alpha >= 0.0 && alpha <= 1.0) && 
           (beta  >= 0.0 && beta  <= 1.0) && 
           (gamma >= 0.0 && gamma <= 1.0);
}

bool pointInRectangle(float px, float py, Turret* turret){
  return pointInRectangle(px, py, 
                          turret->p1.x, turret->p1.y, 
                          turret->p2.x, turret->p2.y, 
                          turret->p3.x, turret->p3.y, 
                          turret->p4.x, turret->p4.y);
}
bool pointInRectangle(
    float px, float py,
    float x1, float y1,
    float x2, float y2,
    float x3, float y3,
    float x4, float y4)
{
    // Check if point is in the triangle (x1,y1), (x2,y2), (x3,y3)
    bool inFirstTriangle = pointInTriangle(px, py, x1, y1, x2, y2, x3, y3);

    // Check if point is in the triangle (x1,y1), (x3,y3), (x4,y4)
    bool inSecondTriangle = pointInTriangle(px, py, x1, y1, x3, y3, x4, y4);

    return (inFirstTriangle || inSecondTriangle);
}

float getDistanceSquared(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return dx * dx + dy * dy;
}

bool isWithinDistance(float x1, float y1, float x2, float y2, float distance) {
    return getDistanceSquared(x1, y1, x2, y2) <= distance * distance;
}

// Returns true if the point (px,py) is inside the polygon
bool pointInPolygon(int numPoints, const Point2D points[], float px, float py) {
  bool inside = false;
  int pxInt = (int)px;
  int pyInt = (int)py;
  // The planet is defined by circle_points.
  // Each point in circle_points is relative to (0,0); the actual world position is offset by worldCenterX/Y.
  int j = numPoints - 1;
  for (int i = 0; i < numPoints; i++) {
    int xi = (int)points[i].x + worldCenterX;
    int yi = (int)points[i].y + worldCenterY;
    int xj = (int)points[j].x + worldCenterX;
    int yj = (int)points[j].y + worldCenterY;
    // Check if the ray crosses the edge
    if (((yi > pyInt) != (yj > pyInt)) &&
         (pxInt < (xj - xi) * (pyInt - yi) / (yj - yi) + xi)) {
      inside = !inside;
    }
    j = i;
  }
  return inside;
}
