// 128 / 8 + 1 = 17 vertices
constexpr uint8_t STEP_X      = 40;
constexpr uint8_t NUM_VERTS   = 40;
constexpr uint8_t MIN_GAP     = 20;   // play with this
constexpr uint8_t WANDER_MAX  = 30;    // max Δy per step
constexpr uint8_t SPIKE_ODDS  = 25;   // 0‑99; bigger → more spikes
constexpr uint8_t SPIKE_SIZE  = 20;    // pixels added/removed
constexpr uint8_t GAP_VERTS   = 4;   // how many vertices are flattened
constexpr uint8_t TOP_GAP_Y   = 4;   // y‑coordinate of ceiling inside the gap
uint8_t gapStartIdx;                 // first vertex inside the bay
uint8_t gapEndIdx;                   // last vertex  (inclusive)

uint8_t topY[NUM_VERTS];
uint8_t botY[NUM_VERTS];

//------------------------------------------------------------------
// Call once per level
uint8_t spawnX, spawnY;   // ship spawn point (set by the generator)

//---------------------------------------------------------------
void generateHorizontalPlanet()
{
  randomSeed(micros());

  topY[0] = 10;                // initial rough height
  botY[0] = WORLD_HEIGHT - 10;

  for (uint8_t i = 1; i < NUM_VERTS; ++i) {
    int8_t dTop = random(-WANDER_MAX,  WANDER_MAX + 1);
    int8_t dBot = random(-WANDER_MAX,  WANDER_MAX + 1);

    topY[i] = constrain(int8_t(topY[i-1]) + dTop, 0, WORLD_HEIGHT - MIN_GAP - 1);
    botY[i] = constrain(int8_t(botY[i-1]) + dBot, topY[i] + MIN_GAP, WORLD_HEIGHT);

    if (random(100) < SPIKE_ODDS) {
      uint8_t spike = SPIKE_SIZE + random(-2, 3);
      if (random(2))
        topY[i] = constrain(topY[i] + spike, 0, WORLD_HEIGHT - MIN_GAP - 1);
      else
        botY[i] = constrain(botY[i] - spike, topY[i] + MIN_GAP, WORLD_HEIGHT);
    }
  }

  // -------- carve the middle gap --------------------------------
  gapStartIdx = (NUM_VERTS - GAP_VERTS) / 2;
  gapEndIdx   = gapStartIdx + GAP_VERTS - 1;
  for (uint8_t i = 0; i < GAP_VERTS; ++i) {
  topY[gapStartIdx + i] = TOP_GAP_Y;
  if (botY[gapStartIdx + i] < TOP_GAP_Y + MIN_GAP)
      botY[gapStartIdx + i] = TOP_GAP_Y + MIN_GAP;
  }


//   startX = (gapStart + GAP_VERTS / 2) * STEP_X;  // x = centre of the bay
//   startY = TOP_GAP_Y + 8;                        // a little below the roof
}

// Need to make horizontal planet use the world coordinate system properly
void drawHorizontalPlanet()
{
  for (uint8_t i = 0; i < NUM_VERTS - 1; ++i) {

    bool  v0InGap = (i           >= gapStartIdx && i           <= gapEndIdx);
    bool  v1InGap = (i + 1       >= gapStartIdx && i + 1       <= gapEndIdx);

    int16_t x0 = i       * STEP_X;
    int16_t x1 = (i + 1) * STEP_X;

    // ---- ceiling --------------------------------------------------
    if (!(v0InGap && v1InGap)) {           // skip segments wholly inside the bay
      // Clamp any vertex that lies inside the bay to TOP_GAP_Y so we
      // draw a neat diagonal into (or out of) the gap instead of a
      // long vertical line.
      uint8_t y0 = v0InGap ? TOP_GAP_Y : topY[i];
      uint8_t y1 = v1InGap ? TOP_GAP_Y : topY[i + 1];

      arduboy.drawLine(x0, y0, x1, y1, WHITE);
    }

    // ---- floor (unchanged) ---------------------------------------
    arduboy.drawLine(x0, botY[i], x1, botY[i + 1], WHITE);
  }
}