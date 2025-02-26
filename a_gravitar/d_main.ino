#include <Arduboy2.h>
#include <math.h>

void setup() {
  Serial.begin(9600);
  delay(2000);
  arduboy.begin();
  arduboy.setFrameRate(120);

  randomSeed(analogRead(0));
  circle_points = randomCircle(planetStepAngle, planetMinRadius, planetMaxRadius, circle_num_points);
  
  // for (int i = 0; i < circle_num_points; i++) {
  //   Serial.print("Point ");
  //   Serial.print(i);
  //   Serial.print(": (");
  //   Serial.print(circle_points[i].x);
  //   Serial.print(", ");
  //   Serial.print(circle_points[i].y);
  //   Serial.println(")");
  // }
  generateTurrets(MAX_TURRETS);
  generateStars();
  // Start the ship somewhere in the world. For demonstration, let's put it in the middle
  shipX     = worldWidth / 2;   
  shipY     = worldHeight / 2;  
  shipAngle = 0;
  velX      = 0;
  velY      = 0;

  // Initialize bullets array
  for (int i = 0; i < MAX_BULLETS; i++) {
    bullets[i].active = false;
  }
}

void loop() {
  if (!arduboy.nextFrame()) return;
  arduboy.pollButtons();
  arduboy.clear();

  // --- 1. Ship Controls ---
  if (arduboy.pressed(LEFT_BUTTON)) {
    shipAngle -= ROTATION_SPEED;
  }
  if (arduboy.pressed(RIGHT_BUTTON)) {
    shipAngle += ROTATION_SPEED;
  }
  if (arduboy.pressed(A_BUTTON)) {
    velX += cos(shipAngle - PI / 2) * ACCELERATION;
    velY += sin(shipAngle - PI / 2) * ACCELERATION;
  }

  // Update ship
  shipX += velX;
  shipY += velY;
  velX   *= FRICTION;
  velY   *= FRICTION;

  // Clamp ship in world
  if (shipX < 0) shipX = 0;
  if (shipX > worldWidth)  shipX = worldWidth;
  if (shipY < 0) shipY = 0;
  if (shipY > worldHeight) shipY = worldHeight;

  // --- 2. Camera ---
  cameraX = shipX - (screenWidth / 2);
  cameraY = shipY - (screenHeight / 2);
  if (cameraX < 0) cameraX = 0;
  if (cameraX > (worldWidth  - screenWidth))  cameraX = worldWidth  - screenWidth;
  if (cameraY < 0) cameraY = 0;
  if (cameraY > (worldHeight - screenHeight)) cameraY = worldHeight - screenHeight;

  // --- 3. Shooting Bullets on B Press ---
  if (arduboy.justPressed(B_BUTTON)) {
    // Instead of re-generating planet, spawn a bullet
    spawnBullet(shipX, shipY, shipAngle);
  }

  // --- 4. Update Bullets ---
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) {
      // Move bullet
      bullets[i].x += bullets[i].vx;
      bullets[i].y += bullets[i].vy;

      // Optionally deactivate if off-screen or out of world
      if (bullets[i].x < 0 || bullets[i].x > worldWidth ||
          bullets[i].y < 0 || bullets[i].y > worldHeight) {
        bullets[i].active = false;
      }
    }
  }

  // Check collision with turrets
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) {
      // 1. Move the bullet
      bullets[i].x += bullets[i].vx;
      bullets[i].y += bullets[i].vy;

      // 2. Deactivate if off-screen or out of world
      if (bullets[i].x < 0 || bullets[i].x > worldWidth ||
          bullets[i].y < 0 || bullets[i].y > worldHeight) {
        bullets[i].active = false;
      }
      else {
        // 3. Check collision with each turret
        for (int t = 0; t < MAX_TURRETS; t++) {
          float dx = bullets[i].x - turrets[t].x;
          float dy = bullets[i].y - turrets[t].y;
          // Distance squared
          float distSq = dx * dx + dy * dy;

          // Compare to a threshold radius squared.
          // If your turrets are drawn 4×4, a radius of ~2 might suffice.
          // For safety, let's use 16 (so radius = 4).
          if (distSq < 22.0f) {
            // Destroy bullet
            bullets[i].active = false;
            turretCount--;
            turrets[t] = turrets[turretCount];
            // If you ALSO want the turret destroyed, 
            // add code here like:
            // turrets[t].active = false; // (You'd need an 'active' flag for turrets too.)

            // Break out of the turret loop
            break;
          }
        }
      }
    }
  }

  // --- 5. Draw Everything ---

  // Stars (background, parallax)
  drawStars();

  // Planet circle (optional regen with some other input if you like)
  if (circle_points != nullptr) {
    for (int i = 0; i < circle_num_points; i++) {
      int x = (int)((worldCenterX + circle_points[i].x) - cameraX);
      int y = (int)((worldCenterY + circle_points[i].y) - cameraY);
      arduboy.fillCircle(x, y, 1, WHITE);
    }
    drawLines(circle_points, circle_num_points, true);
  }

  drawAllTurrets();

  // Draw Bullets
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) {
      // Convert bullet position from world coords to screen coords
      float bx = bullets[i].x - cameraX;
      float by = bullets[i].y - cameraY;
      // Draw a pixel or small circle for the bullet
      arduboy.drawPixel((int)bx, (int)by, WHITE);
      // Or arduboy.fillCircle((int)bx, (int)by, 1, WHITE);
    }
  }

  // Draw the ship
  float screenShipX = shipX - cameraX;
  float screenShipY = shipY - cameraY;
  drawShip(screenShipX, screenShipY, shipAngle);

  arduboy.display();
}
