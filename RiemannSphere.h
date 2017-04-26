//
// Created by Robert Jacobson on 4/22/17.
//

#ifndef CONFORMALPP_RIEMANNSPHERE_H
#define CONFORMALPP_RIEMANNSPHERE_H

#include <valarray>
#include <opencv2/opencv.hpp>
#include "SphericalPoint.h"
#include "ComplexRectangle.h"

//using ComplexRectangle = cv::Rect2d;
//using Rectangle = cv::Rect2i;
using Size = cv::Size;
using Color = cv::Vec3b;
//using Colord = cv::Vec3d; //color as a double.
//using Color = std::valarray<unsigned char>;
using Colord = std::valarray<double>;
using Image = cv::Mat;
using ColorImage = cv::Mat_<Color>;

#define RiemannSphere_DEFAULT_SIZE Size(1920, 960)

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
    Colord gridColor(Complex point);
    double octang(double x, double y);
};


#endif //CONFORMALPP_RIEMANNSPHERE_H
