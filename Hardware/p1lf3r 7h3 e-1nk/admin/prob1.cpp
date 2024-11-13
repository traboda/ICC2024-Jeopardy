//Author:br34dcrumb
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <stdlib.h>
#include "matrix.h"

#define FLAG "icc{c0uld_h4ve_1nclud3d_7h3_3PD}" // XXX: Change the flag here
extern CustomMatrix matrix;
void game_of_life(int delay_ms = 150) {
    boolean currentGrid[8][16];
    boolean nextGrid[8][16];
    boolean history[10][8][16]; // Limited to last 5 grids for cycle detection

    // Initialize the grid with random values
    randomSeed(
        analogRead(9)); // Seed from a noise source like an unconnected analog pin
    auto randomizeGrid = [](boolean grid[8][16]) {
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 16; x++) {
                grid[y][x] = random(2); // Random 0 or 1
            }
        }
    };
    randomizeGrid(currentGrid);

    // Function to count neighbors with wraparound
    auto countNeighbors = [](boolean grid[8][16], int y, int x) {
        int count = 0;
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0)
                    continue;
                int ny = (y + dy + 8) % 8;
                int nx = (x + dx + 16) % 16;
                if (grid[ny][nx])
                    count++;
            }
        }
        return count;
    };

    // Main game loop
    matrix.autoplay_off();

    while (true) {
        boolean isSame = true;

        // Calculate next generation
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 16; x++) {
                int neighbors = countNeighbors(currentGrid, y, x);
                boolean currentState = currentGrid[y][x];
                nextGrid[y][x] = currentState ? (neighbors == 2 || neighbors == 3)
                    : (neighbors == 3);
                if (nextGrid[y][x] != currentGrid[y][x])
                    isSame = false;
            }
        }

        // Detect cycle or stability by checking against the last few grids
        for (int i = 9; i > 0; i--)
            memcpy(history[i], history[i - 1], sizeof(history[i]));
        memcpy(history[0], nextGrid, sizeof(nextGrid));

        bool inHistory = false;
        for (int i = 1; i < 10; i++) {
            inHistory =
                inHistory || !memcmp(history[0], history[i], sizeof(nextGrid));
        }

        if (isSame || inHistory) {
            randomizeGrid(currentGrid); // Reset if stable or cycling
            continue;
        }

        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 16; x++) {
                if (nextGrid[y][x]) {
                    matrix.drawPixel(x, y, 60);
                } else {
                    matrix.drawPixel(x, y, 0);
                }
                currentGrid[y][x] = nextGrid[y][x];
            }
        }

        delay(delay_ms);
    }
}



void probing_spi() {
    printf("EPD_7IN5B_V2 Display\r\n");
    DEV_Module_Init();

    printf("e-Paper Init and Clear...\r\n");
    EPD_7IN5B_V2_Init();
    EPD_7IN5B_V2_Clear();
    DEV_Delay_ms(500);

    UBYTE *BlackImage, *RYImage;
    UWORD Imagesize = ((EPD_7IN5B_V2_WIDTH % 8 == 0) ? (EPD_7IN5B_V2_WIDTH / 8 ) : (EPD_7IN5B_V2_WIDTH / 8 + 1)) * EPD_7IN5B_V2_HEIGHT;
    if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        while(1);
    }
    if ((RYImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for red memory...\r\n");
        while(1);
    }
    printf("BlackImage and RedImage\r\n");
    Paint_NewImage(BlackImage, EPD_7IN5B_V2_WIDTH, EPD_7IN5B_V2_HEIGHT , 0, WHITE);
    Paint_NewImage(RYImage, EPD_7IN5B_V2_WIDTH, EPD_7IN5B_V2_HEIGHT , 0, WHITE);

    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_SelectImage(RYImage);
    Paint_Clear(WHITE);
    
    EPD_7IN5B_V2_Init();
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_DrawString_EN(18, 0, "The flag is: ", &Font24, BLACK, WHITE);

    Paint_SelectImage(RYImage); Paint_Clear(WHITE);
    Paint_DrawString_EN(10, 20, FLAG, &Font24, WHITE, BLACK);

    printf("Sending data to the Display\r\n");
    EPD_7IN5B_V2_Display(BlackImage, RYImage);
    DEV_Delay_ms(200);

    free(BlackImage);
    free(RYImage);
    BlackImage = NULL;
    RYImage = NULL;
    printf("Data Transmission Complete, RESET to transmit again - br34d\r\n");
    printf("Enjoy Game of life - br34d\r\n");
    game_of_life();
    while (1) {

        delay(100);
    }
}

extern "C" {
    uint8_t probing1() {
        probing_spi();
        return 1;
    }
}
