#include <Arduboy2.h>
#include <math.h>

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(120);

  randomSeed(analogRead(0));
  circle_points = randomCircle(planetStepAngle, planetMinRadius, planetMaxRadius, circle_num_points);
  generateTurrets(NUM_TURRETS);
  generateStars();
  // Start the ship somewhere in the world. For demonstration, let's put it in the middle
  shipX     = worldWidth / 2;   
  shipY     = worldHeight / 2;  
  shipAngle = 0;
  velX      = 0;
  velY      = 0;
}

void loop() {
  if (!arduboy.nextFrame()) return;
  arduboy.pollButtons();
  arduboy.clear();
  // for(int i = 0; i < NUM_TURRETS; i++){
  //   arduboy.print(turrets[i].x);
  //   arduboy.print(",");
  //   arduboy.print(turrets[i].y);
  //   arduboy.print("\n");
  // }
  // arduboy.display();
  // while(true){

  // }

  // --- 1. Ship Controls ---
  // Rotate
  if (arduboy.pressed(LEFT_BUTTON)) {
    shipAngle -= ROTATION_SPEED;
  }
  if (arduboy.pressed(RIGHT_BUTTON)) {
    shipAngle += ROTATION_SPEED;
  }

  // Thrust
  if (arduboy.pressed(A_BUTTON)) {
    // angle 0 = up, so subtract PI/2
    velX += cos(shipAngle - PI / 2) * ACCELERATION;
    velY += sin(shipAngle - PI / 2) * ACCELERATION;
  }

  // Update position
  shipX += velX;
  shipY += velY;

  // Friction
  velX *= FRICTION;
  velY *= FRICTION;

  // Clamp within 512×256 world
  if (shipX < 0) shipX = 0;
  if (shipX > worldWidth)  shipX = worldWidth;
  if (shipY < 0) shipY = 0;
  if (shipY > worldHeight) shipY = worldHeight;

  // --- 2. Camera Calculation ---
  cameraX = shipX - (screenWidth  / 2);
  cameraY = shipY - (screenHeight / 2);

  // Clamp camera so it doesn’t go beyond world edges
  if (cameraX < 0) cameraX = 0;
  if (cameraX > (worldWidth  - screenWidth))  cameraX = worldWidth  - screenWidth;
  if (cameraY < 0) cameraY = 0;
  if (cameraY > (worldHeight - screenHeight)) cameraY = worldHeight - screenHeight;

  drawStars();

  // --- 3. Generate a new circle (optional) ---
  if (arduboy.justPressed(B_BUTTON)) {
    if (circle_points != nullptr) {
      delete[] circle_points;
    }
    circle_points = randomCircle(planetStepAngle, planetMinRadius, planetMaxRadius, circle_num_points);
  }

  // --- 4. Draw the Random Circle ---
  if (circle_points != nullptr) {
    // Draw each point as a small dot
    for (int i = 0; i < circle_num_points; i++) {
      int x = (int)((worldCenterX + circle_points[i].x) - cameraX);
      int y = (int)((worldCenterY + circle_points[i].y) - cameraY);
      arduboy.fillCircle(x, y, 1, WHITE);
    }
    // Draw connected lines (closing the shape)
    drawLines(circle_points, circle_num_points, true);
  }

  //My coordinates seem to be off

  if(turrets != nullptr){
    for(int i = 0; i < NUM_TURRETS; i++){
      float sX = turrets[i].x - cameraX;
      float sY = turrets[i].y - cameraY;
      drawRotatedRect(sX, sY, 1.0,1.0,1.5);
    }
  }

  // --- 5. Draw the Ship ---
  // Convert world position to screen position
  float screenShipX = shipX - cameraX;
  float screenShipY = shipY - cameraY;
  drawShip(screenShipX, screenShipY, shipAngle);
  arduboy.print(starCount);
  arduboy.display();
}