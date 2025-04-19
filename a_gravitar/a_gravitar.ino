#include <Arduboy2.h>
#include <math.h>
#include <avr/pgmspace.h>
#include "Font3x5.h"
Arduboy2 arduboy;
Font3x5 font3x5 = Font3x5();


const uint8_t PROGMEM title[] = {
72, 35,
0x00, 0x00, 0x00, 0xf0, 0xf0, 0x10, 0x10, 0x10, 0x10, 0x10, 0xf0, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0x40, 0x40, 0x40, 0xc0, 0xc0, 0x00, 0x00, 0x00, 0xfc, 0xfc, 0x40, 0x40, 0x40, 0x40, 0x40, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdc, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0xf0, 0xf0, 0x40, 0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xc0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xfc, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x1f, 0x1f, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1f, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x10, 0x00, 0x00, 0x00, 0x1e, 0x1e, 0x12, 0x12, 0x12, 0x12, 0x12, 0x1f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xf0, 0x10, 0x10, 0x10, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x10, 0x10, 0x10, 0x10, 0x10, 0xff, 0x00, 0x00, 0x00, 0xf0, 0x90, 0x90, 0x90, 0x90, 0x90, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x08, 0x08, 0x08, 0x08, 0x08, 0xf8, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x07, 0x04, 0x04, 0x04, 0x04, 0x04, 0x07, 0x04, 0x04, 0x04, 0x04, 0x04, 0x07, 0x00, 0x00, 0x07, 0x04, 0x04, 0x04, 0x04, 0x04, 0x07, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x04, 0x04, 0x04, 0x04, 0x04, 0x07, 0x00, 0x00, 0x00, 0x07, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};


const uint8_t PROGMEM shipSprite[] = {
5, 7,
0x60, 0x5c, 0x43, 0x5c, 0x60, 
};


//Frame count since starting life
int frames_alive = 0;

// -----------------------
// World & Camera Settings
// -----------------------
const int SCREEN_WIDTH  = 128;
const int SCREEN_HEIGHT = 64;
const int PLANET_MIN_RADIUS = 30;
const int PLANET_MAX_RADIUS = 150;
const int PLANET_STEP_ANGLE = 25;
float bumpiness = 0.0f; // Percent of min/max distance to use
float planetScale = 1.0f;

const int WORLD_WIDTH  = 400;
const int WORLD_HEIGHT = 400;

const int WORLD_CENTER_X = WORLD_WIDTH  / 2; 
const int WORLD_CENTER_Y = WORLD_HEIGHT / 2; 

float cameraX = 0;
float cameraY = 0;

const int TURRET_ACTIVE_DISTANCE = 60;
const int TURRET_SCORE = 250;
const int FRAME_RATE = 60;
const int DEFAULT_LIVES = 3;
int lives = DEFAULT_LIVES;
const int NUM_FUEL_PICKUPS = 3;
const int DEFAULT_FUEL = 15000;
const int FUEL_PER_PICKUP = 2500;
const int FUEL_PICKUP_SCORE = 100;
const int THRUST_FUEL_BURN_RATE = 1000; //per second when thrusting
const int TRACTOR_FUEL_BURN_RATE = 2000; //extra fuel burned per second when tractor beam is active
int currentFuel = DEFAULT_FUEL;
int score = 0;
float startX = WORLD_WIDTH / 2;
float startY = (WORLD_HEIGHT / 2) - PLANET_MAX_RADIUS - 10;  
float shipX, shipY;  // Position of the ship in world coordinates
float shipAngle;      // Ship’s facing angle in radians
float velX, velY;     // Velocity

const float ACCELERATION   = 0.02f;  
const float ROTATION_SPEED = 0.1f;  // Radians/frame rotation
const float FRICTION       = 0.997f; 
const float GRAVITY_ACCEL = 0.005f; 


const float TURRET_WIDTH = 3.0;
const float TURRET_HEIGHT = 6.0;

struct FuelPickup {
    float x;
    float y;
    float angle;
    int fuelAmount = FUEL_PER_PICKUP;
};

FuelPickup fuelPickups[NUM_FUEL_PICKUPS];
int pickupCount = 0;

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

static Point2D circle_points[360 / PLANET_STEP_ANGLE];
const int CIRCLE_NUM_POINTS  = (int)(360 / PLANET_STEP_ANGLE);
int circleCenterX = 0;
int circleCenterY = 0;

const int MAX_TURRETS = 5; 


// Simple bullet structure
struct Bullet {
  bool active;  
  float x;      
  float y;      
  float vx;     
  float vy;
  int framesAlive = 0;
};

static const int MAX_TURRET_BULLETS = 15;
static const int MAX_TURRET_BULLET_FRAMES_ALIVE = 120;


Bullet turretBullets[MAX_TURRET_BULLETS];

//Frames between shots
static const int TURRET_FIRE_DELAY = 90; 
//Frames before turrets start firing when you start the game. 
static const int TURRET_START_DELAY = 180;

Turret turrets[MAX_TURRETS];
int turretCount = 0;

// Maximum number of on-screen player bullets
static const int MAX_BULLETS = 5;
static const float MAX_BULLET_FRAMES_ALIVE = 120;

Bullet bullets[MAX_BULLETS];

void spawnBullet(float x, float y, float angle) {
  // Look for an inactive bullet slot
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (!bullets[i].active) {
      // Activate and position it
      bullets[i].active = true;
      bullets[i].x      = x;
      bullets[i].y      = y;

      float bulletSpeed = 1.0f;  // tweak as you wish

      bullets[i].vx = cos(angle - PI / 2) * bulletSpeed;
      bullets[i].vy = sin(angle - PI / 2) * bulletSpeed;
      playerShootSound();
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
        turretShootSound();
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

bool pointInTriangle(
  float px, float py,
  float x1, float y1,
  float x2, float y2,
  float x3, float y3)
{

  Point2D triPoints[3] = {
      { x1, y1 },
      { x2, y2 },
      { x3, y3 }
  };

  return pointInPolygonLocal(3, triPoints, px, py);
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

  Point2D rect[4] = {
      {x1, y1},
      {x2, y2},
      {x3, y3},
      {x4, y4}
  };

  return pointInPolygonLocal(4, rect, px, py);
}

float getDistanceSquared(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return dx * dx + dy * dy;
}

bool isWithinDistance(float x1, float y1, float x2, float y2, float distance) {
    return getDistanceSquared(x1, y1, x2, y2) <= distance * distance;
}

bool pointInPolygon(int numPoints, const Point2D points[], float px, float py) {
  bool inside = false;
  int pxInt = (int)px;
  int pyInt = (int)py;

  // Each point in circle_points is relative to (0,0); the actual world position is offset by WORLD_CENTER_X/Y.
  int j = numPoints - 1;
  for (int i = 0; i < numPoints; i++) {
    int xi = (int)points[i].x + WORLD_CENTER_X;
    int yi = (int)points[i].y + WORLD_CENTER_Y;
    int xj = (int)points[j].x + WORLD_CENTER_X;
    int yj = (int)points[j].y + WORLD_CENTER_Y;
    // Check if the ray crosses the edge
    if (((yi > pyInt) != (yj > pyInt)) &&
         (pxInt < (xj - xi) * (pyInt - yi) / (yj - yi) + xi)) {
      inside = !inside;
    }
    j = i;
  }
  return inside;
}

bool pointInPolygonLocal(int numPoints, const Point2D points[], float px, float py) {
  bool inside = false;

  int pxInt = (int)px;
  int pyInt = (int)py;

  int j = numPoints - 1;
  for (int i = 0; i < numPoints; i++) {
      int xi = (int)points[i].x;
      int yi = (int)points[i].y;
      int xj = (int)points[j].x;
      int yj = (int)points[j].y;

      // Standard ray-casting edge check
      if (((yi > pyInt) != (yj > pyInt)) &&
          (pxInt < (xj - xi) * (pyInt - yi) / (float)(yj - yi) + xi)) {
          inside = !inside;
      }
      j = i;
  }
  return inside;
}


// Generates an array of slightly irregular points forming a "random circle."
void generatePlanet(int angle_step, float min_distance, float max_distance) {
  randomSeed(micros()); 
  float maxHeight = WORLD_HEIGHT;
  for (int i = 0; i < CIRCLE_NUM_POINTS; i++) {
    // Base angle for this index
    int baseAngle = i * angle_step;

    // Add a small random offset in the range [-5..5]
    int offset = random(-5, 6); // random(a, b) goes [a..(b-1)]
    int angle = baseAngle + offset;

    // Clamp angle to [0..359] just in case
    if (angle < 0) {
      angle += 360;
    } else if (angle >= 360) {
      angle -= 360;
    }

    bumpiness = 0.3f + randomFloat(0.0f, 1.0f);
    if(bumpiness > 1.0f) {
      bumpiness = 1.0f;
    }

    planetScale = 0.2f + randomFloat(0.0f, 0.8f);
    if(planetScale > 1.0f) {
      planetScale = 1.0f;
    }

    min_distance = min_distance + ((1.0f - bumpiness) * (max_distance - min_distance));
     // Create a point at the (slightly randomized) angle
    Point2D pt = randomPointAtAngle(angle, min_distance * planetScale, max_distance * planetScale);
    // Optionally add a small x/y offset
    pt.x += randomFloat(-2.0f, 2.0f);
    pt.y += randomFloat(-2.0f, 2.0f);
    
    // Start the player at the top of the planet
    if(pt.y < maxHeight){
      maxHeight = pt.y;
      startX = WORLD_CENTER_X + pt.x;
      startY = WORLD_CENTER_Y + pt.y - 20;
    }

    circle_points[i] = pt;
  }


  // Compute centroid for gravity
  float sumX = 0;
  float sumY = 0;
  for (int i = 0; i < CIRCLE_NUM_POINTS; i++) {
    sumX += circle_points[i].x;
    sumY += circle_points[i].y;
  }
  circleCenterX = (int)(float(sumX) / CIRCLE_NUM_POINTS);
  circleCenterY = (int)(float(sumY) / CIRCLE_NUM_POINTS);
}

void generateFuelPickups(int numFuelPickups) {
  pickupCount = 0;
  for (int i = 0; i < numFuelPickups; i++) {
      int pickupIndex = random(0, CIRCLE_NUM_POINTS);
      Point2D p1 = circle_points[pickupIndex];
      Point2D p2 = circle_points[(pickupIndex + 1) % CIRCLE_NUM_POINTS];

      float dx = p2.x - p1.x;
      float dy = p2.y - p1.y;
      float edgeAngle = atan2(dy, dx);

      // pickupAngle is perpendicular to the edge
      float pickupAngle = edgeAngle + PI / 2.0;

      Point2D pickupOffset = randomPointOnLine(p1, p2);

      float worldX = WORLD_CENTER_X + pickupOffset.x;
      float worldY = WORLD_CENTER_Y + pickupOffset.y;

      float pickupRadius = 3.0f;
      worldX += cos(pickupAngle) * pickupRadius;
      worldY += sin(pickupAngle) * pickupRadius;

      fuelPickups[pickupCount].x = worldX;
      fuelPickups[pickupCount].y = worldY;
      fuelPickups[pickupCount].angle = pickupAngle;
      pickupCount++;
  }
}

void generateTurrets(int numTurrets) {
  turretCount = 0;
  for (int i = 0; i < numTurrets; i++) {
    int turretIndex = random(0, CIRCLE_NUM_POINTS);
    Point2D p1 = circle_points[turretIndex];
    Point2D p2 = circle_points[(turretIndex + 1) % CIRCLE_NUM_POINTS];

    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float edgeAngle = atan2(dy, dx);
    float turretAngle = edgeAngle;

    Point2D turretOffset = randomPointOnLine(p1, p2);
    float worldX = WORLD_CENTER_X + turretOffset.x;
    float worldY = WORLD_CENTER_Y + turretOffset.y;

    turrets[turretCount].x = worldX;
    turrets[turretCount].y = worldY;
    turrets[turretCount].angle = turretAngle;
    // Start the turret’s timer so they don’t all fire at once
    turrets[turretCount].fireTimer = random(0, TURRET_FIRE_DELAY);
    turretCount++;
  }
  for (int i = 0; i < turretCount; i++) {
    float pivotX = turrets[i].x - cameraX; 
    float pivotY = turrets[i].y - cameraY;
    float angle  = turrets[i].angle;      

    setTurretCorners(&turrets[i], pivotX, pivotY, TURRET_WIDTH, TURRET_HEIGHT);
  }

  // init all turret bullets as inactive
  for (int i = 0; i < MAX_TURRET_BULLETS; i++) {
    turretBullets[i].active = false;
  }

}

void waitForPress(){
  while(true){
    arduboy.pollButtons();
    if(arduboy.buttonsState()){
      break;
    }
  }
}