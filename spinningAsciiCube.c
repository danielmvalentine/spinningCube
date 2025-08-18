#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// ANSI color codes
#define LIGHT_BLUE "\033[195m"
#define PINK       "\033[212m" 
#define WHITE      "\033[97m"
#define RESET      "\033[0m"

float A, B, C;
float cubeWidth = 20;
int width = 160, height = 44;
float zBuffer[160 * 44];
char buffer[160 * 44];
char colorBuffer[160 * 44]; // New buffer to store color info
int backgroundASCIICode = ' ';
int distanceFromCam = 100;
float horizontalOffset;
float K1 = 40;
float incrementSpeed = 0.6;
float x, y, z;
float ooz;
int xp, yp;
int idx;

float calculateX(int i, int j, int k) {
    return (
        j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C) +
        j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C)
    );
}

float calculateY(int i, int j, int k) {
    return (
        j * cos(A) * cos(C) + k * sin(A) * cos(C) -
        j * sin(A) * sin(B) * sin(C) + k * cos(A) * sin(B) * sin(C) -
        i * cos(B) * sin(C)
    );
}

float calculateZ(int i, int j, int k) {
    return (
        k * cos(A) * cos(B) -
        j * sin(A) * cos(B) + i * sin(B)
    );
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch, int colorCode) {
    x = calculateX(cubeX, cubeY, cubeZ);
    y = calculateY(cubeX, cubeY, cubeZ);
    z = calculateZ(cubeX, cubeY, cubeZ) + distanceFromCam;
    ooz = 1 / z;
    xp = (int)(width / 2 + horizontalOffset + K1 * ooz * x * 2);
    yp = (int)(height / 2 + K1 * ooz * y);
    idx = xp + yp * width;
    if (idx >= 0 && idx < width * height) {
        if (ooz > zBuffer[idx]) {
            zBuffer[idx] = ooz;
            buffer[idx] = ch;
            colorBuffer[idx] = colorCode;
        }
    }
}

const char* getColorCode(int colorCode) {
    switch(colorCode) {
        case 1: return LIGHT_BLUE;
        case 2: return PINK;
        case 3: return WHITE;
        default: return RESET;
    }
}

int main() {
    printf("\x1b[2J");
    while (1) {
        memset(buffer, backgroundASCIICode, width * height);
        memset(colorBuffer, 0, width * height);
        memset(zBuffer, 0, width * height * 4);
        
        // first cube
        cubeWidth = 20;
        horizontalOffset = -2 * cubeWidth;
        for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed) {
            for (float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementSpeed) {
                calculateForSurface(cubeX, cubeY, -cubeWidth, '&', 1);  // Light blue
                calculateForSurface(cubeWidth, cubeY, cubeX, '.', 2);   // Pink
                calculateForSurface(-cubeWidth, cubeY, -cubeX, '=', 3); // White
                calculateForSurface(-cubeX, cubeY, cubeWidth, ',', 1);  // Light blue
                calculateForSurface(cubeX, -cubeWidth, -cubeY, '#', 2); // Pink
                calculateForSurface(cubeX, cubeWidth, cubeY, '`', 3);   // White
            }
        }
        
        printf("\x1b[H");
        for (int k = 0; k < width * height; k++) {
            if (k % width == 0) {
                printf("\n");
            } else {
                if (buffer[k] != backgroundASCIICode) {
                    printf("%s%c%s", getColorCode(colorBuffer[k]), buffer[k], RESET);
                } else {
                    putchar(buffer[k]);
                }
            }
        }
        
        A += 0.05;
        B += 0.05;
        C += 0.01;
        usleep(8000 * 2);
    }
    return 0;
}