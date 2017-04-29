//
// Created by Robert Jacobson on 4/22/17.
//

#ifndef CONFORMALPP_RIEMANNSPHERE_H
#define CONFORMALPP_RIEMANNSPHERE_H

#include <array>
#include "Color.h"
#include "ComplexRectangle.h"
#include "FastMath.h"

using Size = cv::Size;
using Image = cv::Mat;
using ColorImage = cv::Mat_<Color>;

//Default size of the rendered image.
#define RiemannSphere_DEFAULT_SIZE Size(1920, 960)
//Number of colors in the precomputed table.
#define RiemannSphere_COLORS 360
//The default scale of our subsample.
#define RiemannSphere_SUBSAMPLE 3


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

    //The scale of our subsample.
    unsigned char subsample;

private:
    Image image;
    Image patternImage;
    bool hasImage;
    Size preferredSize; // Default = Size(1920, 960);
    std::array<Colori, RiemannSphere_COLORS> colorTable;

    void precomputeColorTable();
    Colori atan2color(Real y, Real x, Real dist);
    Colori h2rgb(Real h);
    void renderSpherePattern();
    Image renderPlanePattern(ComplexRectangle window, Size size, Complex (*f)(Complex));
    Colori gridColor(Complex point);
};


#endif //CONFORMALPP_RIEMANNSPHERE_H
