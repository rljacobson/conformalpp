//
// Created by Robert Jacobson on 4/22/17.
//

#ifndef CONFORMALPP_SPHERICALPOINT_H
#define CONFORMALPP_SPHERICALPOINT_H

#include <complex.h>
#include <opencv2/core/types.hpp>

using Complex = std::complex<double>;
//using Complex = cv::Complexd;

class SphericalPoint {
public:
    double theta;
    double phi;

    SphericalPoint(double theta, double phi);
    SphericalPoint(Complex c);

    Complex toComplex();
    static Complex toComplex(double theta, double phi);
};


#endif //CONFORMALPP_SPHERICALPOINT_H
