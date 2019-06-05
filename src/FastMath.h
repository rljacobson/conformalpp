//
// Created by Robert Jacobson on 4/29/17.
//

#ifndef CONFORMALPP_FASTMATH_H
#define CONFORMALPP_FASTMATH_H

#include <complex>

namespace FastMath {

    using Real = float;
    using Complex = std::complex<Real>;

    const float M_PIf = static_cast<float>(M_PI);
    //const float M_2_PIf = static_cast<float>(M_2_PI);
    const float M_2PIf = static_cast<float>(2.0*M_PI);
    const float M_PI_2f = static_cast<float>(M_PI_2);
    const float M_PI_4f = static_cast<float>(M_PI_4);
    const float M_3PI_4f = static_cast<float>(3.0*M_PI_4);
    const float M_1_PIf = static_cast<float>(M_1_PI);
    const float M_1_PI_2f = static_cast<float>(M_1_PI*0.5);

    Real fastATan2(Real y, Real x);
    Real fastFloorf(Real x);
    int fastFloori(Real x);
    Complex IdentityFunction(Complex p);
    Complex EquirectangularProjection(Complex p);

}


#endif //CONFORMALPP_FASTMATH_H
