//
// Created by Robert Jacobson on 4/22/17.
//

#ifndef CONFORMALPP_RECTANGLE_H
#define CONFORMALPP_RECTANGLE_H

#include <complex>

using Complex = std::complex<double>;

class ComplexRectangle {
public:
    ComplexRectangle(Complex bottomLeft, Complex topRight);
    ComplexRectangle(Complex bottomLeft, double width, double height);
    Complex topRight;
    Complex bottomLeft;
    Complex getCenter();
    double getWidth();
    double getHeight();

    static ComplexRectangle fromCenterRadius(Complex center, double radius);
    static ComplexRectangle fromCenterWidthHeight(Complex center, double width, double height);
};


#endif //CONFORMALPP_RECTANGLE_H
