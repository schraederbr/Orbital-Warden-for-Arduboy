#include <Arduboy2.h>
#include <math.h>

// Creates a random point at a given angle and distance from the center.
Point2D random_point_at_angle(float angle_deg, float min_distance, float max_distance) {
  float angle_rad = angle_deg * (PI / 180.0f);
  float distance  = randomFloat(min_distance, max_distance);
  Point2D pt;
  pt.x = distance * cos(angle_rad);
  pt.y = distance * sin(angle_rad);
  return pt;
}

// Generates an array of slightly irregular points forming a "random circle."
Point2D* random_circle(int angle_step, float min_distance, float max_distance, int &num_points) {
  num_points = 360 / angle_step;  // assumes angle_step divides 360
  Point2D* points = new Point2D[num_points];
  int index = 0;

  for (int angle = 0; angle < 360; angle += angle_step) {
    // Base point
    Point2D pt = random_point_at_angle(angle, min_distance, max_distance);
    // Slight random offset
    pt.x += randomFloat(-2.0f, 2.0f);
    pt.y += randomFloat(-2.0f, 2.0f);

    points[index++] = pt;
  }
  return points;
}

// Draw lines connecting the points in "circle_points" (in world space).
// We subtract cameraX/cameraY so they appear correctly on the screen.
void draw_lines(Point2D* points, int num_points, bool close_shape) {
  if (num_points <= 0) return;

  int prevX = (int)(worldCenterX + points[0].x - cameraX);
  int prevY = (int)(worldCenterY + points[0].y - cameraY);

  for (int i = 1; i < num_points; i++) {
    int x = (int)(worldCenterX + points[i].x - cameraX);
    int y = (int)(worldCenterY + points[i].y - cameraY);
    arduboy.drawLine(prevX, prevY, x, y, WHITE);
    prevX = x;
    prevY = y;
  }

  if (close_shape) {
    int firstX = (int)(worldCenterX + points[0].x - cameraX);
    int firstY = (int)(worldCenterY + points[0].y - cameraY);
    arduboy.drawLine(prevX, prevY, firstX, firstY, WHITE);
  }
}

// Draws the "ship" as a small triangle, given a screen position & angle.
void drawShip(float screenX, float screenY, float angle) {
  // Triangle points relative to ship center
  float x1 = 0,   y1 = -4;
  float x2 = -3,  y2 = 4;
  float x3 = 3,   y3 = 4;

  float cosA = cos(angle);
  float sinA = sin(angle);

  // Rotate
  float rx1 = x1 * cosA - y1 * sinA;
  float ry1 = x1 * sinA + y1 * cosA;
  float rx2 = x2 * cosA - y2 * sinA;
  float ry2 = x2 * sinA + y2 * cosA;
  float rx3 = x3 * cosA - y3 * sinA;
  float ry3 = x3 * sinA + y3 * cosA;

  // Translate to screen position
  int ix1 = (int)(screenX + rx1);
  int iy1 = (int)(screenY + ry1);
  int ix2 = (int)(screenX + rx2);
  int iy2 = (int)(screenY + ry2);
  int ix3 = (int)(screenX + rx3);
  int iy3 = (int)(screenY + ry3);

  arduboy.drawTriangle(ix1, iy1, ix2, iy2, ix3, iy3, WHITE);
}