#include <Arduboy2.h>
#include <math.h>

void setup() {
  Serial.begin(9600);
  delay(2000);
  arduboy.begin();
  arduboy.setFrameRate(FRAME_RATE);

  randomSeed(analogRead(0));
  circle_points = randomCircle(planetStepAngle, planetMinRadius, planetMaxRadius);
  
  // for (int i = 0; i < circle_num_points; i++) {
  //   Serial.print("Point ");
  //   Serial.print(i);
  //   Serial.print(": (");
  //   Serial.print(circle_points[i].x);
  //   Serial.print(", ");
  //   Serial.print(circle_points[i].y);
  //   Serial.println(")");
  // }
  generateFuelPickups(NUM_FUEL_PICKUPS);
  generateTurrets(MAX_TURRETS);
  generateStars();
  // Start the ship somewhere in the world. For demonstration, let's put it in the middle
  shipX     = worldWidth / 2;   
  shipY     = 0;  
  shipAngle = 0;
  velX      = 0;
  velY      = 0;

  // Initialize bullets array
  for (int i = 0; i < MAX_BULLETS; i++) {
    bullets[i].active = false;
  }
}
void updateTurrets() {
  // For each turret
  for (int t = 0; t < turretCount; t++) {
    // Decrement fireTimer
    turrets[t].fireTimer--;

    // If time to shoot, spawn a bullet
    if (turrets[t].fireTimer <= 0) {
      // reset timer
      turrets[t].fireTimer = TURRET_FIRE_DELAY;
      // aim at player
      spawnTurretBullet(turrets[t].x, 
                        turrets[t].y, 
                        shipX,    // player's position
                        shipY);
    }
  }
}

void updateTurretBullets() {
  // if (gameOver) return; // no need if game stops updating on gameOver

  for (int i = 0; i < MAX_TURRET_BULLETS; i++) {
    if (turretBullets[i].active) {
      // Move bullet
      turretBullets[i].x += turretBullets[i].vx;
      turretBullets[i].y += turretBullets[i].vy;

      // Check if out of world, deactivate
      if (turretBullets[i].x < 0 || turretBullets[i].x > worldWidth ||
          turretBullets[i].y < 0 || turretBullets[i].y > worldHeight) {
        turretBullets[i].active = false;
        continue;
      }

      // Check collision with player
      // Suppose the player's radius is ~3 or 4, or just do a point collision 
      // if your ship is small. We'll do a small threshold:
      float dx = turretBullets[i].x - shipX;
      float dy = turretBullets[i].y - shipY;
      float distSq = dx*dx + dy*dy;
      float collideRadius = 5.0f;   // tweak as needed
      if (distSq < (collideRadius * collideRadius)) {
        // Hit the player!
        death();
        // gameOver = true;
        // Deactivate bullet
        turretBullets[i].active = false;
        // Optionally break out altogether,
        // or just let the rest keep updating.
      }
    }
  }
}

void death() {
  // Optionally display a brief game-over message.
  lives--;
  if(lives <= 0){
    lives = DEFAULT_LIVES;
    arduboy.clear();
    arduboy.setCursor(10, 30);
    arduboy.print("Game Over!");
    arduboy.display();
    delay(5000); // Pause briefly before restarting
  }


  // Reset ship to starting position in the middle of the world.
  shipX     = worldWidth / 2;
  shipY     = 0;
  shipAngle = 0;
  velX      = 0;
  velY      = 0;

  // Reset bullets.
  for (int i = 0; i < MAX_BULLETS; i++) {
    bullets[i].active = false;
  }
  
  // Reset turret timers (so they don't all fire immediately)
  for (int t = 0; t < turretCount; t++) {
    turrets[t].fireTimer = random(0, TURRET_FIRE_DELAY);
  }

  for (int i = 0; i < MAX_TURRET_BULLETS; i++) {
    turretBullets[i].active = false;
  }
}

void drawTurretBullets() {
  for (int i = 0; i < MAX_TURRET_BULLETS; i++) {
    if (turretBullets[i].active) {
      float bx = turretBullets[i].x - cameraX;
      float by = turretBullets[i].y - cameraY;
      // Draw as a small dot
      arduboy.drawPixel((int)bx, (int)by, WHITE);
      // Or arduboy.fillCircle((int)bx, (int)by, 1, WHITE);
    }
  }
}

void tractorBeam(){
    
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
  //Thrusting
  if (arduboy.pressed(A_BUTTON)) {
    velX += cos(shipAngle - PI / 2) * ACCELERATION;
    velY += sin(shipAngle - PI / 2) * ACCELERATION;
    currentFuel -= THRUST_FUEL_BURN_RATE / FRAME_RATE;
  }

// --- 1.1 Apply Gravity ---
  // Compute the planet's center in world coordinates.
  float planetCenterX = worldCenterX + circleCenterX;
  float planetCenterY = worldCenterY + circleCenterY;

  // Compute vector from ship to planet center.
  float dx = planetCenterX - shipX;
  float dy = planetCenterY - shipY;
  float distance = sqrt(dx * dx + dy * dy);
  if (distance > 0) {  // Normalize the vector to avoid division by zero.
    dx /= distance;
    dy /= distance;
  }
  const float GRAVITY_ACCEL = 0.01f;  // Adjust this constant to change gravity strength.
  velX += dx * GRAVITY_ACCEL;
  velY += dy * GRAVITY_ACCEL;


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
          if (distSq < 22.0f) {
            // Destroy bullet
            bullets[i].active = false;
            turretCount--;
            turrets[t] = turrets[turretCount];
            break;
          }
        }
      }
    }
  }
  updateTurrets();
  updateTurretBullets();
  // --- 5. Draw Everything ---

  // Stars (background, parallax)
  drawStars();
  drawTurretBullets();

  // Planet circle (optional regen with some other input if you like)
  drawPlanet(true, false, false, false);

  drawAllTurrets();
  drawAllFuelPickups();

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
  drawShip(false, screenShipX, screenShipY, shipAngle);
  if (pointInPlanet(shipX, shipY)) {
    arduboy.print("HIT PLANET!");
    death();
  }
  arduboy.print(lives);
  arduboy.setCursorX(64);
  arduboy.print(currentFuel);
  arduboy.setCursorX(0);
  // font3x5.print(lives);
  arduboy.display();
}
