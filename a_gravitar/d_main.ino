#include <Arduboy2.h>
#include <math.h>

void setup() {
  Serial.begin(9600);
  delay(2000);
  arduboy.begin();
  arduboy.setFrameRate(FRAME_RATE);
  //Add a start screen instead of this delay so I can use a button presss randomness to seed random.
  //This allows for random in both simular and hardware
  delay(1);  
  randomSeed(micros()); 
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
  resetShip();
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
      if(isWithinDistance(shipX, shipY, turrets[t].x, turrets[t].y, TURRET_ACTIVE_DISTANCE)){
        spawnTurretBullet(turrets[t].x, 
                          turrets[t].y, 
                          shipX,    // player's position
                          shipY);
      }
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
      //Down button means the shield is active
      if(!arduboy.pressed(DOWN_BUTTON)){
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
}

void resetShip(){
  // Reset ship to starting position in the middle of the world.
  shipX     = worldWidth / 2;
  shipY     = planetMaxRadius + 10;  
  shipAngle = 0;
  velX      = 0;
  velY      = 0;
}

void death() {
  lives--;
  if(lives <= 0){
    lives = DEFAULT_LIVES;
    currentFuel = DEFAULT_FUEL;
    score = 0;
    arduboy.clear();
    arduboy.setCursor(10, 30);
    arduboy.println("Game Over!");
    arduboy.println("Press any button");
    arduboy.println("to restart");
    arduboy.display();
    while(true){
      arduboy.pollButtons();
      if(arduboy.buttonsState()){
        break;
      }
    }
    
  }

  resetShip();


  randomSeed(micros()); 
  circle_points = randomCircle(planetStepAngle, planetMinRadius, planetMaxRadius);

  generateFuelPickups(NUM_FUEL_PICKUPS);
  generateTurrets(MAX_TURRETS);
  generateStars();

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


void tractorBeam(){
  currentFuel -= TRACTOR_FUEL_BURN_RATE / FRAME_RATE;

  // Tractor beam triangle in ship-local coordinates
  float localX1 = -15, localY1 = 25;
  float localX2 = 0,   localY2 = 0;
  float localX3 = 15,  localY3 = 25;
  
  // Compute rotation factors
  float cosA = cos(shipAngle);
  float sinA = sin(shipAngle);
  
  // Rotate and translate the points into world coordinates
  float beamX1 = shipX + (localX1 * cosA - localY1 * sinA);
  float beamY1 = shipY + (localX1 * sinA + localY1 * cosA);
  
  float beamX2 = shipX + (localX2 * cosA - localY2 * sinA);
  float beamY2 = shipY + (localX2 * sinA + localY2 * cosA);
  
  float beamX3 = shipX + (localX3 * cosA - localY3 * sinA);
  float beamY3 = shipY + (localX3 * sinA + localY3 * cosA);
  
  // Check all fuel pickups to see if they lie within the tractor beam triangle
  for (int i = 0; i < pickupCount; ) { 
    if (pointInTriangle(fuelPickups[i].x, fuelPickups[i].y, beamX1, beamY1, beamX2, beamY2, beamX3, beamY3)) {
      // Add fuel to the ship
      currentFuel += FUEL_PER_PICKUP;
      
      // Replace the removed pickup with the last one in the array
      fuelPickups[i] = fuelPickups[pickupCount - 1];
      pickupCount--;

      // Do NOT increment i here to recheck the swapped pickup
    } else {
      i++; // Only increment if no deletion occurred
    }
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
  //Thrusting
  if (arduboy.pressed(A_BUTTON)) {
    if(currentFuel > 0){
      velX += cos(shipAngle - PI / 2) * ACCELERATION;
      velY += sin(shipAngle - PI / 2) * ACCELERATION;
      currentFuel -= THRUST_FUEL_BURN_RATE / FRAME_RATE;
    }

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

  //Making gravity acceleration constant
  // if(dx > 0)
  //   dx = 1;
  // else
  //   dx = -1;
  // if(dy > 0)
  //   dy = 1;
  // else
  //   dy = -1;


  velX += dx * GRAVITY_ACCEL;
  velY += dy * GRAVITY_ACCEL;
  // velX += GRAVITY_ACCEL;
  // velY += GRAVITY_ACCEL;


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
          //These corners aren't right,  they aren't rotated with the turret
          // if(pointInRectangle(
          //   bullets[i].x, bullets[i].y,
          //   turrets[t].x - 2, turrets[t].y - 4,  // Corner 1
          //   turrets[t].x + 2, turrets[t].y - 4,  // Corner 2
          //   turrets[t].x + 2, turrets[t].y + 4,  // Corner 3
          //   turrets[t].x - 2, turrets[t].y + 4   // Corner 4
          // )
          // ){
          //   // Deactivate bullet
          //   bullets[i].active = false;
          //   // Destroy turret
          //   turretCount--;
          //   turrets[t] = turrets[turretCount];
          //   break;
          // }
          if(isWithinDistance(bullets[i].x, bullets[i].y, turrets[t].x, turrets[t].y, 4)){
            // Destroy bullet
            score += TURRET_SCORE;
            bullets[i].active = false;
            turretCount--;
            turrets[t] = turrets[turretCount];
            break;
          }
        }
      }
    }
  }

  if (arduboy.pressed(DOWN_BUTTON)){
    tractorBeam();
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


  drawPlayerBullets();
  // Draw the ship
  float screenShipX = shipX - cameraX;
  float screenShipY = shipY - cameraY;
  drawShip(true, true, screenShipX, screenShipY, shipAngle);
  if (pointInPlanet(shipX, shipY)) {
    arduboy.print("HIT PLANET!");
    death();
  }
  arduboy.print(lives);
  arduboy.setCursorX(24);
  if(currentFuel > 0){
    arduboy.print(currentFuel);
  }
  else{
    arduboy.print("EMPTY");
  }
  arduboy.setCursorX(88);
  arduboy.println(score);
  arduboy.setCursorX(0);
  // arduboy.print(shipX);
  // arduboy.print(" , ");
  // arduboy.print(shipY);
  // font3x5.print(lives);
  arduboy.display();
}
