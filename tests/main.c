#include <stdio.h>
#include "../src/vector.h"

int main() {

	Vec3 u;
	u.x = 10;
	u.y = 10;
	u.z = 10;

	printf("%f %f %f\n", u.data[0], u.data[1], u.data[2]);
	printf("%f %f %f\n", u.x, u.y, u.z);
	
	return 0;
}
