#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7789  _panel_instance;
  lgfx::Bus_SPI       _bus_instance;
  lgfx::Light_PWM     _light_instance;
  lgfx::Touch_XPT2046 _touch_instance; // Touch instance

public:
  LGFX(void) {
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host = HSPI_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 40000000;
      cfg.freq_read  = 16000000;
      cfg.pin_sclk = 14;
      cfg.pin_mosi = 13;
      cfg.pin_miso = 12;
      cfg.pin_dc   = 2;
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }
    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs           = 15;
      cfg.pin_rst          = -1;
      cfg.panel_width      = 240;
      cfg.panel_height     = 320;
      cfg.offset_x         = 0;
      cfg.offset_y         = 0;
      cfg.offset_rotation  = 0;
      cfg.invert = false;
      cfg.rgb_order = false;
      _panel_instance.config(cfg);
    }
    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = 21;
      cfg.freq   = 44100;
      cfg.pwm_channel = 7;
      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }
    {
      // --- CORRECTED CYD TOUCH CONFIGURATION ---
      auto cfg = _touch_instance.config();
      cfg.x_min      = 0;    
      cfg.x_max      = 240;  
      cfg.y_min      = 0;    
      cfg.y_max      = 320;  
      cfg.pin_int    = 36;    // CYD uses pin 36 for Touch Interrupts
      
      cfg.bus_shared = false; // CRITICAL: The CYD does NOT share the screen's SPI bus
      cfg.offset_rotation = 0;

      // CYD hardwired touch pins
      cfg.spi_host = VSPI_HOST; // Must use VSPI (HSPI is already in use by the screen)
      cfg.freq = 1000000;       
      cfg.pin_sclk = 25;
      cfg.pin_mosi = 32;
      cfg.pin_miso = 39;
      cfg.pin_cs   = 33;

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }
    setPanel(&_panel_instance);
  }
};

LGFX lcd;
LGFX_Sprite lifeSprite(&lcd);

const int DISPLAY_WIDTH = 320;
const int DISPLAY_HEIGHT = 240;
const int CELL_SIZE = 4;
const int GRID_W = DISPLAY_WIDTH / CELL_SIZE;
const int GRID_H = DISPLAY_HEIGHT / CELL_SIZE;
const uint16_t COLOR_ALIVE = TFT_WHITE;
const uint16_t COLOR_DEAD  = TFT_BLACK;

uint8_t grid[GRID_H][GRID_W];
uint8_t nextGrid[GRID_H][GRID_W];

void randomizeGrid() {
  for (int y = 0; y < GRID_H; y++) {
    for (int x = 0; x < GRID_W; x++) {
      grid[y][x] = random(2);
    }
  }
}

int countNeighbors(int x, int y) {
  int count = 0;
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      if (dx == 0 && dy == 0) continue;
      int nx = x + dx;
      int ny = y + dy;
      if (nx >= 0 && nx < GRID_W && ny >= 0 && ny < GRID_H) {
        count += grid[ny][nx];
      }
    }
  }
  return count;
}

void stepGeneration() {
  for (int y = 0; y < GRID_H; y++) {
    for (int x = 0; x < GRID_W; x++) {
      int alive = grid[y][x];
      int neighbors = countNeighbors(x, y);
      nextGrid[y][x] = alive ? (neighbors == 2 || neighbors == 3) : (neighbors == 3);
    }
  }
  memcpy(grid, nextGrid, sizeof(grid));
}

void drawGrid() {
  lifeSprite.fillSprite(COLOR_DEAD);
  for (int y = 0; y < GRID_H; y++) {
    for (int x = 0; x < GRID_W; x++) {
      if (grid[y][x]) {
        lifeSprite.fillRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, COLOR_ALIVE);
      }
    }
  }
  lifeSprite.pushSprite(0, 0);
}

void setup() {
  lcd.init();
  lcd.setRotation(1);
  lcd.setBrightness(128);
  lcd.clear(TFT_BLACK);

  randomSeed(analogRead(0));
  randomizeGrid();
  
  // NEW: Drop color depth to 8-bit to halve memory usage to 76800 bytes
  lifeSprite.setColorDepth(8); 
  
  lifeSprite.createSprite(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  // lifeSprite.setSwapBytes(true); // You can delete or comment this out for 8-bit
  lifeSprite.fillSprite(COLOR_DEAD);
}

static bool wasTouched = false;

void loop() {
  int32_t touchX, touchY;
  
  // Ask LovyanGFX if the screen is currently being touched
  bool isTouched = lcd.getTouch(&touchX, &touchY);

  // If touched NOW, but wasn't touched a millisecond ago...
  if (isTouched && !wasTouched) {
    // 1. Repopulate the array with new random data
    randomizeGrid();
    
    // 2. Redraw the grid immediately so the sprite updates
    drawGrid(); 
  }
  
  // Save the current touch state for the next loop iteration
  wasTouched = isTouched;

  // ... [Keep your existing stepGeneration(), drawGrid(), and pushSprite() here] ...
  
  stepGeneration();
  drawGrid();
  lifeSprite.pushSprite(0, 0);
  delay(50); // Adjust speed of the game if necessary
}
