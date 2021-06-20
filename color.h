#pragma once

#include <cstdio>

class Color{
public:
    double r, g, b;
    
    Color() : Color(0.0, 0.0, 0.0) {}
    Color(double R, double G, double B) : r(R), g(G), b(B) {}
    static Color black() { return Color(0.0, 0.0, 0.0); }
    static Color white() { return Color(1.0, 1.0, 1.0); }
    static Color red() { return Color(1.0, 0.0, 0.0); }
    static Color yellow() { return Color(1.0, 1.0, 0.0); }
    static Color green() { return Color(0.0, 1.0, 0.0); }
    static Color cyan() { return Color(0.0, 1.0, 1.0); }
    static Color blue() { return Color(0.0, 0.0, 1.0); }
    static Color pink() { return Color(1.0, 0.0, 1.0); }

    friend Color operator*(const Color&, double);
    friend Color operator*(double, const Color&);
    friend Color operator/(const Color&, double);
    friend Color operator+(const Color&, const Color&);
    friend Color operator*(const Color&, const Color&);
    friend Color sq_sum(const Color&, const Color&);
    friend Color raw_sum(const Color&, const Color&);

    Color confine() const;
    double brightness() const;

    void print() const {
        printf("(%f, %f, %f)", r, g, b);
    }
};