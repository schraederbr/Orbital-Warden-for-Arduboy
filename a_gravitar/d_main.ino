#include <Arduboy2.h>
#include <math.h>

// #define DEBUG


unsigned long stepStart;
unsigned long stepDuration;



void drawDpad(int x, int y){
  // Offset
  int o = 9;
  // Size
  int s = 7;
  arduboy.fillRect(x - o, y, s, s);
  arduboy.fillTriangle(
    x - o + s - 1,        y,              
    x - o + s - 1,        y + s - 1,     
    x - o + s + (s / 2) - 1,        y + s / 2       
  );
  arduboy.fillRect(x, y + o, s, s);
  arduboy.fillTriangle(
    x,            y + o,      
    x + s - 1,    y + o,      
    x + s / 2,    y + o - (s / 2) 
  );

  arduboy.fillRect(x + o, y, s, s);
  arduboy.fillTriangle(
    x + o,            y,      
    x + o,    y + s - 1,      
    x + o - s / 2,    y + (s / 2)
  );

  // Skip the top button because we don't use it
  // arduboy.drawRect(x, y - o, s, s);
  // arduboy.fillTriangle(
  //   x,            y - o + s - 1,      
  //   x + s - 1,    y - o + s - 1,      
  //   x + s / 2,    y - o + s + (s / 2) - 1 
  // );

}

void startScreen(){
  arduboy.clear();
  font3x5.println("Destroy Turrets     250 points");
  arduboy.drawRect(67, 1, 4, 6);

  font3x5.println("Collect Fuel     100 points");
  arduboy.fillCircle(57, 11, 3);

  Sprites::drawSelfMasked(0, 16, title, 0);

  font3x5.setCursor(105, 25);
  font3x5.println("Shoot");
  arduboy.fillCircle(90, 28, 4);
  arduboy.drawLine(97, 28, 102, 28);

  font3x5.setCursor(96, 35);
  font3x5.println("Thrust");
  arduboy.fillCircle(80, 38, 4);
  arduboy.drawLine(88, 38, 93, 38);
  
  
  font3x5.setCursor(45, 49);
  font3x5.println("rotate left or right");
  arduboy.drawLine(36, 52, 41, 52);

  font3x5.setCursor(36, 57);
  font3x5.println("shield and tractor beam");
  arduboy.drawLine(27, 60, 32, 60);

  drawDpad(16, 48);

  arduboy.display();
  waitForPress();
}

void setup() {
  #ifdef DEBUG
  Serial.begin(115200);
  #endif
  arduboy.begin();
  arduboy.setFrameRate(FRAME_RATE);
  startScreen();
  randomSeed(micros()); 
  generatePlanet(PLANET_STEP_ANGLE, PLANET_MIN_RADIUS, PLANET_MAX_RADIUS);
  
  generateFuelPickups(NUM_FUEL_PICKUPS);
  generateTurrets(MAX_TURRETS);
  generateStars();
  resetShip();

  // Initialize bullets array
  for (int i = 0; i < MAX_BULLETS; i++) {
    bullets[i].active = false;
  }
}
void updateTurrets() {
  for (int t = 0; t < turretCount; t++) {
    turrets[t].fireTimer--;

    // If time to shoot, spawn a bullet
    if (turrets[t].fireTimer <= 0) {
      turrets[t].fireTimer = TURRET_FIRE_DELAY;
      // aim at player
      if(isWithinDistance(shipX, shipY, turrets[t].x, turrets[t].y, TURRET_ACTIVE_DISTANCE)){
        spawnTurretBullet(turrets[t].x, 
                          turrets[t].y, 
                          shipX,    
                          shipY);
      }
    }
  }
}

void updateTurretBullets() {

  for (int i = 0; i < MAX_TURRET_BULLETS; i++) {
    if (turretBullets[i].active) {
      // Move bullet
      turretBullets[i].x += turretBullets[i].vx;
      turretBullets[i].y += turretBullets[i].vy;
      stepStart = micros();
      if (pointInPolygon(CIRCLE_NUM_POINTS, circle_points, turretBullets[i].x, turretBullets[i].y)) {
        turretBullets[i].active = false;
        turretBullets[i].framesAlive = 0;
      }
      stepDuration = micros() - stepStart;
      #ifdef DEBUG
      Serial.print(F("  pointInPolygon() took: "));
      Serial.print(stepDuration);
      Serial.println(F("us"));
      #endif
      // Check if out of world, deactivate
      if (turretBullets[i].x < 0 || turretBullets[i].x > WORLD_WIDTH ||
          turretBullets[i].y < 0 || turretBullets[i].y > WORLD_HEIGHT) {
        turretBullets[i].active = false;
        turretBullets[i].framesAlive = 0;
        continue;
      }
      if(turretBullets[i].framesAlive > MAX_TURRET_BULLET_FRAMES_ALIVE){
        turretBullets[i].active = false;
        turretBullets[i].framesAlive = 0;
      }
      //Down button means the shield is active
      if(!arduboy.pressed(DOWN_BUTTON)){
        // Check collision with player
        // May want to use a polygon check instead of circle
        float dx = turretBullets[i].x - shipX;
        float dy = turretBullets[i].y - shipY;
        float distSq = dx*dx + dy*dy;
        float collideRadius = 3.0f;   // tweak as needed
        if (distSq < (collideRadius * collideRadius)) {
          // Hit the player!
          death();
          // Deactivate bullet
          turretBullets[i].active = false;
        }
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
    arduboy.println();
    arduboy.println("Press any button");
    arduboy.println("to restart");
    arduboy.display();
    
    waitForPress();

    randomSeed(micros()); 
    generatePlanet(PLANET_STEP_ANGLE, PLANET_MIN_RADIUS, PLANET_MAX_RADIUS);

    generateFuelPickups(NUM_FUEL_PICKUPS);
    generateTurrets(MAX_TURRETS);
    generateStars();
    
  }
  frames_alive = 0;
  resetShip();
  resetBullets();
}


void tractorBeam(){
  currentFuel -= TRACTOR_FUEL_BURN_RATE / FRAME_RATE;
  if(currentFuel > 0){
    // Tractor beam triangle in ship-local coordinates
    float localX1 = -15, localY1 = 25;
    float localX2 = 0,   localY2 = 0;
    float localX3 = 15,  localY3 = 25;
    
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

        currentFuel += FUEL_PER_PICKUP;
        score += FUEL_PICKUP_SCORE;
        // Replace the removed pickup with the last one in the array
        fuelPickups[i] = fuelPickups[pickupCount - 1];
        pickupCount--;
      } else {
        i++; 
      }
    }
  }
}

void updateBullets(){
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) {
      bullets[i].framesAlive++;
      // Move bullet
      bullets[i].x += bullets[i].vx;
      bullets[i].y += bullets[i].vy;
      for (int t = 0; t < MAX_TURRETS; t++) {
        stepStart = micros();
        if(pointInRectangle(bullets[i].x, bullets[i].y, &turrets[t])){
          // Destroy bullet
          score += TURRET_SCORE;
          bullets[i].active = false;
          // Destroy Turret
          turretCount--;
          turrets[t] = turrets[turretCount];
        }
        stepDuration = micros() - stepStart;
        #ifdef DEBUG
        Serial.print(F("  pointInRectangle() took: "));
        Serial.print(stepDuration);
        Serial.println(F("us"));
        #endif
      }
      // Deactivate if off-screen or out of world
      if (bullets[i].x < 0 || bullets[i].x > WORLD_WIDTH ||
          bullets[i].y < 0 || bullets[i].y > WORLD_HEIGHT) {
        bullets[i].active = false;
        bullets[i].framesAlive = 0;
        continue;
      }
      if(bullets[i].framesAlive > MAX_BULLET_FRAMES_ALIVE){
        bullets[i].active = false;
        bullets[i].framesAlive = 0;
        continue;
      }
      //If bullet hits planet, delete
      stepStart = micros();
      if (pointInPolygon(CIRCLE_NUM_POINTS, circle_points, bullets[i].x, bullets[i].y)) {
        bullets[i].active = false;
        bullets[i].framesAlive = 0;
      }
      stepDuration = micros() - stepStart;
      #ifdef DEBUG
      Serial.print(F("  pointInPolygon() took: "));
      Serial.print(stepDuration);
      Serial.println(F("us"));
      #endif

    }
  }
}

void checkPlanetComplete(){
  if(turretCount <= 0 && pickupCount <= 0){
    arduboy.clear();
    arduboy.setCursor(0, 0);
    arduboy.println("Turrets Destroyed!");
    arduboy.println("Current score: " );
    arduboy.println(score);
    arduboy.display();
    delay(1000);
    arduboy.println();
    arduboy.println("Press any button");
    arduboy.println("to start new planet");
    arduboy.display();
    
    while(true){
      arduboy.pollButtons();
      if(arduboy.buttonsState()){
        break;
      }
    }
    randomSeed(micros()); 
    generatePlanet(PLANET_STEP_ANGLE, PLANET_MIN_RADIUS, PLANET_MAX_RADIUS);

    generateFuelPickups(NUM_FUEL_PICKUPS);
    generateTurrets(MAX_TURRETS);
    generateStars();
      
    frames_alive = 0;
    resetShip();
    resetBullets();
  }
}

void drawLives(){
  for (int i = 0; i < lives; i++) {
    Sprites::drawSelfMasked(i * 6, 0, shipSprite, 0);
  }
}

void loop() {
  // Measure the start of this frame in microseconds
  unsigned long frameStart = micros();

  // If Arduboy won't render a new frame, skip
  if (!arduboy.nextFrame()) return;

  unsigned long stepStart, stepDuration;

  stepStart = micros();
  arduboy.pollButtons();
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("pollButtons() took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));
#endif

  stepStart = micros();
  arduboy.clear();
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("arduboy.clear() took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));
#endif

  frames_alive++;

  // Handle ship controls (rotation, thrust)
  stepStart = micros();
  if (arduboy.pressed(LEFT_BUTTON)) {
    shipAngle -= ROTATION_SPEED;
  }
  if (arduboy.pressed(RIGHT_BUTTON)) {
    shipAngle += ROTATION_SPEED;
  }
  if (arduboy.pressed(A_BUTTON)) {
    if (currentFuel > 0) {
      velX += cos(shipAngle - PI / 2) * ACCELERATION;
      velY += sin(shipAngle - PI / 2) * ACCELERATION;
      currentFuel -= THRUST_FUEL_BURN_RATE / FRAME_RATE;
    }
  }
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("Ship controls took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));
#endif

  // Apply gravity
  stepStart = micros();
  float planetCenterX = WORLD_CENTER_X + circleCenterX;
  float planetCenterY = WORLD_CENTER_Y + circleCenterY;
  float dx = planetCenterX - shipX;
  float dy = planetCenterY - shipY;
  float distance = sqrt(dx * dx + dy * dy);
  if (distance > 0) {
    dx /= distance;
    dy /= distance;
  }
  velX += dx * GRAVITY_ACCEL;
  velY += dy * GRAVITY_ACCEL;
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("Gravity calc took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));
#endif

  // Ship update + friction
  stepStart = micros();
  shipX += velX;
  shipY += velY;
  velX   *= FRICTION;
  velY   *= FRICTION;
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("Ship update + friction took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));
#endif

  // Clamp ship position
  stepStart = micros();
  if (shipX < 0)           shipX = 0;
  if (shipX > WORLD_WIDTH)  shipX = WORLD_WIDTH;
  if (shipY < 0)           shipY = 0;
  if (shipY > WORLD_HEIGHT) shipY = WORLD_HEIGHT;
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("Clamp ship position took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));
#endif

  // Camera
  stepStart = micros();
  cameraX = shipX - (SCREEN_WIDTH / 2);
  cameraY = shipY - (SCREEN_HEIGHT / 2);
  if (cameraX < 0) cameraX = 0;
  if (cameraX > WORLD_WIDTH  - SCREEN_WIDTH)  cameraX = WORLD_WIDTH  - SCREEN_WIDTH;
  if (cameraY < 0) cameraY = 0;
  if (cameraY > WORLD_HEIGHT - SCREEN_HEIGHT) cameraY = WORLD_HEIGHT - SCREEN_HEIGHT;
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("Camera update took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));
#endif

  // spawnBullet() if B pressed
  if (arduboy.justPressed(B_BUTTON)) {
    stepStart = micros();
    spawnBullet(shipX, shipY, shipAngle);
    stepDuration = micros() - stepStart;
#ifdef DEBUG
    Serial.print(F("spawnBullet() took: "));
    Serial.print(stepDuration);
    Serial.println(F("us"));
#endif
  }

  // updateBullets()
  stepStart = micros();
  updateBullets();
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("updateBullets() took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));
#endif

  // tractorBeam() if DOWN pressed
  if (arduboy.pressed(DOWN_BUTTON)) {
    stepStart = micros();
    tractorBeam();
    stepDuration = micros() - stepStart;
#ifdef DEBUG
    Serial.print(F("tractorBeam() took: "));
    Serial.print(stepDuration);
    Serial.println(F("us"));
#endif
  }

  // updateTurrets()
  stepStart = micros();
  updateTurrets();
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("updateTurrets() took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));
#endif

  // updateTurretBullets()
  stepStart = micros();
  updateTurretBullets();
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("updateTurretBullets() took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));
#endif

  // --- Draw Everything ---

  // drawStars()
//   stepStart = micros();
//   drawStars();
//   stepDuration = micros() - stepStart;
// #ifdef DEBUG
//   Serial.print(F("drawStars() took: "));
//   Serial.print(stepDuration);
//   Serial.println(F("us"));
// #endif

  // drawAllTurrets()
  stepStart = micros();
  drawAllTurrets();
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("drawAllTurrets() took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));
#endif

  // drawTurretBullets()
  stepStart = micros();
  drawTurretBullets();
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("drawTurretBullets() took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));
#endif

  // drawPlanet()
  stepStart = micros();
  drawPlanet(true, false, false, false);
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("drawPlanet() took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));
#endif

  // drawAllFuelPickups()
  stepStart = micros();
  drawAllFuelPickups();
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("drawAllFuelPickups() took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));
#endif

  // drawPlayerBullets()
  stepStart = micros();
  drawPlayerBullets();
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("drawPlayerBullets() took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));
#endif

  // drawShip()
  float screenShipX = shipX - cameraX;
  float screenShipY = shipY - cameraY;
  stepStart = micros();
  drawShip(true, true, screenShipX, screenShipY, shipAngle);
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("drawShip() took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));
#endif

  // pointInPolygon()
  stepStart = micros();
  bool planetHit = pointInPolygon(CIRCLE_NUM_POINTS, circle_points, shipX, shipY);
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("pointInPolygon() took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));
#endif

  if (planetHit) {
    arduboy.print(F("HIT PLANET!"));
    stepStart = micros();
    death();
    stepDuration = micros() - stepStart;
#ifdef DEBUG
    Serial.print(F("death() took: "));
    Serial.print(stepDuration);
    Serial.println(F("us"));
#endif
  }

  // HUD
  drawLives();
  arduboy.setCursorX(36);
  if (currentFuel > 0) {
    arduboy.print(currentFuel);
  } else {
    arduboy.print(F("EMPTY"));
  }
  arduboy.setCursorX(88);
  arduboy.println(score);
  arduboy.setCursorX(0);

  // arduboy.display()
  stepStart = micros();
  arduboy.display();
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("arduboy.display() took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));
#endif

  // checkPlanetComplete()
  stepStart = micros();
  checkPlanetComplete();
  stepDuration = micros() - stepStart;
#ifdef DEBUG
  Serial.print(F("checkPlanetComplete() took: "));
  Serial.print(stepDuration);
  Serial.println(F("us"));

  // Print total frame time
  Serial.print(F("TOTAL FRAME took: "));
  Serial.print(micros() - frameStart);
  Serial.println(F("us"));
  Serial.println(F("-----"));
#endif
}

