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

}