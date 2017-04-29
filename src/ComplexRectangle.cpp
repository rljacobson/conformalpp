//
// Created by Robert Jacobson on 4/22/17.
//

#include "ComplexRectangle.h"

ComplexRectangle::ComplexRectangle(Complex bottomLeft, Complex topRight):
    topRight(topRight),
    bottomLeft(bottomLeft)
{

}

ComplexRectangle::ComplexRectangle(Complex bottomLeft, float width, float height):
    topRight(Complex(bottomLeft.real()+width, bottomLeft.imag()+height)),
    bottomLeft(bottomLeft)
{

}

Complex ComplexRectangle::getCenter() {
    return Complex((topRight.real() + bottomLeft.real())/2.0,  (topRight.imag() + bottomLeft.imag())/2.0 );
}

float ComplexRectangle::getWidth() {
    return topRight.real() - bottomLeft.real();
}

float ComplexRectangle::getHeight() {
    return topRight.imag() - bottomLeft.imag();
}

ComplexRectangle ComplexRectangle::fromCenterRadius(Complex center, float radius) {
    Complex bottomLeft = Complex(center.real() - radius, center.imag() - radius);
    Complex topRight = Complex(center.real() + radius, center.imag() + radius);

    return ComplexRectangle(bottomLeft, topRight);
}

ComplexRectangle ComplexRectangle::fromCenterWidthHeight(Complex center, float width, float height) {
    float w2 = width/2.0;
    float h2 = height/2.0;

    Complex bottomLeft = Complex(center.real() - w2, center.imag() - h2);
    Complex topRight = Complex(center.real() + w2, center.imag() + h2);

    return ComplexRectangle(bottomLeft, topRight);
}
