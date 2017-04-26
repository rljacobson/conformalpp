//
// Created by Robert Jacobson on 4/22/17.
//

#include "ComplexRectangle.h"

ComplexRectangle::ComplexRectangle(Complex bottomLeft, Complex topRight):
    topRight(topRight),
    bottomLeft(bottomLeft)
{

}

ComplexRectangle::ComplexRectangle(Complex bottomLeft, double width, double height):
    bottomLeft(bottomLeft),
    topRight(Complex(bottomLeft.real()+width, bottomLeft.imag()+height))
{

}

Complex ComplexRectangle::getCenter() {
    return Complex((topRight.real() + bottomLeft.real())/2.0,  (topRight.imag() + bottomLeft.imag())/2.0 );
}

double ComplexRectangle::getWidth() {
    return topRight.real() - bottomLeft.real();
}

double ComplexRectangle::getHeight() {
    return topRight.imag() - bottomLeft.imag();
}

ComplexRectangle ComplexRectangle::fromCenterRadius(Complex center, double radius) {
    Complex bottomLeft = Complex(center.real() - radius, center.imag() - radius);
    Complex topRight = Complex(center.real() + radius, center.imag() + radius);

    return ComplexRectangle(bottomLeft, topRight);
}

ComplexRectangle ComplexRectangle::fromCenterWidthHeight(Complex center, double width, double height) {
    double w2 = width/2.0;
    double h2 = height/2.0;

    Complex bottomLeft = Complex(center.real() - w2, center.imag() - h2);
    Complex topRight = Complex(center.real() + w2, center.imag() + h2);

    return ComplexRectangle(bottomLeft, topRight);
}
