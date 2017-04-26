//
// Created by Robert Jacobson on 4/22/17.
//

#include <cmath>
#include "RiemannSphere.h"
#include "SphericalPoint.h"

Complex IdentityFunction(Complex p){
    return p;
}

Complex EquirectangularProjection(Complex p){
    //Maps the rectangle [-pi, pi]X[-pi/2, pi/2] onto the plane.
    return SphericalPoint::toComplex(p.real(), p.imag());
}

//Convenience function to map a point to its angle, and then
//rescale to the interval [0, 255].
double atan2color(double y, double x){
    return (std::atan2(y, x) + M_PI)*M_1_PI*0.5*255.0;
}

// Does the math to compute hsb(h, 1, v)
Colord hb2rgb(double h, double v) {
    double x = v * (1.0 - std::abs(std::fmod(h , 2.0) - 1.0));

    int hi = (int)((h - std::floor(h)) * 6.0);
    switch (hi) {
        case 0:
            return Colord({v, x, 0.0});
        case 1:
            return Colord({x, v, 0.0});
        case 2:
            return Colord({0.0, v, x});
        case 3:
            return Colord({0.0, x, v});
        case 4:
            return Colord({x, 0.0, v});
        //case 5:
        //    return Colord({v, 0.0, x});
    }
    //case 5:
    return Colord({v, 0.0, x});
}

RiemannSphere::RiemannSphere(): hasImage(false), preferredSize(RiemannSphere_DEFAULT_SIZE){

}

RiemannSphere::RiemannSphere(Image img): image(img), hasImage(true), preferredSize(RiemannSphere_DEFAULT_SIZE) {
}

Image RiemannSphere::getImage() {
    if(hasImage) return image;

    if(patternImage.empty()) renderSpherePattern();
    return patternImage;
}

void RiemannSphere::setImage(Image img) {
    image = img;
    hasImage = true;
}

void RiemannSphere::setImage() {
    hasImage = false;
    image.release();
}

Image RiemannSphere::getProjectedImage(ComplexRectangle window, Size extent) {
    if(!hasImage) return renderPlanePattern(window, extent, IdentityFunction);
    return image;
}

Image RiemannSphere::getImage(Size size) {
    Image outimg;

    if(hasImage){
        if(image.cols == size.width && image.rows == size.height){
            //Nothing to do!
            outimg = image;
        }else{
            //They are asking for a different size than we have. Resize it.
            cv::resize(image, outimg, size, 0, 0, cv::INTER_CUBIC);
        }
    }else{
        if(patternImage.empty() || patternImage.cols != size.width || patternImage.rows != size.height) {
            //We need to render the pattern to the right size.
            preferredSize = size;
            renderSpherePattern();
        }
        outimg = patternImage;
    }

    return outimg;
}

Size RiemannSphere::getSize() {
    if(hasImage) return Size(image.cols, image.rows);

    return preferredSize;
}

void RiemannSphere::setSize(Size size) {
    preferredSize = size;

}

void RiemannSphere::setSize() {
    preferredSize = RiemannSphere_DEFAULT_SIZE;
}

void RiemannSphere::renderSpherePattern() {
    ComplexRectangle window = ComplexRectangle(Complex(-M_PI, -M_PI_2), M_2_PI, M_PI);
    patternImage = renderPlanePattern(window, getSize(), EquirectangularProjection);
}

Image RiemannSphere::renderPlanePattern(ComplexRectangle window, Size size, Complex (*f)(Complex)) {
    ColorImage img = Image::zeros(size, CV_8UC3);
    ColorImage outimg;
    Complex point;

    //Compute the vertical and horizontal points per pixel.
    double vscale =  window.getHeight()/static_cast<double>(size.height);
    double hscale = window.getWidth()/static_cast<double>(size.width);
    //The scale for subsampling is 1/subsample the size.
    double vscaless = vscale / static_cast<double>(subsample);
    double hscaless = hscale / static_cast<double>(subsample);
    //For convenience, we take the midpoint of subsample.
    int mid = (subsample - 1) / 2;
    double subsamplesq = static_cast<double>(subsample*subsample);

    Colord color;
    Colord colorAccumulator;

    for( int row = 0; row < img.rows; row++){
        for ( int col = 0; col < img.cols; col++){
            //We take the average color of the subsample*subsample grid of subsampled colors.
            colorAccumulator = Colord({0.0, 0.0, 0.0});
            for(int ssrow = 0; ssrow < subsample; ssrow++ ){
                for(int sscol = 0; sscol < subsample; sscol++ ){
                    //We compute the current location.
                    point = window.bottomLeft
                            + Complex(hscale*(double)col + hscaless*(double)(sscol - mid),
                                      vscale*(double)row + vscaless*(double)(ssrow - mid));
                    colorAccumulator += gridColor(f(point));
                }
            }
            color = colorAccumulator/subsamplesq;
            img(row, col)[0] = cv::saturate_cast<unsigned char>(color[0]);
            img(row, col)[1] = cv::saturate_cast<unsigned char>(color[1]);
            img(row, col)[2] = cv::saturate_cast<unsigned char>(color[2]);
        }
    }

    cv::cvtColor(img, outimg, cv::COLOR_HSV2BGR);
    return outimg;
}

Colord RiemannSphere::gridColor(Complex point) {
    //These are RGB colors.
//    Colord Black = Colord({0.0,0.0,0.0});
//    Colord White = Colord({0xff,0xff,0xff});
//    Colord Gray = Colord({85.0,85.0,85.0});
//    Colord DarkGray = Colord({32.0,32.0,32.0});
    //These are HSB colors.
    Colord Black = Colord({0.0,0.0,0.0});
    Colord White = Colord({0,0,0xff});
    Colord Gray = Colord({0.0,0.0,85.0});
    Colord DarkGray = Colord({0.0,0.0,32.0});

    double re = point.real();
    double im = point.imag();
    double re2 = re*re;
    double im2 = im*im;
    double dist2 = re2 + im2;

    //White/Black.
    bool light = !static_cast<bool>(std::abs(((int)std::floor(re*16.0) + (int)std::floor(im*16.0))) % 2);

    //Ring at Unity or Infinity
    enum {
        None,
        UnitCircle,
        InfinityCircle
    } ring = None;

    if (dist2 <= 1.12890625 && dist2 >= 0.87890625) {
        ring = UnitCircle;
    } else if (dist2 <= 0.00390625) {
        return light ? White : Black;  // ring at 0
    } else if (dist2 >= 256.0) {
        if ((re >= 0) != (im >= 0)) return DarkGray;
        ring = InfinityCircle;  // ring at infinity
    } else if (!light) {
        return White;
    }

    Colord color;
    double dist = dist2 < 1.0 ? 1.0 : std::sqrt(re2 + im2);
    double distvalue = 255.0 / dist; // Brightness decreases as distance increases.
    switch (ring) {
        case InfinityCircle:
            // ring at infinity
            color = Colord({atan2color(im, -re), 255.0, distvalue});
            break;
        case UnitCircle:
            // ring at unity
        {
            double im12 = (std::fabs(im) - 1.0) * (std::fabs(im) - 1.0);
            double re12 = (std::fabs(re) - 1.0) * (std::fabs(re) - 1.0);
            if (im2 > re2 ? re2 + im12 > 0.00390625 : re12 + im2 > 0.00390625) {
                //Computes a real between 0 and 8 based on angle.
                double octang = 4.0 * M_1_PI * std::atan2(im, re) + 4.0;
                //The second term ranges continuously from 0 to -85.0.
                double grayvalue = 170.0 - 85.0 * (std::fmod(octang, 2.0) / 2.0);
                return dist == 1.0 ? Colord({0.0, 0.0, grayvalue})
                                   : Colord({0.0, 0.0, grayvalue}) + Gray;
            }
        }
            if (!light) return Black;
            // fallthrough
        default:
            color = Colord({atan2color(im, -re), 255.0, distvalue});
    }

    return color;
}

//double RiemannSphere::octang(double x, double y) {
//    return 4.0*M_1_PI*std::atan2(y, x) + 4.0;
//}

