#include <stdio.h>
#include <math.h>

void rgb_to_xy(double red, double green, double blue) {
	double normalized[3];
	
	normalized[0] = red/255;
	normalized[1] = green/255;
	normalized[2] = blue/255;
	
	if (normalized[0] > 0.04045)
		red = (double)pow((normalized[0] + 0.055) / (1.0 + 0.055), 2.4);
	else
		red = (double)(normalized[0] / 12.92);
	
	if (normalized[1] > 0.04045)
		green = (double)pow((normalized[1] + 0.055) / (1.0 + 0.005), 2.4);
	else
		green = (double)(normalized[1] / 12.92);

	if (normalized[2] > 0.04045)
		blue = (double)pow((normalized[2] + 0.055) / (1.0 + 0.005), 2.4);
	else
		blue = (double)(normalized[2] / 12.92);

	double X = (double) (red * 0.649926 + green * 0.103455 + blue * 0.197109);
	double Y = (double) (red * 0.234327 + green * 0.743075 + blue * 0.022598);
	double Z = (double) (red * 0.000000 + green * 0.053077 + blue * 1.035763);

	double x = X / (X + Y + Z);
	double y = Y / (X + Y + Z);

	printf("%lf %lf", x, y);
}	

int main() {
	rgb_to_xy(0, 240, 240);
	return 0;
}
	
