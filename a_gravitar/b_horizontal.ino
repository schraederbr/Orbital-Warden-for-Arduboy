// 128 / 8 + 1 = 17 vertices
constexpr uint8_t STEP_X      = 8;
constexpr uint8_t NUM_VERTS   = 17;
constexpr uint8_t MIN_GAP     = 20;   // play with this
constexpr uint8_t WANDER_MAX  = 5;    // max Δy per step
constexpr uint8_t SPIKE_ODDS  = 25;   // 0‑99; bigger → more spikes
constexpr uint8_t SPIKE_SIZE  = 6;    // pixels added/removed

uint8_t topY[NUM_VERTS];
uint8_t botY[NUM_VERTS];

//------------------------------------------------------------------
// Call once per level
void generateHorizontalPlanet()
{
  randomSeed(micros());

  // start roughly 1/4 from top & bottom
  topY[0] = 10;                 // feel free to tune
  botY[0] = 63 - 10;

  for (uint8_t i = 1; i < NUM_VERTS; ++i) {

    // ── 1. Let each line “wander” up or down a little
    int8_t dTop = random(-WANDER_MAX,  WANDER_MAX + 1);
    int8_t dBot = random(-WANDER_MAX,  WANDER_MAX + 1);

    topY[i] = constrain(int8_t(topY[i-1]) + dTop, 0, 63 - MIN_GAP - 1);
    botY[i] = constrain(int8_t(botY[i-1]) + dBot, topY[i] + MIN_GAP, 63);

    // ── 2. 20‑25 % of the segments get a spike
    if (random(100) < SPIKE_ODDS) {
      uint8_t spike = SPIKE_SIZE + random(-2, 3);   // small variance
      // 50 % chance to put the spike on the upper wall,
      // otherwise on the lower wall
      if (random(2)) {
        // spike pointing down from the ceiling
        topY[i] = constrain(topY[i] + spike, 0, 63 - MIN_GAP - 1);
      } else {
        // spike pointing up from the floor
        botY[i] = constrain(botY[i] - spike, topY[i] + MIN_GAP, 63);
      }
    }
  }
}


void drawHorizontalPlanet()
{
  for (uint8_t i = 0; i < NUM_VERTS - 1; ++i) {
    int16_t x1 = i * STEP_X;
    int16_t x2 = (i + 1) * STEP_X;

    // ceiling
    arduboy.drawLine(x1, topY[i], x2, topY[i + 1], WHITE);

    // floor
    arduboy.drawLine(x1, botY[i], x2, botY[i + 1], WHITE);
  }
}