//
// Created by Robert Jacobson on 4/22/17.
//

#ifndef CONFORMALPP_RECTANGLE_H
#define CONFORMALPP_RECTANGLE_H

#include "FastMath.h"

using namespace FastMath;

class ComplexRectangle {
public:
    ComplexRectangle(Complex bottomLeft, Complex topRight);
    ComplexRectangle(Complex bottomLeft, Real width, Real height);
    Complex topRight;
    Complex bottomLeft;
    Complex getCenter();
    Real getWidth();
    Real getHeight();

    static ComplexRectangle fromCenterRadius(Complex center, Real radius);
    static ComplexRectangle fromCenterWidthHeight(Complex center, Real width, Real height);
};


#endif //CONFORMALPP_RECTANGLE_H
