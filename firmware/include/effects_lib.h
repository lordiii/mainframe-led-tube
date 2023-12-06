// Color Constants
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF

int applyBrightness(int color);
int fadePixelByAmount(int color, float amount);
void fillLEDs(int color);
int calculatePixelId(int ring, int pixel);
void setPixelColor( int ring,  int pixel, int color);
void fadePixelToBlack( int ring,  int pixel);
void fadeRingToBlack( int ring, float strength);
void fadeAllToBlack();
void setRingColor( int ring, int color);
int getPixelColor(int ring, int pixel);