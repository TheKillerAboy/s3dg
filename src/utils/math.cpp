#include <cmath>

bool cmpf(float A, float B, float epsilon) {
    return (fabs(A - B) < epsilon);
}
