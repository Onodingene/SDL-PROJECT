SDL
#include "test.h"
#include <SDL.h>
#include <stdio.h>
#include <SDL_image.h>
#include <string>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// The window we'll be rendering to
SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

SDL_Texture* loadTexture(const std::string& path) {
    // The final texture
    SDL_Texture* newTexture = NULL;

    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else {
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL) {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }
        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}

// Starts up SDL and creates window
bool initSDL() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create window
    gWindow = SDL_CreateWindow("2D Rendering ", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create renderer for window
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Initialize renderer color
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return false;
    }

    return true;
}

// Frees media and shuts down SDL
void closeSDL() {
    // Destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* args[]) {
    // Start up SDL and create window
    if (!initSDL()) {
        printf("Failed to initialize!\n");
        return -1;
    }

    // Load images
    SDL_Texture* a = loadTexture("./Assets/background1.png");
    SDL_Texture* b = loadTexture("./Assets/layer1.png");
    SDL_Texture* c = loadTexture("./Assets/layer2.png");

    if (a == NULL || b == NULL || c == NULL) {
        printf("Failed to load images!\n");
        return -1;
    }

    // Main loop flag
    bool quit = false;

    // Event handler
    SDL_Event e;

    // While application is running
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Clear screen
        SDL_RenderClear(gRenderer);

        // Render background image
        SDL_Rect backgroundQuad = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_RenderCopy(gRenderer, a, NULL, &backgroundQuad);

        // Render the images side by side with frames around them
        int imageWidth = 300; // Increased image size
        int imageHeight = 225;
        int frameThickness = 15; // Increased frame thickness for more realistic frames
        int outerBorderThickness = 5; // Outer border thickness for dark blue outline

        // Calculate positions
        SDL_Rect renderQuad1 = { (SCREEN_WIDTH / 4) - (imageWidth / 2), (SCREEN_HEIGHT / 2) - (imageHeight / 2), imageWidth, imageHeight };
        SDL_Rect renderQuad2 = { (3 * SCREEN_WIDTH / 4) - (imageWidth / 2), (SCREEN_HEIGHT / 2) - (imageHeight / 2), imageWidth, imageHeight };

        // Draw outer dark blue outline
        SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x8B, 0xFF); // Dark blue color for the outline

        SDL_Rect outerFrame1 = { renderQuad1.x - frameThickness - outerBorderThickness, renderQuad1.y - frameThickness - outerBorderThickness, renderQuad1.w + 2 * (frameThickness + outerBorderThickness), renderQuad1.h + 2 * (frameThickness + outerBorderThickness) };
        SDL_Rect outerFrame2 = { renderQuad2.x - frameThickness - outerBorderThickness, renderQuad2.y - frameThickness - outerBorderThickness, renderQuad2.w + 2 * (frameThickness + outerBorderThickness), renderQuad2.h + 2 * (frameThickness + outerBorderThickness) };

        SDL_RenderFillRect(gRenderer, &outerFrame1);
        SDL_RenderFillRect(gRenderer, &outerFrame2);

        // Draw frames
        SDL_SetRenderDrawColor(gRenderer, 0x8B, 0x45, 0x13, 0xFF); // Brown color for a wooden frame effect

        SDL_Rect frame1_outer = { renderQuad1.x - frameThickness, renderQuad1.y - frameThickness, renderQuad1.w + 2 * frameThickness, renderQuad1.h + 2 * frameThickness };
        SDL_Rect frame2_outer = { renderQuad2.x - frameThickness, renderQuad2.y - frameThickness, renderQuad2.w + 2 * frameThickness, renderQuad2.h + 2 * frameThickness };

        SDL_RenderFillRect(gRenderer, &frame1_outer);
        SDL_RenderFillRect(gRenderer, &frame2_outer);

        // Optional inner border for a more detailed frame
        int innerBorderThickness = 5;
        SDL_SetRenderDrawColor(gRenderer, 0xCD, 0x85, 0x3F, 0xFF); // Lighter brown for inner border

        SDL_Rect frame1_inner = { renderQuad1.x - innerBorderThickness, renderQuad1.y - innerBorderThickness, renderQuad1.w + 2 * innerBorderThickness, renderQuad1.h + 2 * innerBorderThickness };
        SDL_Rect frame2_inner = { renderQuad2.x - innerBorderThickness, renderQuad2.y - innerBorderThickness, renderQuad2.w + 2 * innerBorderThickness, renderQuad2.h + 2 * innerBorderThickness };

        SDL_RenderFillRect(gRenderer, &frame1_inner);
        SDL_RenderFillRect(gRenderer, &frame2_inner);

        // Draw images
        SDL_RenderCopy(gRenderer, b, NULL, &renderQuad1);
        SDL_RenderCopy(gRenderer, c, NULL, &renderQuad2);

        // Update screen
        SDL_RenderPresent(gRenderer);
    }

    // Free loaded images
    SDL_DestroyTexture(a);
    SDL_DestroyTexture(b);
    SDL_DestroyTexture(c);
    a = NULL;
    b = NULL;
    c = NULL;

    // Free resources and close SDL
    closeSDL();

    return 0;
}