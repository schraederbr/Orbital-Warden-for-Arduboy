#include <Arduboy2.h>
#include <math.h>

void setup() {
  // Serial.begin(115200);
  arduboy.begin();
  // delay(2000);
  // testFixedPointInPolygon();
  // while(true){
  //     arduboy.pollButtons();
  //     if(arduboy.buttonsState()){
  //         break;
  //     }
  // }
  arduboy.setFrameRate(FRAME_RATE);
  //Add a start screen instead of this delay so I can use a button presss randomness to seed random.
  //This allows for random in both simular and hardware 
  randomSeed(micros()); 
  randomCircle(planetStepAngle, planetMinRadius, planetMaxRadius);
  
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
  resetShip();
  // Initialize bullets array
  for (int i = 0; i < MAX_BULLETS; i++) {
    bullets[i].active = false;
  }
}
void updateTurrets() {
  for (int t = 0; t < turretCount; t++) {
    // Decrement fireTimer
    turrets[t].fireTimer--;

    // If time to shoot, spawn a bullet
    if (turrets[t].fireTimer <= 0) {
      turrets[t].fireTimer = TURRET_FIRE_DELAY;

      // Only shoot if player is within range
      if (isWithinDistance(shipX, shipY, turrets[t].x, turrets[t].y, FP(TURRET_ACTIVE_DISTANCE))) {
        spawnTurretBullet(
          turrets[t].x,
          turrets[t].y,
          shipX,  // aim at player
          shipY
        );
      }
    }
  }
}
void updateTurretBullets() {
  for (int i = 0; i < MAX_TURRET_BULLETS; i++) {
    if (!turretBullets[i].active) continue;

    // Move bullet
    turretBullets[i].x += turretBullets[i].vx;
    turretBullets[i].y += turretBullets[i].vy;

    // If bullet hits planet
    if (pointInPolygon(circle_num_points, circle_points, turretBullets[i].x, turretBullets[i].y)) {
      turretBullets[i].active = false;
      turretBullets[i].framesAlive = 0;
      continue;
    }

    // If out of world
    if (turretBullets[i].x < FP(0) || turretBullets[i].x > worldWidth ||
        turretBullets[i].y < FP(0) || turretBullets[i].y > worldHeight) {
      turretBullets[i].active = false;
      turretBullets[i].framesAlive = 0;
      continue;
    }

    turretBullets[i].framesAlive++;
    if (turretBullets[i].framesAlive > MAX_TURRET_BULLET_FRAMES_ALIVE) {
      turretBullets[i].active = false;
      turretBullets[i].framesAlive = 0;
      continue;
    }

    // Check collision with player (unless shield is active)
    if (!arduboy.pressed(DOWN_BUTTON)) {
      FP dx = turretBullets[i].x - shipX;
      FP dy = turretBullets[i].y - shipY;
      FP distSq = dx*dx + dy*dy;

      // We'll use a fixed collision radius ~3
      FP collideRadius = FP(3.0f);
      if (distSq < collideRadius * collideRadius) {
        // Player was hit
        death();

        // Deactivate bullet
        turretBullets[i].active = false;
        turretBullets[i].framesAlive = 0;
      }
    }
  }
}

void resetShip(){
  // Reset ship to starting position in the middle of the world.
  shipX     = startX;
  shipY     = startY;  
  shipAngle = 0;
  velX      = 0;
  velY      = 0;
}

void resetBullets(){
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

void death() {
  lives--;
  if(lives <= 0){
    arduboy.clear();
    arduboy.setCursor(0,0);
    arduboy.println("Game Over!");
    arduboy.println("Final Score: ");
    arduboy.println(score);
    arduboy.display();
    delay(1000);
    lives = DEFAULT_LIVES;
    currentFuel = DEFAULT_FUEL;
    score = 0;

    arduboy.println("Press any button");
    arduboy.println("to restart");
    arduboy.display();
    
    while(true){
      arduboy.pollButtons();
      if(arduboy.buttonsState()){
        break;
      }
    }
    randomSeed(micros()); 
    randomCircle(planetStepAngle, planetMinRadius, planetMaxRadius);

    generateFuelPickups(NUM_FUEL_PICKUPS);
    generateTurrets(MAX_TURRETS);
    generateStars();
    
  }
  frames_alive = 0;
  resetShip();
  resetBullets();
}


void tractorBeam(){
  // Burn fuel
  currentFuel = currentFuel - ((float)TRACTOR_FUEL_BURN_RATE / (float)(FRAME_RATE));
  if (currentFuel <= 0) {
    return;
  }

  // We'll define a local 3-vertex shape in ship coords
  // Then rotate+translate to world
  FP localX1 = FP(-15), localY1 = FP(25);
  FP localX2 = FP(  0), localY2 = FP( 0);
  FP localX3 = FP( 15), localY3 = FP(25);

  // We need float for sin/cos
  float angleF = float(shipAngle);
  float c = cos(angleF);
  float s = sin(angleF);

  // Rotate & translate
  FP beamX1 = shipX + FP(localX1*c - localY1*s);
  FP beamY1 = shipY + FP(localX1*s + localY1*c);

  FP beamX2 = shipX + FP(localX2*c - localY2*s);
  FP beamY2 = shipY + FP(localX2*s + localY2*c);

  FP beamX3 = shipX + FP(localX3*c - localY3*s);
  FP beamY3 = shipY + FP(localX3*s + localY3*c);

  // Check all fuel pickups
  for (int i = 0; i < pickupCount; ) {
    if (pointInTriangle(
          fuelPickups[i].x, fuelPickups[i].y,
          beamX1, beamY1, beamX2, beamY2, beamX3, beamY3
        )) {
      // Collect fuel
      currentFuel += FUEL_PER_PICKUP;
      score += FUEL_PICKUP_SCORE;

      // Remove pickup
      pickupCount--;
      fuelPickups[i] = fuelPickups[pickupCount];
      // do NOT i++ so we re-check the swapped one
    } else {
      i++;
    }
  }
}

void updateBullets(){
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (!bullets[i].active) continue;

    bullets[i].framesAlive++;
    bullets[i].x += bullets[i].vx;
    bullets[i].y += bullets[i].vy;

    // If bullet hits planet
    if (pointInPolygon(circle_num_points, circle_points, bullets[i].x, bullets[i].y)) {
      bullets[i].active = false;
      bullets[i].framesAlive = 0;
      continue;
    }

    // Out of world
    if (bullets[i].x < FP(0) || bullets[i].x > worldWidth ||
        bullets[i].y < FP(0) || bullets[i].y > worldHeight) {
      bullets[i].active = false;
      bullets[i].framesAlive = 0;
      continue;
    }

    if (bullets[i].framesAlive > MAX_BULLET_FRAMES_ALIVE) {
      bullets[i].active = false;
      bullets[i].framesAlive = 0;
      continue;
    }

    // Check turret collision
    for (int t = 0; t < turretCount; t++) {
      if (pointInRectangle(bullets[i].x, bullets[i].y, &turrets[t])) {
        score += TURRET_SCORE;
        bullets[i].active = false;

        turretCount--;
        turrets[t] = turrets[turretCount]; // swap
        break;
      }
    }
  }
}

void checkPlanetComplete(){
  if (turretCount <= 0 && pickupCount <= 0) {
    arduboy.clear();
    arduboy.setCursor(0,0);
    arduboy.println("Planet Complete!");
    arduboy.print("Current score: ");
    arduboy.println(score);
    arduboy.display();
    delay(1000);
    arduboy.println("Press any button");
    arduboy.println("to start new planet");
    arduboy.display();

    while (true) {
      arduboy.pollButtons();
      if (arduboy.buttonsState()) {
        break;
      }
    }

    randomSeed(micros());
    randomCircle(planetStepAngle, planetMinRadius, planetMaxRadius);

    generateFuelPickups(NUM_FUEL_PICKUPS);
    generateTurrets(MAX_TURRETS);
    generateStars();

    frames_alive = 0;
    resetShip();
    resetBullets();
  }
}

void loop() {
  if (!arduboy.nextFrame()) return;
  arduboy.pollButtons();
  arduboy.clear();
  frames_alive++;

  // 1. Ship rotation
  if (arduboy.pressed(LEFT_BUTTON)) {
    shipAngle -= ROTATION_SPEED;
  }
  if (arduboy.pressed(RIGHT_BUTTON)) {
    shipAngle += ROTATION_SPEED;
  }

  // 2. Thrust
  if (arduboy.pressed(A_BUTTON)) {
    if (currentFuel > 0) {
      // direction = shipAngle - PI/2, but we store shipAngle as FP
      float angleF = float(shipAngle - FP(M_PI/2));
      float c = cos(angleF);
      float s = sin(angleF);

      velX += FP(c) * ACCELERATION;
      velY += FP(s) * ACCELERATION;

      currentFuel -= (float)THRUST_FUEL_BURN_RATE / (float)(FRAME_RATE);
    }
  }

  // 3. Gravity
  FP planetCenterX = worldCenterX + circleCenterX;
  FP planetCenterY = worldCenterY + circleCenterY;
  FP dx = planetCenterX - shipX;
  FP dy = planetCenterY - shipY;

  // Convert to float for sqrt
  float distF = sqrt(float(dx*dx + dy*dy));
  if (distF > 0.0f) {
    // Normalize in FP
    dx /= FP(distF);
    dy /= FP(distF);
  }
  // velX += dx * GRAVITY_ACCEL;
  // velY += dy * GRAVITY_ACCEL;

  // 4. Update ship position, apply friction
  shipX += velX;
  shipY += velY;
  velX   *= FRICTION;
  velY   *= FRICTION;

  // Clamp ship in world
  if (shipX < FP(0))         shipX = FP(0);
  if (shipX > worldWidth)    shipX = worldWidth;
  if (shipY < FP(0))         shipY = FP(0);
  if (shipY > worldHeight)   shipY = worldHeight;

  // 5. Camera
  cameraX = shipX - FP(screenWidth / 2);
  cameraY = shipY - FP(screenHeight / 2);
  if (cameraX < FP(0)) cameraX = FP(0);
  if (cameraY < FP(0)) cameraY = FP(0);

  // clamp camera
  FP maxCamX = worldWidth  - FP(screenWidth);
  FP maxCamY = worldHeight - FP(screenHeight);
  if (cameraX > maxCamX) cameraX = maxCamX;
  if (cameraY > maxCamY) cameraY = maxCamY;

  // 6. Fire bullets on B press
  if (arduboy.justPressed(B_BUTTON)) {
    spawnBullet(shipX, shipY, shipAngle);
  }

  // 7. Update all bullets
  updateBullets();

  // 8. Tractor beam if holding DOWN
  if (arduboy.pressed(DOWN_BUTTON)) {
    tractorBeam();
  }

  // 9. Turrets
  updateTurrets();
  updateTurretBullets();

  // 10. Draw everything
  drawStars();
  drawAllTurrets();
  drawTurretBullets();
  drawPlanet(true, false, false, false);  // example planet drawing
  drawAllFuelPickups();
  drawPlayerBullets();

  // Draw the ship
  FP screenShipX = shipX - cameraX;
  FP screenShipY = shipY - cameraY;
  drawShip(
    /*smallShip=*/true,
    /*simpleStyle=*/true,
    screenShipX,
    screenShipY,
    shipAngle
  );

  // If ship is inside the planet polygon -> death
  if (pointInPolygon(circle_num_points, circle_points, shipX, shipY)) {
    arduboy.print("HIT PLANET!");
    death();
  }

  // Print HUD
  arduboy.setCursor(0,0);
  arduboy.print(lives);

  arduboy.setCursorX(24);
  if (currentFuel > 0) {
    arduboy.print(float(currentFuel), 1);  // e.g. 1 decimal place
  } else {
    arduboy.print("EMPTY");
  }

  arduboy.setCursorX(88);
  arduboy.println(score);
  arduboy.display();

  checkPlanetComplete();
}