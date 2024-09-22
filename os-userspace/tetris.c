#include <event.h>
#include <graphics.h>
#include <malloc.h>
#include <sage.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ABS_INT 32767

#define SCREEN_WIDTH (1280)
#define SCREEN_HEIGHT (800)
// #define WIDTH (600)
// #define HEIGHT (600)
#define WIDTH SCREEN_WIDTH
#define HEIGHT SCREEN_HEIGHT
#define BORDER_THICKNESS (10)

#define XO_WIDTH (SCREEN_WIDTH / 6)
#define XO_HEIGHT (SCREEN_HEIGHT / 6)
// #define XO_HEIGHT (100)

#define COL_LIMIT0 (WIDTH / 3)
#define COL_LIMIT1 (2 * WIDTH / 3)
#define COL_LIMIT2 (WIDTH)

#define ROW_LIMIT0 (HEIGHT / 3)
#define ROW_LIMIT1 (2 * HEIGHT / 3)
#define ROW_LIMIT2 (HEIGHT)

#define CELL_X0 (XO_WIDTH / 2) // TODO: Need to account for change in origin
#define CELL_X1 (CELL_X0 + COL_LIMIT0)
#define CELL_X2 (CELL_X0 + COL_LIMIT1)
#define CELL_Y0 (XO_HEIGHT / 2)
#define CELL_Y1 (CELL_Y0 + ROW_LIMIT0)
#define CELL_Y2 (CELL_Y0 + ROW_LIMIT1)

static int cell_x[3];
static int cell_y[3];

static bool filled[9];


void draw_board(Pixel *buf, Pixel *background_pix, Pixel *border_pix, uint32_t screen_width, uint32_t screen_height) {
  // Background
  Rectangle background_rect = {0, 0, WIDTH, HEIGHT};
  draw_rect(buf, &background_rect, background_pix, screen_width, screen_height);

  // Upper horizontal border
  Rectangle top_border_rect = {0, 0, WIDTH, BORDER_THICKNESS};
  draw_rect(buf, &top_border_rect, border_pix, screen_width, screen_height);

  // Lower horizontal border
  Rectangle bot_border_rect = {0, HEIGHT - BORDER_THICKNESS, WIDTH, BORDER_THICKNESS};
  draw_rect(buf, &bot_border_rect, border_pix, screen_width, screen_height);

  // Left border
  Rectangle left_border_rect = {0, 0, BORDER_THICKNESS, HEIGHT};
  draw_rect(buf, &left_border_rect, border_pix, screen_width, screen_height);

  // Right border
  Rectangle right_border_rect = {WIDTH - BORDER_THICKNESS, 0, BORDER_THICKNESS, HEIGHT};
  draw_rect(buf, &right_border_rect, border_pix, screen_width, screen_height);

  Rectangle vert_mid_border_rect0 = {WIDTH / 3, 0, BORDER_THICKNESS, HEIGHT};
  draw_rect(buf, &vert_mid_border_rect0, border_pix, screen_width, screen_height);

  Rectangle vert_mid_border_rect1 = {2 * WIDTH / 3, 0, BORDER_THICKNESS, HEIGHT};
  draw_rect(buf, &vert_mid_border_rect1, border_pix, screen_width, screen_height);

  Rectangle hor_mid_border_rect0 = {0, HEIGHT / 3, WIDTH, BORDER_THICKNESS};
  draw_rect(buf, &hor_mid_border_rect0, border_pix, screen_width, screen_height);

  Rectangle hor_mid_border_rect1 = {0, 2 * HEIGHT / 3, WIDTH, BORDER_THICKNESS};
  draw_rect(buf, &hor_mid_border_rect1, border_pix, screen_width, screen_height);

  // FIXME
  Rectangle lol = {0, 0, WIDTH, HEIGHT};
  screen_draw_rect(buf, &lol, 1, 1);
  screen_flush(&lol);
}


void fill_cell(Pixel *buf, Rectangle *rect) {
  Pixel fill_color = {0, 255, 0, 0};
  draw_rect(buf, rect, &fill_color, WIDTH, HEIGHT);
  Rectangle lol = {0, 0, WIDTH, HEIGHT};
  screen_draw_rect(buf, &lol, 1, 1);
  screen_flush(&lol);
}

typedef struct {
    int x, y; // position of the block
    bool active; // is the block currently falling
} Block;

Block current_block = {1, 0, true}; // starting position and state of the block

void drop_block() {
    // Logic to drop a new block from the top
    current_block.x = 1; // start from the middle column
    current_block.y = 0; // start from the top
    current_block.active = true;
    printf("Dropping new block at x: %d, y: %d\n", current_block.x, current_block.y);
}

void update_game_state() {
    // Update game state, check for landing and stack
    if (current_block.y < HEIGHT - XO_HEIGHT && !filled[current_block.x + current_block.y * 3]) {
        current_block.y++; // Move the block down
        printf("Moving block down to y: %d\n", current_block.y);
    } else {
        // Block has landed, update filled array
        filled[current_block.x + current_block.y * 3] = true;
        current_block.active = false;
        printf("Block landed at x: %d, y: %d\n", current_block.x, current_block.y);
    }
}

void move_block(int direction) {
    // Move block left or right based on direction, with boundary check
    int new_x = current_block.x + direction;
    if (new_x >= 0 && new_x < 3) { // Assuming 3 columns
        current_block.x = new_x;
    }
}

void clear_filled_lines() {
    for (int y = 0; y < 3; y++) { // Assuming 3 rows
        bool line_filled = true;
        for (int x = 0; x < 3; x++) { // Check if the line is filled
            if (!filled[x + y * 3]) {
                line_filled = false;
                break;
            }
        }

        if (line_filled) {
            // Clear the line and move down blocks above this line
            for (int dy = y; dy > 0; dy--) {
                for (int x = 0; x < 3; x++) {
                    filled[x + dy * 3] = filled[x + (dy - 1) * 3];
                }
            }
            // Clear the top line
            for (int x = 0; x < 3; x++) {
                filled[x] = false;
            }
            printf("Clearing line at y: %d\n", y);
        }
    }
}

bool is_game_over() {
    // The game is over if the top row is filled
    for (int x = 0; x < 3; x++) {
        if (filled[x]) {
            printf("Game over condition met at top row.\n");
            return true;
        }
    }
    return false;
}

int main() {

  cell_x[0] = CELL_X0;
  cell_x[1] = CELL_X1;
  cell_x[2] = CELL_X2;
  cell_y[0] = CELL_Y0;
  cell_y[1] = CELL_Y1;
  cell_y[2] = CELL_Y2;
  
  Rectangle screen_rect;
  screen_get_dims(&screen_rect);

  Pixel buf[WIDTH * HEIGHT] = {0};
  Pixel background_pix = {0, 0, 0, 0}; // Black
  Pixel border_pix = {0, 0, 255, 0}; // Blue
  printf("Printing board...\n");
  draw_board(buf, &background_pix, &border_pix, WIDTH, HEIGHT);
  printf("Printed board!\n");

  VirtioInputEvent tablet_event;
  uint32_t tablet_x = 0;
  uint32_t tablet_y = 0;
  int counter = 0;
  // Main game loop
  while (1) {
    printf("Main loop iteration\n");
    get_tablet_event(&tablet_event);
    if (tablet_event.type == EV_KEY && tablet_event.code == BTN_LEFT && tablet_event.value == 1) {
      printf("HELLO 0x%x\n", tablet_event.value);
      do {
        get_tablet_event(&tablet_event);
        move_block(-1); // Move left
        if (tablet_event.type == EV_ABS) {
          if (tablet_event.code == ABS_X)
            tablet_x = screen_rect.width * tablet_event.value / ABS_INT; // Move calc
          if (tablet_event.code == ABS_Y)
            tablet_y = screen_rect.height * tablet_event.value / ABS_INT;
        }
        printf("T: 0x%x, C: 0x%x, V: 0x%x\n", tablet_event.type, tablet_event.code, tablet_event.value);
      } while (tablet_event.type != EV_SYN);
      printf("X: %d, Y: %d\n", tablet_x, tablet_y);
    }
    if (tablet_event.type == EV_KEY && tablet_event.code == BTN_RIGHT && tablet_event.value == 1) {
      printf("HELLO 0x%x\n", tablet_event.value);
      do {
        get_tablet_event(&tablet_event);
        move_block(1); // Move left
        if (tablet_event.type == EV_ABS) {
          if (tablet_event.code == ABS_X)
            tablet_x = screen_rect.width * tablet_event.value / ABS_INT; // Move calc
          if (tablet_event.code == ABS_Y)
            tablet_y = screen_rect.height * tablet_event.value / ABS_INT;
        }
        printf("T: 0x%x, C: 0x%x, V: 0x%x\n", tablet_event.type, tablet_event.code, tablet_event.value);
      } while (tablet_event.type != EV_SYN);
      printf("X: %d, Y: %d\n", tablet_x, tablet_y);
    }
    if (counter++ % 50 == 0) { // This controls the speed of the falling block
        if (!current_block.active) {
            if (is_game_over()) {
                break; // Exit the game loop if game over
            }
            drop_block();
        }
        update_game_state();
    }

    clear_filled_lines(); // Call after updating the game state to clear any filled lines
    draw_board(buf, &background_pix, &border_pix, WIDTH, HEIGHT);
    if (current_block.active) {
        // Draw the falling block
        Rectangle block_rect = {cell_x[current_block.x], cell_y[current_block.y], XO_WIDTH, XO_HEIGHT};
        fill_cell(buf, &block_rect);
    }

    // Check for game over condition again after clearing lines
    if (is_game_over()) {
        break; // Exit the game loop
    }

  }
  return 0; 
}
