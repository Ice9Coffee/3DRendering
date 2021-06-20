#include "color.h"
#include <cmath>

Color operator+(const Color& A, const Color& B){
    return Color(A.r + B.r, A.g + B.g, A.b + B.b);
}
Color operator*(const Color& A, const Color& B) {
    return Color(A.r * B.r, A.g * B.g, A.b * B.b);
}
Color sq_sum(const Color& raw_A, const Color& raw_B) {
    Color A = raw_A.confine(),
        B = raw_B.confine();
    return Color(
        sqrt(A.r * A.r + B.r * B.r),
        sqrt(A.g * A.g + B.g * B.g),
        sqrt(A.b * A.b + B.b * B.b)
    );
}
Color raw_sum(const Color& A, const Color& B) {
    return Color(A.r + B.r, A.g + B.g, A.b + B.b);
}
Color operator*(const Color& A, double k) {
    return Color(A.r * k, A.g * k, A.b * k);
}
Color operator/(const Color& A, double k) {
    return Color(A.r / k, A.g / k, A.b / k);
}
Color operator*(double k, const Color& A) {
    return A * k;
}
Color Color::confine() const {
    Color c(r, g, b);
    if(c.r > 1.0) c.r = 1.0;
    if(c.g > 1.0) c.g = 1.0;
    if(c.b > 1.0) c.b = 1.0;
    if(c.r < 0.0) c.r = 0.0;
    if(c.g < 0.0) c.g = 0.0;
    if(c.b < 0.0) c.b = 0.0;
    return c;
}

double Color::brightness() const {
    return (r + g + b) / 3;
    //return sqrt((r*r + g*g + b*b) / 3);
}