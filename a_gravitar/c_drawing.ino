#include <Arduboy2.h>
#include <math.h>





// -------------------------
// Stars (Background)
// -------------------------
static const int NUM_STARS = 1; // how many stars you want
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
void drawPlanet(bool drawLines = true, bool drawDots = false, bool drawTriangles = false, bool drawHorizontalLines = false){
  //Draw dots where lines connect
  if(drawDots){
    for (int i = 0; i < CIRCLE_NUM_POINTS; i++) {
      int x = (int)((worldCenterX + circle_points[i].x) - cameraX);
      int y = (int)((worldCenterY + circle_points[i].y) - cameraY);
      arduboy.fillCircle(x, y, 1, WHITE);
    }
  }

  // Draw planet as lines
  if(drawLines){
    drawPolygonLines(circle_points, CIRCLE_NUM_POINTS, true);
  }
      
    
  if(drawHorizontalLines){
    static int tx[CIRCLE_NUM_POINTS]; // or some safe max; must >= CIRCLE_NUM_POINTS
    static int ty[CIRCLE_NUM_POINTS];

    for (int i = 0; i < CIRCLE_NUM_POINTS; i++) {
      tx[i] = (int)(worldCenterX + circle_points[i].x - cameraX);
      ty[i] = (int)(worldCenterY + circle_points[i].y - cameraY);
    }

    //--------------------------------------------------------------------------
    // 4. Fill the polygon by horizontal scan lines
    //--------------------------------------------------------------------------
    fillPolygonHorizontal(tx, ty, CIRCLE_NUM_POINTS);
  }
  
  // Draw planet as filled in triangles
  if (drawTriangles) {
      // 1. Convert all world-space points to screen-space **once** before the loop
      static int tx[CIRCLE_NUM_POINTS]; // Store transformed X coordinates
      static int ty[CIRCLE_NUM_POINTS]; // Store transformed Y coordinates

      for (int i = 0; i < CIRCLE_NUM_POINTS; i++) {
          tx[i] = (int)(worldCenterX + circle_points[i].x - cameraX);
          ty[i] = (int)(worldCenterY + circle_points[i].y - cameraY);
      }

      // 3. Iterate through edges and draw triangles
      for (int i = 0; i < CIRCLE_NUM_POINTS - 1; i++) {
          arduboy.fillTriangle(circleCenterX, circleCenterY, tx[i], ty[i], tx[i + 1], ty[i + 1], WHITE);
      }

      // 4. Close the loop: draw last triangle (avoids modulo)
      arduboy.fillTriangle(circleCenterX, circleCenterY, tx[CIRCLE_NUM_POINTS - 1], ty[CIRCLE_NUM_POINTS - 1], tx[0], ty[0], WHITE);
  }

  
    

}

void drawPlayerBullets(){
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) {
      // Convert bullet position from world coords to screen coords
      float bx = bullets[i].x - cameraX;
      float by = bullets[i].y - cameraY;
      // Draw a pixel or small circle for the bullet
      // arduboy.drawPixel((int)bx, (int)by, WHITE);
      arduboy.fillCircle((int)bx, (int)by, 1, WHITE);
      // Or arduboy.fillCircle((int)bx, (int)by, 1, WHITE);
    }
  }
}

void drawTurretBullets() {
  for (int i = 0; i < MAX_TURRET_BULLETS; i++) {
    if (turretBullets[i].active) {
      float bx = turretBullets[i].x - cameraX;
      float by = turretBullets[i].y - cameraY;
      // Draw as a small dot
      // arduboy.drawPixel((int)bx, (int)by, WHITE);
      arduboy.fillCircle((int)bx, (int)by, 1, WHITE);
      // Or arduboy.fillCircle((int)bx, (int)by, 1, WHITE);
    }
  }
}


void fillPolygonHorizontal(const int *px, const int *py, int n) {
  if (n < 3) return;

  // 1. Find bounding box in Y
  int minY = py[0];
  int maxY = py[0];
  for (int i = 1; i < n; i++) {
    if (py[i] < minY) minY = py[i];
    if (py[i] > maxY) maxY = py[i];
  }

  // Clamp to screen bounds [0..HEIGHT-1]
  if (minY < 0) minY = 0;
  if (maxY >= HEIGHT) maxY = HEIGHT - 1;

  // Temporary array for storing intersection X coords
  static int xIntersections[256]; // must be >= max number of edges

  // 2. For each scanline y in [minY .. maxY]
  for (int y = minY; y <= maxY; y++) {
    int numIntersections = 0;

    // Scan all edges
    for (int i = 0; i < n; i++) {
      int j = (i + 1) % n; // next vertex index

      // Retrieve edge endpoints
      int y1 = py[i], y2 = py[j];
      int x1 = px[i], x2 = px[j];

      // Skip horizontal edges
      if (y1 == y2) continue;

      // Order y1 <= y2
      if (y1 > y2) {
        int tmp = y1; y1 = y2; y2 = tmp;
        tmp = x1; x1 = x2; x2 = tmp;
      }

      // Check if this scanline intersects [y1..y2)
      if (y >= y1 && y < y2) {
        // Integer interpolation:
        // xHit = x1 + ((x2 - x1)*(y - y1)) / (y2 - y1)
        int dy1 = y - y1;
        int dy  = (y2 - y1);
        // Use 32-bit for multiplication to avoid 16-bit overflow
        int32_t dx = (int32_t)(x2 - x1) * (int32_t)dy1;

        int xHit = x1 + (int)(dx / dy);

        xIntersections[numIntersections++] = xHit;
      }
    }

    // If no intersections, skip
    if (numIntersections < 2) continue;

    // 2b. Sort intersection X coords (Insertion sort here for example)
    for (int i = 1; i < numIntersections; i++) {
      int key = xIntersections[i];
      int j = i - 1;
      while (j >= 0 && xIntersections[j] > key) {
        xIntersections[j + 1] = xIntersections[j];
        j--;
      }
      xIntersections[j + 1] = key;
    }

    // 2c. Draw horizontal line segments
    for (int i = 0; i < numIntersections - 1; i += 2) {
      int xStart = xIntersections[i];
      int xEnd   = xIntersections[i + 1];

      // Clip horizontally
      if (xEnd < 0 || xStart >= WIDTH) {
        continue;
      }
      if (xStart < 0) xStart = 0;
      if (xEnd >= WIDTH) xEnd = WIDTH - 1;

      arduboy.drawFastHLine(xStart, y, (xEnd - xStart + 1), WHITE);
    }
  }
}

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
  // Serial.println("Turret:");
  // Serial.println("Pos:");
  // Serial.print(turret->x);
  // Serial.print(" , ");
  // Serial.println(turret->y);
  // Serial.println("Corners:");
  // Serial.print(turret->p1.x); Serial.print(" , "); Serial.println(turret->p1.y);
  // Serial.print(turret->p2.x); Serial.print(" , "); Serial.println(turret->p2.y);
  // Serial.print(turret->p3.x); Serial.print(" , "); Serial.println(turret->p3.y);
  // Serial.print(turret->p4.x); Serial.print(" , "); Serial.println(turret->p4.y);


}
//My coordinates may be off
// I should try drawing an unrotated square first
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

// Generates an array of slightly irregular points forming a "random circle."
void randomCircle(int angle_step, float min_distance, float max_distance) {
  // We still produce approximately (360 / angle_step) points:
  // Point2D* points = new Point2D[CIRCLE_NUM_POINTS];

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

    // Create a point at the (slightly randomized) angle
    Point2D pt = randomPointAtAngle(angle, min_distance, max_distance);

    // Optionally add a small x/y offset
    pt.x += randomFloat(-2.0f, 2.0f);
    pt.y += randomFloat(-2.0f, 2.0f);

    circle_points[i] = pt;
    // if(i == 0){
    //   Serial.print(pt.x);
    //   Serial.print(" , ");
    //   Serial.println(pt.y);
    //     startX = pt.x;
    //     startY = pt.y;
    // }
  }
  // 1. Compute centroid
  float sumX = 0;
  float sumY = 0;
  for (int i = 0; i < CIRCLE_NUM_POINTS; i++) {
    sumX += circle_points[i].x;
    sumY += circle_points[i].y;
  }
  circleCenterX = (int)(float(sumX) / CIRCLE_NUM_POINTS);
  circleCenterY = (int)(float(sumY) / CIRCLE_NUM_POINTS);

  // Save fixedPoint version of the circle
  for (int i = 0; i < CIRCLE_NUM_POINTS; i++) {
      fixed_points[i].x = FP(circle_points[i].x);
      fixed_points[i].y = FP(circle_points[i].y);
  }

}

void generateFuelPickups(int numFuelPickups){
    pickupCount = 0;
    for (int i = 0; i < numFuelPickups; i++) {
        int pickupIndex = random(0, CIRCLE_NUM_POINTS);
        Point2D p1 = circle_points[pickupIndex];
        Point2D p2 = circle_points[(pickupIndex + 1) % CIRCLE_NUM_POINTS];

        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float edgeAngle = atan2(dy, dx);
        float pickupAngle = edgeAngle + PI / 2.0;

        Point2D pickupOffset = randomPointOnLine(p1, p2);
        float worldX = worldCenterX + pickupOffset.x;
        float worldY = worldCenterY + pickupOffset.y;

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
    float worldX = worldCenterX + turretOffset.x;
    float worldY = worldCenterY + turretOffset.y;

    turrets[turretCount].x = worldX;
    turrets[turretCount].y = worldY;
    turrets[turretCount].angle = turretAngle;
    // Start the turret’s timer so they don’t all fire at once
    turrets[turretCount].fireTimer = random(0, TURRET_FIRE_DELAY);
    turretCount++;
  }
  for (int i = 0; i < turretCount; i++) {
    float pivotX = turrets[i].x - cameraX; // pivot = planet perimeter
    float pivotY = turrets[i].y - cameraY;
    float angle  = turrets[i].angle;       // tangent + π/2 (or however you computed)

    setTurretCorners(&turrets[i], pivotX, pivotY, turretWidth, turretHeight);
  }

  // Also init all turret bullets as inactive
  for (int i = 0; i < MAX_TURRET_BULLETS; i++) {
    turretBullets[i].active = false;
  }

}

void drawAllFuelPickups(){
    for (int i = 0; i < pickupCount; i++) {
        float pivotX = fuelPickups[i].x - cameraX; // pivot = planet perimeter
        float pivotY = fuelPickups[i].y - cameraY;
        float angle  = fuelPickups[i].angle + PI/2;       // tangent + π/2 (or however you computed)

        float pickupW = 4.0;  // or whatever size
        float pickupH = 4.0;  // for example

        // Half-sizes
        float halfW = pickupW * 0.5;
        float halfH = pickupH * 0.5;

        // We want the rectangle's bottom edge at the pivot. 
        // In local coords, that bottom edge = +halfH.
        // So we shift the center by (0, -halfH) in local space, then rotate it.
        float offsetX =  (0.0f) * cos(angle) - (-halfH) * sin(angle);
        float offsetY =  (0.0f) * sin(angle) + (-halfH) * cos(angle);

        float centerX = pivotX + offsetX;
        float centerY = pivotY + offsetY;

        // Now draw the rectangle with its center at (centerX, centerY) and rotation = angle
        drawRotatedRect(centerX, centerY, pickupW, pickupH, angle);
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

//Use drawline instead of drawTriangle so you can skip one of the lines
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

  // arduboy.drawTriangle(ix1, iy1, ix2, iy2, ix3, iy3, WHITE);
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

// Draws the "ship" as a small triangle, given a screen position & angle.
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
    // Triangle points relative to ship center
    // x1 = 0,   y1 = -5;
    // x2 = -5,  y2 = 0;
    // x3 = 5,   y3 = 0;
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




