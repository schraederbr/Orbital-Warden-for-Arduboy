#include <Arduboy2.h>
#include <math.h>


// Test function with random data, using fixed-point
void testFixedPointInPolygon() {
  const int numTests = 3;   // how many random polygons to generate
  const int minPoints = 3;  // min vertices
  const int maxPoints = 6;  // max vertices

  Serial.println(F("Starting testPointInPolygon with fixed-point random data"));

  for (int t = 0; t < numTests; t++) {
    // Generate a random polygon size
    int numPoints = random(minPoints, maxPoints + 1);

    // Allocate our polygon array
    FixedPoint2D *testPolygon = new FixedPoint2D[numPoints];

    // Fill the polygon with random integer coordinates, then construct fixed-point
    // FP can handle approximately ±127.0 for the integer portion
    for (int i = 0; i < numPoints; i++) {
      int16_t rx = random(0, 512); // in the range [-50..50]
      int16_t ry = random(0, 512);
      testPolygon[i].x = FP(rx);
      testPolygon[i].y = FP(ry);
    }

    Serial.print(F("Generated random polygon #"));
    Serial.println(t);
    for (int i = 0; i < numPoints; i++) {
      Serial.print(F("  P"));
      Serial.print(i);
      Serial.print(F(": ("));
      Serial.print((float)testPolygon[i].x);
      Serial.print(F(", "));
      Serial.print((float)testPolygon[i].y);
      Serial.println(F(")"));
    }

    // Generate a random test point in the same range
    FP testX = FP((int16_t)random(0, 511));
    FP testY = FP((int16_t)random(0, 511));

    Serial.print(F("Test point for polygon #"));
    Serial.print(t);
    Serial.print(F(": ("));
    Serial.print((float)testX);
    Serial.print(F(", "));
    Serial.print((float)testY);
    Serial.println(F(")"));

    // Measure time
    unsigned long startTime = micros();
    bool inside = fixedPointInPolygon(numPoints, testPolygon, testX, testY);
    unsigned long endTime = micros();

    // Print timing and results
    Serial.print(F("Random test #"));
    Serial.print(t);
    Serial.print(F(" -> Point is "));
    Serial.println(inside ? F("INSIDE") : F("OUTSIDE"));

    unsigned long totalTime = endTime - startTime;
    Serial.print(F("Execution time (microseconds): "));
    Serial.println(totalTime);
    Serial.println();

    delete[] testPolygon;
  }

  Serial.println(F("Ending testFixedPointInPolygon\n"));
}


void testPointInPolygon() {
  // We'll create multiple random polygons & random points in a loop
  const int numTests = 3; // how many polygons to generate/test
  const int maxPoints = 6; // max number of vertices per polygon
  const int minPoints = 3; // min number of vertices per polygon

  Serial.println(F("Starting testPointInPolygon with random data"));

  for (int t = 0; t < numTests; t++) {
    // Generate a random polygon size in [minPoints, maxPoints]
    int numPoints = random(minPoints, maxPoints + 1);

    // Allocate a local array for the polygon
    Point2D *testPolygon = new Point2D[numPoints];

    // Fill the polygon with random coordinates
    // Adjust range as appropriate for your project
    for (int i = 0; i < numPoints; i++) {
      testPolygon[i].x = random(0, 512);  // e.g. [-50..50]
      testPolygon[i].y = random(0, 512);  // e.g. [-50..50]
    }

    // Print the random polygon
    Serial.print(F("Generated random polygon #"));
    Serial.println(t);
    for (int i = 0; i < numPoints; i++) {
      Serial.print(F("  P"));
      Serial.print(i);
      Serial.print(F(": ("));
      Serial.print(testPolygon[i].x);
      Serial.print(F(", "));
      Serial.print(testPolygon[i].y);
      Serial.println(F(")"));
    }

    // Generate a random test point
    float testX = (float)random(0, 512);
    float testY = (float)random(0, 512);

    Serial.print(F("Test point for polygon #"));
    Serial.print(t);
    Serial.print(F(": ("));
    Serial.print(testX);
    Serial.print(F(", "));
    Serial.print(testY);
    Serial.println(F(")"));

    // Measure time
    unsigned long startTime = micros();
    bool inside = pointInPolygon(numPoints, testPolygon, testX, testY);
    unsigned long endTime = micros();

    // Print timing and results
    Serial.print(F("Random test #"));
    Serial.print(t);
    Serial.print(F(" -> Point is "));
    Serial.println(inside ? F("INSIDE") : F("OUTSIDE"));
    
    unsigned long totalTime = endTime - startTime;
    Serial.print(F("Execution time (microseconds): "));
    Serial.println(totalTime);
    Serial.println();

    // Clean up
    delete[] testPolygon;
  }

  Serial.println(F("Ending testPointInPolygon\n"));
}
void setup() {
  // Serial.begin(115200);
  arduboy.begin();
  delay(2000);
  testFixedPointInPolygon();
  while(true){
      arduboy.pollButtons();
      if(arduboy.buttonsState()){
          break;
      }
  }
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

  for (int i = 0; i < MAX_TURRET_BULLETS; i++) {
    if (turretBullets[i].active) {
      // Move bullet
      turretBullets[i].x += turretBullets[i].vx;
      turretBullets[i].y += turretBullets[i].vy;
      if (pointInPolygon(circle_num_points, circle_points, turretBullets[i].x, turretBullets[i].y)) {
        turretBullets[i].active = false;
        turretBullets[i].framesAlive = 0;
      }
      // Check if out of world, deactivate
      if (turretBullets[i].x < 0 || turretBullets[i].x > worldWidth ||
          turretBullets[i].y < 0 || turretBullets[i].y > worldHeight) {
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
        // Suppose the player's radius is ~3 or 4, or just do a point collision 
        // if your ship is small. We'll do a small threshold:
        float dx = turretBullets[i].x - shipX;
        float dy = turretBullets[i].y - shipY;
        float distSq = dx*dx + dy*dy;
        float collideRadius = 3.0f;   // tweak as needed
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
  currentFuel -= TRACTOR_FUEL_BURN_RATE / FRAME_RATE;
  if(currentFuel > 0){
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
        score += FUEL_PICKUP_SCORE;
        // Replace the removed pickup with the last one in the array
        fuelPickups[i] = fuelPickups[pickupCount - 1];
        pickupCount--;

        // Do NOT increment i here to recheck the swapped pickup
      } else {
        i++; // Only increment if no deletion occurred
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
      //If bullet hits planet, delete
      if (pointInPolygon(circle_num_points, circle_points, bullets[i].x, bullets[i].y)) {
        bullets[i].active = false;
        bullets[i].framesAlive = 0;
      }
      // Optionally deactivate if off-screen or out of world
      if (bullets[i].x < 0 || bullets[i].x > worldWidth ||
          bullets[i].y < 0 || bullets[i].y > worldHeight) {
        bullets[i].active = false;
        bullets[i].framesAlive = 0;
      }
      if(bullets[i].framesAlive > MAX_BULLET_FRAMES_ALIVE){
        bullets[i].active = false;
        bullets[i].framesAlive = 0;
      }
      for (int t = 0; t < MAX_TURRETS; t++) {
        if(pointInRectangle(bullets[i].x, bullets[i].y, &turrets[t])){
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

void checkPlanetComplete(){
  if(turretCount <= 0 && pickupCount <= 0){
    arduboy.clear();
    arduboy.setCursor(0, 0);
    arduboy.println("Planet Complete!");
    arduboy.println("Current score: " );
    arduboy.println(score);
    arduboy.display();
    delay(1000);
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

  updateBullets();
  


  if (arduboy.pressed(DOWN_BUTTON)){
    tractorBeam();
  }
  updateTurrets();
  updateTurretBullets();
  // --- 5. Draw Everything ---

  // Stars (background, parallax)
  drawStars();
  drawAllTurrets();
  drawTurretBullets();

  // Planet circle (optional regen with some other input if you like)
  drawPlanet(true, false, false, false);

  
  drawAllFuelPickups();


  drawPlayerBullets();
  // Draw the ship
  float screenShipX = shipX - cameraX;
  float screenShipY = shipY - cameraY;
  drawShip(true, true, screenShipX, screenShipY, shipAngle);
  if (pointInPolygon(circle_num_points, circle_points, shipX, shipY)) {
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
  checkPlanetComplete();
}
