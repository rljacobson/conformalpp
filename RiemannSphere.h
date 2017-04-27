//
// Created by Robert Jacobson on 4/22/17.
//

#ifndef CONFORMALPP_RIEMANNSPHERE_H
#define CONFORMALPP_RIEMANNSPHERE_H

#include "Color.h"
//#include "SphericalPoint.h"
#include "ComplexRectangle.h"

//using ComplexRectangle = cv::Rect2d;
//using Rectangle = cv::Rect2i;
using Size = cv::Size;

using Image = cv::Mat;
using ColorImage = cv::Mat_<Color>;

#define RiemannSphere_DEFAULT_SIZE Size(1920, 960)

const float M_PIf = static_cast<float>(M_PI);
const float M_2_PIf = static_cast<float>(M_2_PI);
const float M_PI_2f = static_cast<float>(M_PI_2);
const float M_PI_4f = static_cast<float>(M_PI_4);
const float M_3PI_4f = static_cast<float>(3.0*M_PI_4);
const float M_1_PIf = static_cast<float>(M_1_PI);
const float M_1_PI_2f = static_cast<float>(M_1_PI*0.5);


class RiemannSphere {
public:
    RiemannSphere();
    RiemannSphere(Image img);

    Image getImage();
    Image getImage(Size size);
    Size getSize();
    void setImage(Image img);
    void setImage();
    void setSize(Size size);
    void setSize();
    Image getProjectedImage(ComplexRectangle window, Size extent);

    //The scale of our subsample. Must be odd.
    const unsigned char subsample = 3;

private:
    Image image;
    Image patternImage;
    bool hasImage;
    Size preferredSize; // Default = Size(1920, 960);

    void renderSpherePattern();
    Image renderPlanePattern(ComplexRectangle window, Size size, Complex (*f)(Complex));
    Colorf gridColor(Complex point);
};


#endif //CONFORMALPP_RIEMANNSPHERE_H
