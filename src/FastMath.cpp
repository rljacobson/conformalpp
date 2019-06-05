//
// Created by Robert Jacobson on 4/29/17.
//

#include "FastMath.h"

namespace FastMath{

    //Fast approximation to atan2(y, x).
    Real fastATan2(Real y, Real x){
        if(y>0){
            if (x >= 0)
                return M_PI_4f - M_PI_4f * (x - y) / (x + y);
            else
                return M_3PI_4f - M_PI_4f * (x + y) / (y - x);
        }else{
            if (x >= 0)
                return -M_PI_4f + M_PI_4f * (x + y) / (x - y);
        }
        return -M_3PI_4f - M_PI_4f * (x - y) / (y + x);
    }


    //The std::floor() function is notoriously slow because it has
    //to implement the IEEE fp spec.
    Real fastFloorf(Real x){
        int xi = static_cast<int>(x) - (x < static_cast<int>(x));
        return static_cast<Real>(xi);
    }


    //The std::floor() function is notoriously slow because it has
    //to implement the IEEE fp spec.
    int fastFloori(Real x){
        return static_cast<int>(x) - (x < static_cast<int>(x));
    }



    //No-Op.
    Complex IdentityFunction(Complex p){
        return p;
    }


    /*
     * Maps the rectangle [-pi, pi]X[-pi/2, pi/2] onto the plane. The
     * point p = (theta, phi), i.e., p.real() = theta and p.imag() = phi.
     */
    Complex EquirectangularProjection(Complex p){
        Real c, s, cotan;
        //This computes cotan(phi/2 + M_PI_4) using the identity tan(x)=cot(pi/2-x).
        cotan = std::tan(M_PI_4f - p.imag()*0.5f);

        //This computes sin(theta) and cosine(theta) simultaneously.
        __sincosf(p.real(), &s, &c);
        /*
        s = std::sin(p.real());
        c = std::cos(p.real());
        */
        return std::proj(Complex(c*cotan, s*cotan));
    }


}