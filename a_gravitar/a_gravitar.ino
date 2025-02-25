#include <Arduboy2.h>
#include <math.h>

Arduboy2 arduboy;

// Screen and world dimensions.
const int screenWidth = 128;
const int screenHeight = 64;
const int worldWidth = 256;
const int worldHeight = 128;
const int worldCenterX = worldWidth / 2;  // 128
const int worldCenterY = worldHeight / 2; // 64

// Global view offset (top-left corner of the visible region in the world)
int viewX = 0;
int viewY = 0;

// Custom point structure.
struct Point2D {
  float x;
  float y;
};

// Returns a random float between min_val and max_val.
float randomFloat(float min_val, float max_val) {
  return min_val + (max_val - min_val) * (random(0, 10001) / 10000.0f);
}

// Returns a point (relative to world center) at the specified angle (in degrees)
// and at a random distance between min_distance and max_distance.
Point2D random_point_at_angle(float angle_deg, float min_distance, float max_distance) {
  float angle_rad = angle_deg * (PI / 180.0f);
  float distance = randomFloat(min_distance, max_distance);
  Point2D pt;
  pt.x = distance * cos(angle_rad);
  pt.y = distance * sin(angle_rad);
  return pt;
}

// Draws connected lines between the points in the array.
// Each point is assumed to be relative to the world center. The view offset is applied.
// If close_shape is true, the last point is connected back to the first.
void draw_lines(Point2D* points, int num_points, bool close_shape) {
  if (num_points > 0) {
    int prevX = (worldCenterX + (int)round(points[0].x)) - viewX;
    int prevY = (worldCenterY + (int)round(points[0].y)) - viewY;
    for (int i = 1; i < num_points; i++) {
      int x = (worldCenterX + (int)round(points[i].x)) - viewX;
      int y = (worldCenterY + (int)round(points[i].y)) - viewY;
      arduboy.drawLine(prevX, prevY, x, y, WHITE);
      prevX = x;
      prevY = y;
    }
    if (close_shape) {
      int x = (worldCenterX + (int)round(points[0].x)) - viewX;
      int y = (worldCenterY + (int)round(points[0].y)) - viewY;
      arduboy.drawLine(prevX, prevY, x, y, WHITE);
    }
  }
}

// Generates an array of points by stepping through angles from 0 to 360 (non-inclusive).
// Each point is generated relative to the world center with a random distance (min_distance to max_distance)
// and then given a slight random offset (±2 pixels) for irregularity.
Point2D* random_circle(int angle_step, float min_distance, float max_distance, int &num_points) {
  num_points = 360 / angle_step;  // Assumes angle_step divides 360 evenly.
  Point2D* circle_points = new Point2D[num_points];
  int index = 0;
  for (int angle = 0; angle < 360; angle += angle_step) {
    Point2D pt = random_point_at_angle(angle, min_distance, max_distance);
    pt.x += randomFloat(-2.0f, 2.0f);
    pt.y += randomFloat(-2.0f, 2.0f);
    circle_points[index++] = pt;
  }
  return circle_points;
}

// Global variable to hold the current random circle.
Point2D* circle_points = nullptr;
int circle_num_points = 0;

void setup() {
  arduboy.begin();
  arduboy.clear();
  randomSeed(analogRead(0));
  // Generate an initial random circle with angle step 30° and distance between 10 and 30 pixels.
  circle_points = random_circle(30, 10, 30, circle_num_points);
}

void loop() {
  if (!arduboy.nextFrame())
    return;
    
  arduboy.pollButtons();
  arduboy.clear();
  
  // --- DPAD Scrolling ---
  const int scrollSpeed = 2;
  if (arduboy.pressed(LEFT_BUTTON)) {
    viewX -= scrollSpeed;
    if (viewX < 0)
      viewX = 0;
  }
  if (arduboy.pressed(RIGHT_BUTTON)) {
    viewX += scrollSpeed;
    if (viewX > (worldWidth - screenWidth))
      viewX = worldWidth - screenWidth;
  }
  if (arduboy.pressed(UP_BUTTON)) {
    viewY -= scrollSpeed;
    if (viewY < 0)
      viewY = 0;
  }
  if (arduboy.pressed(DOWN_BUTTON)) {
    viewY += scrollSpeed;
    if (viewY > (worldHeight - screenHeight))
      viewY = worldHeight - screenHeight;
  }
  
  // --- Button to Generate New Circle ---
  if (arduboy.justPressed(A_BUTTON)) {
    if (circle_points != nullptr) {
      delete[] circle_points;
    }
    circle_points = random_circle(30, 20, 60, circle_num_points);
  }
  
  // --- Draw the Random Circle ---
  if (circle_points != nullptr) {
    for (int i = 0; i < circle_num_points; i++) {
      int x = (worldCenterX + (int)round(circle_points[i].x)) - viewX;
      int y = (worldCenterY + (int)round(circle_points[i].y)) - viewY;
      arduboy.fillCircle(x, y, 1, WHITE);
    }
    draw_lines(circle_points, circle_num_points, true);
  }
  
  arduboy.display();
}
