#include <Arduboy2.h>
#include <math.h>


static const int NUM_STARS = 0;
struct Star {
  float x;
  float y;
  int   size;  
};
Star stars[NUM_STARS];
int starCount = 0;

// We pick a parallax factor < 1. If we use 0.5f, the stars move at half speed.
const float STAR_PARALLAX = 0.5f;

void generateStars() {
  for (int i = 0; i < NUM_STARS; i++) {
    stars[i].x    = random(0, WORLD_WIDTH);   // anywhere in 0..512
    stars[i].y    = random(0, WORLD_HEIGHT);  // anywhere in 0..512
    stars[i].size = random(0, 2);            // 1..3 size
  }

  starCount = NUM_STARS;
  // Now remove any star that lies within the planet's bounding circle.
  cullStarsInsidePlanet();
}

// This function compacts the array so that any star inside the planet is skipped.
void cullStarsInsidePlanet() {
  int j = 0;  // index for "kept" stars
  for (int i = 0; i < starCount; i++) {
    float dx = stars[i].x - WORLD_CENTER_X;
    float dy = stars[i].y - WORLD_CENTER_Y;
    float distSq = dx * dx + dy * dy;

    // If this star is OUTSIDE the planet's bounding circle, keep it.
    // If it's inside, skip it.
    if (distSq >= (PLANET_MAX_RADIUS * PLANET_MAX_RADIUS)) {
      stars[j] = stars[i];
      j++;
    }
  }
  starCount = j;
}

// Draw all stars with parallax.
void drawStars() {
  for (int i = 0; i < NUM_STARS; i++) {
    float starX = stars[i].x;
    float starY = stars[i].y;
    int   starSize = stars[i].size;

    // convert to screen coordinates with parallax factor
    int screenX = (int)(starX - (cameraX * STAR_PARALLAX));
    int screenY = (int)(starY - (cameraY * STAR_PARALLAX));

    // Only draw if on screen
    if (screenX >= 0 && screenX < SCREEN_WIDTH &&
        screenY >= 0 && screenY < SCREEN_HEIGHT) {
      arduboy.fillCircle(screenX, screenY, starSize, WHITE);
    }
  }
}
void drawPlanet(bool drawLines = true, bool drawDots = false, bool drawTriangles = false, bool drawHorizontalLines = false){
  //Draw dots where lines connect
  if(drawDots){
    for (int i = 0; i < CIRCLE_NUM_POINTS; i++) {
      int x = (int)((WORLD_CENTER_X + circle_points[i].x) - cameraX);
      int y = (int)((WORLD_CENTER_Y + circle_points[i].y) - cameraY);
      arduboy.fillCircle(x, y, 1, WHITE);
    }
  }

  // Draw planet as lines
  if(drawLines){
    drawPolygonLines(circle_points, CIRCLE_NUM_POINTS, true);
  }
      
  // Draw planet as filled in by using horizontal scan lines
  if(drawHorizontalLines){
    static int tx[CIRCLE_NUM_POINTS]; // or some safe max; must >= CIRCLE_NUM_POINTS
    static int ty[CIRCLE_NUM_POINTS];

    for (int i = 0; i < CIRCLE_NUM_POINTS; i++) {
      tx[i] = (int)(WORLD_CENTER_X + circle_points[i].x - cameraX);
      ty[i] = (int)(WORLD_CENTER_Y + circle_points[i].y - cameraY);
    }

    //--------------------------------------------------------------------------
    // 4. Fill the polygon by horizontal scan lines
    //--------------------------------------------------------------------------
    // fillPolygonHorizontal(tx, ty, CIRCLE_NUM_POINTS);
  }
  
  // Draw planet as filled in triangles
  if (drawTriangles) {
      static int tx[CIRCLE_NUM_POINTS]; 
      static int ty[CIRCLE_NUM_POINTS]; 

      for (int i = 0; i < CIRCLE_NUM_POINTS; i++) {
          tx[i] = (int)(WORLD_CENTER_X + circle_points[i].x - cameraX);
          ty[i] = (int)(WORLD_CENTER_Y + circle_points[i].y - cameraY);
      }

      for (int i = 0; i < CIRCLE_NUM_POINTS - 1; i++) {
          arduboy.fillTriangle(circleCenterX, circleCenterY, tx[i], ty[i], tx[i + 1], ty[i + 1], WHITE);
      }

      arduboy.fillTriangle(circleCenterX, circleCenterY, tx[CIRCLE_NUM_POINTS - 1], ty[CIRCLE_NUM_POINTS - 1], tx[0], ty[0], WHITE);
  }

  
    

}

void drawPlayerBullets(){
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) {
      float bx = bullets[i].x - cameraX;
      float by = bullets[i].y - cameraY;
      arduboy.fillCircle((int)bx, (int)by, 1, WHITE);
    }
  }
}

void drawTurretBullets() {
  for (int i = 0; i < MAX_TURRET_BULLETS; i++) {
    if (turretBullets[i].active) {
      float bx = turretBullets[i].x - cameraX;
      float by = turretBullets[i].y - cameraY;
      arduboy.fillCircle((int)bx, (int)by, 1, WHITE);
    }
  }
}


// void fillPolygonHorizontal(const int *px, const int *py, int n) {
//   if (n < 3) return;

//   // 1. Find bounding box in Y
//   int minY = py[0];
//   int maxY = py[0];
//   for (int i = 1; i < n; i++) {
//     if (py[i] < minY) minY = py[i];
//     if (py[i] > maxY) maxY = py[i];
//   }

//   // Clamp to screen bounds [0..HEIGHT-1]
//   if (minY < 0) minY = 0;
//   if (maxY >= HEIGHT) maxY = HEIGHT - 1;

//   // Temporary array for storing intersection X coords
//   static int xIntersections[CIRCLE_NUM_POINTS]; // must be >= max number of edges

//   // 2. For each scanline y in [minY .. maxY]
//   for (int y = minY; y <= maxY; y++) {
//     int numIntersections = 0;

//     // Scan all edges
//     for (int i = 0; i < n; i++) {
//       int j = (i + 1) % n; // next vertex index

//       // Retrieve edge endpoints
//       int y1 = py[i], y2 = py[j];
//       int x1 = px[i], x2 = px[j];

//       // Skip horizontal edges
//       if (y1 == y2) continue;

//       // Order y1 <= y2
//       if (y1 > y2) {
//         int tmp = y1; y1 = y2; y2 = tmp;
//         tmp = x1; x1 = x2; x2 = tmp;
//       }

//       // Check if this scanline intersects [y1..y2)
//       if (y >= y1 && y < y2) {
//         // Integer interpolation:
//         // xHit = x1 + ((x2 - x1)*(y - y1)) / (y2 - y1)
//         int dy1 = y - y1;
//         int dy  = (y2 - y1);
//         // Use 32-bit for multiplication to avoid 16-bit overflow
//         int32_t dx = (int32_t)(x2 - x1) * (int32_t)dy1;

//         int xHit = x1 + (int)(dx / dy);

//         xIntersections[numIntersections++] = xHit;
//       }
//     }

//     // If no intersections, skip
//     if (numIntersections < 2) continue;

//     // 2b. Sort intersection X coords (Insertion sort here for example)
//     for (int i = 1; i < numIntersections; i++) {
//       int key = xIntersections[i];
//       int j = i - 1;
//       while (j >= 0 && xIntersections[j] > key) {
//         xIntersections[j + 1] = xIntersections[j];
//         j--;
//       }
//       xIntersections[j + 1] = key;
//     }

//     // 2c. Draw horizontal line segments
//     for (int i = 0; i < numIntersections - 1; i += 2) {
//       int xStart = xIntersections[i];
//       int xEnd   = xIntersections[i + 1];

//       // Clip horizontally
//       if (xEnd < 0 || xStart >= WIDTH) {
//         continue;
//       }
//       if (xStart < 0) xStart = 0;
//       if (xEnd >= WIDTH) xEnd = WIDTH - 1;

//       arduboy.drawFastHLine(xStart, y, (xEnd - xStart + 1), WHITE);
//     }
//   }
// }

void drawTurret(Turret* turret, float screenX, float screenY){

  int ix1 = (int)(screenX + turret->p1.x - turret->x);
  int iy1 = (int)(screenY + turret->p1.y - turret->y);
  int ix2 = (int)(screenX + turret->p2.x - turret->x);
  int iy2 = (int)(screenY + turret->p2.y - turret->y);
  int ix3 = (int)(screenX + turret->p3.x - turret->x);
  int iy3 = (int)(screenY + turret->p3.y - turret->y);
  int ix4 = (int)(screenX + turret->p4.x - turret->x);
  int iy4 = (int)(screenY + turret->p4.y - turret->y);

  // Draw the edges of the rectangle (4 lines)
  arduboy.drawLine(ix1, iy1, ix2, iy2, WHITE);
  arduboy.drawLine(ix2, iy2, ix3, iy3, WHITE);
  arduboy.drawLine(ix3, iy3, ix4, iy4, WHITE);
  arduboy.drawLine(ix4, iy4, ix1, iy1, WHITE);
}

void setTurretCorners(Turret* turret, float screenX, float screenY, float width, float height){
  // Half dimensions so the rectangle is centered at (0, 0) before translation
  float halfW = width * 0.5;
  float halfH = height * 0.5;

  // Rectangle corners relative to center
  // We'll define them in a clockwise (or counter-clockwise) order

  //Use this if you want the turret to be smaller on the top, similar to the turret shape in gravitar
  //float x1 = -halfW/2;  float y1 = -halfH;  // top-left
  // float x2 =  halfW/2;  float y2 = -halfH;  // top-right
  float x1 = -halfW;  float y1 = -halfH;  // top-left
  float x2 =  halfW;  float y2 = -halfH;  // top-right
  float x3 =  halfW;  float y3 =  halfH;  // bottom-right
  float x4 = -halfW;  float y4 =  halfH;  // bottom-left
  
  // Precompute sine and cosine of the angle
  float cosA = cos(turret->angle);
  float sinA = sin(turret->angle);

  // Rotate each corner
  float rx1 = x1 * cosA - y1 * sinA;
  float ry1 = x1 * sinA + y1 * cosA;

  float rx2 = x2 * cosA - y2 * sinA;
  float ry2 = x2 * sinA + y2 * cosA;

  float rx3 = x3 * cosA - y3 * sinA;
  float ry3 = x3 * sinA + y3 * cosA;

  float rx4 = x4 * cosA - y4 * sinA;
  float ry4 = x4 * sinA + y4 * cosA;

  float offsetX =  (0.0f) * cos(turret->angle) - (-halfH) * sin(turret->angle);
  float offsetY =  (0.0f) * sin(turret->angle) + (-halfH) * cos(turret->angle);

  turret->x = turret->x + offsetX;
  turret->y = turret->y + offsetY;

  // Translate to the world position
  turret->p1.x = turret->x + rx1;
  turret->p1.y = turret->y + ry1;
  turret->p2.x = turret->x + rx2;
  turret->p2.y = turret->y + ry2;
  turret->p3.x = turret->x + rx3;
  turret->p3.y = turret->y + ry3;
  turret->p4.x = turret->x + rx4;
  turret->p4.y = turret->y + ry4;


}

// Draws a rotated rectangle of given width & height, centered at (screenX, screenY), at the specified angle.
void drawRotatedRect(float screenX, float screenY, float width, float height, float angle)
{
  // Half dimensions so the rectangle is centered at (0, 0) before translation
  float halfW = width * 0.5;
  float halfH = height * 0.5;

  // Rectangle corners relative to center
  // We'll define them in a clockwise (or counter-clockwise) order
  float x1 = -halfW;  float y1 = -halfH;  // top-left
  float x2 =  halfW;  float y2 = -halfH;  // top-right
  float x3 =  halfW;  float y3 =  halfH;  // bottom-right
  float x4 = -halfW;  float y4 =  halfH;  // bottom-left

  // Precompute sine and cosine of the angle
  float cosA = cos(angle);
  float sinA = sin(angle);

  // Rotate each corner
  float rx1 = x1 * cosA - y1 * sinA;
  float ry1 = x1 * sinA + y1 * cosA;

  float rx2 = x2 * cosA - y2 * sinA;
  float ry2 = x2 * sinA + y2 * cosA;

  float rx3 = x3 * cosA - y3 * sinA;
  float ry3 = x3 * sinA + y3 * cosA;

  float rx4 = x4 * cosA - y4 * sinA;
  float ry4 = x4 * sinA + y4 * cosA;

  // Translate to the screen position
  int ix1 = (int)(screenX + rx1);
  int iy1 = (int)(screenY + ry1);
  int ix2 = (int)(screenX + rx2);
  int iy2 = (int)(screenY + ry2);
  int ix3 = (int)(screenX + rx3);
  int iy3 = (int)(screenY + ry3);
  int ix4 = (int)(screenX + rx4);
  int iy4 = (int)(screenY + ry4);

  // Draw the edges of the rectangle (4 lines)
  arduboy.drawLine(ix1, iy1, ix2, iy2, WHITE);
  arduboy.drawLine(ix2, iy2, ix3, iy3, WHITE);
  arduboy.drawLine(ix3, iy3, ix4, iy4, WHITE);
  arduboy.drawLine(ix4, iy4, ix1, iy1, WHITE);
}

// Creates a random point at a given angle and distance from the center.
Point2D randomPointAtAngle(float angle_deg, float min_distance, float max_distance) {
  float angle_rad = angle_deg * (PI / 180.0f);
  float distance  = randomFloat(min_distance, max_distance);
  Point2D pt;
  pt.x = distance * cos(angle_rad);
  pt.y = distance * sin(angle_rad);
  return pt;
}

void drawAllFuelPickups(){
    for (int i = 0; i < pickupCount; i++) {
        float pivotX = fuelPickups[i].x - cameraX; // pivot = planet perimeter
        float pivotY = fuelPickups[i].y - cameraY;
        // The commented-out code is for drawing fuel pickups as triangles
        // float angle  = fuelPickups[i].angle + PI/2;       // tangent + π/2 (or however you computed)

        // float pickupW = 4.0;  // or whatever size
        // float pickupH = 4.0;  // for example

        // // Half-sizes
        // float halfW = pickupW * 0.5;
        // float halfH = pickupH * 0.5;

        // // We want the rectangle's bottom edge at the pivot. 
        // // In local coords, that bottom edge = +halfH.
        // // So we shift the center by (0, -halfH) in local space, then rotate it.
        // float offsetX =  (0.0f) * cos(angle) - (-halfH) * sin(angle);
        // float offsetY =  (0.0f) * sin(angle) + (-halfH) * cos(angle);

        // float centerX = pivotX + offsetX;
        // float centerY = pivotY + offsetY;

        // // Now draw the rectangle with its center at (centerX, centerY) and rotation = angle
        // drawRotatedRect(centerX, centerY, pickupW, pickupH, angle);
        arduboy.fillCircle(pivotX, pivotY, 3);
    }
}


void drawAllTurrets() {
  for (int i = 0; i < turretCount; i++) {
    float pivotX = turrets[i].x - cameraX; // pivot = planet perimeter
    float pivotY = turrets[i].y - cameraY;
    drawTurret(&turrets[i], pivotX, pivotY);
  }
}


// Draw lines connecting the points in "circle_points" (in world space).
// We subtract cameraX/cameraY so they appear correctly on the screen.
void drawPolygonLines(Point2D* points, int num_points, bool close_shape) {
  if (num_points <= 0) return;

  int prevX = (int)(WORLD_CENTER_X + points[0].x - cameraX);
  int prevY = (int)(WORLD_CENTER_Y + points[0].y - cameraY);

  for (int i = 1; i < num_points; i++) {
    int x = (int)(WORLD_CENTER_X + points[i].x - cameraX);
    int y = (int)(WORLD_CENTER_Y + points[i].y - cameraY);
    arduboy.drawLine(prevX, prevY, x, y, WHITE);
    prevX = x;
    prevY = y;
  }

  if (close_shape) {
    int firstX = (int)(WORLD_CENTER_X + points[0].x - cameraX);
    int firstY = (int)(WORLD_CENTER_Y + points[0].y - cameraY);
    arduboy.drawLine(prevX, prevY, firstX, firstY, WHITE);
  }
}

void drawRotatedTriangle(bool filled, int linesToDraw, float screenX, float screenY, float angle, int x1, int y1, int x2, int y2, int x3, int y3){

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

  if(linesToDraw == 0b111 || filled){
    if(filled){
      arduboy.fillTriangle(ix1, iy1, ix2, iy2, ix3, iy3, WHITE);
    }
    else{
      arduboy.drawTriangle(ix1, iy1, ix2, iy2, ix3, iy3, WHITE);
    }
  }
  else{
    if (linesToDraw & 0b001) {
      arduboy.drawLine(ix1, iy1, ix2, iy2, WHITE);
    }
    if (linesToDraw & 0b010) {
      arduboy.drawLine(ix2, iy2, ix3, iy3, WHITE);
    }
    if (linesToDraw & 0b100) {
      arduboy.drawLine(ix3, iy3, ix1, iy1, WHITE);
    }
  }

  
  

}

// simpleStyle = true: Draws a simple triangle
// simpleStyle = false: Draws a ship similar to the one in Gravitar
void drawShip(bool smallShip, bool simpleStyle, float screenX, float screenY, float angle) {
  float x1 = 0,   y1 = -5;
  float x2 = -5,  y2 = 0;
  float x3 = 5,   y3 = 0;
  // Triangle points relative to ship center
  if(smallShip){
    x1 = 0,   y1 = -4;
    x2 = -2,  y2 = 2;
    x3 = 2,   y3 = 2;
  }


  if(simpleStyle){
    drawRotatedTriangle(false, 0b111, screenX, screenY, angle, x1, y1, x2, y2, x3, y3);
  }
  else{
    drawRotatedTriangle(false, 0b101, screenX, screenY, angle, x1, y1, x2, y2, x3, y3);

    // Draw the left small triangle
    x1 = -3;   y1 = 3;
    x2 = -5;  y2 = 1;
    x3 = 0;   y3 = 0;
    drawRotatedTriangle(false, 0b101, screenX, screenY, angle, x1, y1, x2, y2, x3, y3);

    // Draw the right small triangle
    x1 = 3;   y1 = 3;
    x2 = 5;  y2 = 1;
    x3 = 0;   y3 = 0;
    drawRotatedTriangle(false, 0b101, screenX, screenY, angle, x1, y1, x2, y2, x3, y3);
  }
  if (arduboy.pressed(A_BUTTON)){
    // Draw the thrust triangle
    x1 = -2;   y1 = 3;
    x2 = 0;  y2 = 7;
    x3 = 2;   y3 = 3;
    if(currentFuel > 0){
      drawRotatedTriangle(true, 0b011, screenX, screenY, angle, x1, y1, x2, y2, x3, y3);
    }
  }
  if(arduboy.pressed(DOWN_BUTTON)){
    if(currentFuel > 0){
      //Draw tractor beam
      x1 = -15;   y1 = 25;
      x2 = 0;  y2 = 0;
      x3 = 15;   y3 = 25;
      drawRotatedTriangle(false, 0b011, screenX, screenY, angle, x1, y1, x2, y2, x3, y3);
      //Draw shield
      drawRotatedRect(screenX, screenY, 12, 12, angle);
    }

  }

}




