//
// Created by Robert Jacobson on 4/22/17.
//

#include "SphericalPoint.h"

SphericalPoint::SphericalPoint(double theta, double phi) : theta(theta), phi(phi) {
}

/*
 * Computes the inverse stereographic projection of the given complex
 * number by computing
 * theta = arctan(x, y), and
 * phi = 2*arctan(sqrt(x^2+y^2))-pi/2.
 */
SphericalPoint::SphericalPoint(Complex c) {
    c = std::proj(c);

    double r = std::hypot(c.real(), c.imag());

    this->theta = std::atan2(c.real(), c.imag());
    this->phi = 2*std::atan(r) - M_PI_2;
}

/*
 * Computes the stereographic projection of the current point, returning
 * the complex number x+i*y =
 * cos(theta)*cotan(phi/2 + pi/4) + i*sin(theta)*cotan(phi/2 + pi/4).
 */
Complex SphericalPoint::toComplex(double theta, double phi) {
    double c, s, cotan;
    //This computes cotan(phi/2 + M_PI_4).
    cotan = std::tan(M_PI_4 - phi/2);
    //This computes sin(theta) and cosine(theta) simultaneously.
    __sincos(theta, &s, &c);

    return std::proj(Complex(c*cotan, s*cotan));
}


Complex SphericalPoint::toComplex() {
    return toComplex(theta, phi);
}
