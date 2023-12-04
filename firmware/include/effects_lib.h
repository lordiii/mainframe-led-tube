// Color Constants
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF

int applyBrightness(int color);
int applyBrightnessByAmount(int color, float amount);
void fillLEDs(int color);
unsigned int calculatePixelId(unsigned int ring, unsigned int pixel);
void setPixelColor(unsigned int ring, unsigned int pixel, int color);
void fadePixelToBlack(unsigned int ring, unsigned int pixel, unsigned char strength);
void fadeRingToBlack(unsigned int ring, unsigned char strength);
void fadeAllToBlack(unsigned char strength);