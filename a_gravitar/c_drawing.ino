#include <Arduboy2.h>
#include <math.h>





// -------------------------
// Stars (Background)
// -------------------------
static const int NUM_STARS = 100; // how many stars you want
struct Star {
  float x;
  float y;
  int   size;  // 1..3, for example
};
Star stars[NUM_STARS];
int starCount = 0;

// We pick a parallax factor < 1. If we use 0.5f, the stars move at half speed.
const float STAR_PARALLAX = 0.5f;

void generateStars() {
  // Create the full set of stars.
  for (int i = 0; i < NUM_STARS; i++) {
    stars[i].x    = random(0, worldWidth);   // anywhere in 0..512
    stars[i].y    = random(0, worldHeight);  // anywhere in 0..512
    stars[i].size = random(0, 2);            // 1..3 size
  }
  // Initially, we have NUM_STARS
  starCount = NUM_STARS;

  // Now remove any star that lies within the planet's bounding circle.
  // cullStarsInsidePlanet();
  
}

// This function compacts the array so that any star inside the planet is skipped.
void cullStarsInsidePlanet() {
  int j = 0;  // index for "kept" stars
  for (int i = 0; i < starCount; i++) {
    float dx = stars[i].x - worldCenterX;
    float dy = stars[i].y - worldCenterY;
    float distSq = dx * dx + dy * dy;

    // If this star is OUTSIDE the planet's bounding circle, keep it.
    // If it's inside, skip it.
    if (distSq >= (planetMaxRadius * planetMaxRadius)) {
      stars[j] = stars[i];
      j++;
    }
    // otherwise we do nothing, effectively dropping that star.
  }
  // Now 'j' is the number of stars we kept
  starCount = j;
}

// Draw all stars with parallax. Because they’re "far away," we shift them by
// cameraX * STAR_PARALLAX instead of cameraX * 1.0.
void drawStars() {
  for (int i = 0; i < NUM_STARS; i++) {
    // starX, starY in world coordinates
    float starX = stars[i].x;
    float starY = stars[i].y;
    int   starSize = stars[i].size;

    // ---------------------------
    // 1. Check if star is inside the planet's bounding circle
    // ---------------------------
    // float dx = starX - worldCenterX;
    // float dy = starY - worldCenterY;
    // float distSq = dx * dx + dy * dy;
    // if (distSq < (planetMaxRadius * planetMaxRadius)) {
    //   // This star is "behind" the planet, so skip drawing
    //   continue;
    // }

    // convert to screen coordinates with parallax factor
    int screenX = (int)(starX - (cameraX * STAR_PARALLAX));
    int screenY = (int)(starY - (cameraY * STAR_PARALLAX));

    // Only draw if on screen
    if (screenX >= 0 && screenX < screenWidth &&
        screenY >= 0 && screenY < screenHeight) {
      // A single pixel star (or use fillCircle for a larger dot)
      // arduboy.drawPixel(screenX, screenY, WHITE);
      arduboy.fillCircle(screenX, screenY, starSize, WHITE);
    }
  }
}



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
  // We still produce approximately (360 / angle_step) points:
  num_points = 360 / angle_step;
  Point2D* points = new Point2D[num_points];

  for (int i = 0; i < num_points; i++) {
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

    // Create a point at the (slightly randomized) angle
    Point2D pt = random_point_at_angle(angle, min_distance, max_distance);

    // Optionally add a small x/y offset
    pt.x += randomFloat(-2.0f, 2.0f);
    pt.y += randomFloat(-2.0f, 2.0f);

    points[i] = pt;
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

