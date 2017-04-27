//
// Created by Robert Jacobson on 4/22/17.
//

#include <cmath>
#include "RiemannSphere.h"
//#include "SphericalPoint.h"

Complex IdentityFunction(Complex p){
    return p;
}

Complex EquirectangularProjection(Complex p){
    //Maps the rectangle [-pi, pi]X[-pi/2, pi/2] onto the plane.

    Real c, s, cotan;
    //This computes cotan(phi/2 + M_PI_4).
    cotan = std::tan(M_PI_4f - p.imag()/2.0f);
    //This computes sin(theta) and cosine(theta) simultaneously.
//    __sincos(p.real(), &s, &c);
    s = std::sin(p.real());
    c = std::cos(p.real());
    return std::proj(Complex(c*cotan, s*cotan));
}

//Very fast approximation to atan2(y, x).
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
int fastFloori(Real x){
    return static_cast<int>(x) - (x < static_cast<int>(x));
}



//Convenience function to map a point to its angle, and then
//rescale to the interval [0, 1].
Real atan2color(Real y, Real x){
    return (fastATan2(y, x) + M_PIf)*M_1_PI_2f;
}

// Does the math to compute hsb(h, 1, v). Assumes h in [0,1]
Colorf hb2rgb(Real h, Real v) {
    h = ((h - fastFloorf(h)) * 6.0f);
    Real x = v * (1.0f - std::abs(std::fmod(h, 2.0f) - 1.0f));
    int hi = (int)h;
    switch(hi) {
        case 0:
            return Colorf({v, x, 0.0f});
        case 1:
            return Colorf({x, v, 0.0f});
        case 2:
            return Colorf({0.0f, v, x});
        case 3:
            return Colorf({0.0f, x, v});
        case 4:
            return Colorf({x, 0.0f, v});
        //case 5:
        //    return Colorf({v, 0.0, x});
    }
    //case 5:
    return Colorf({v, 0.0f, x});
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
    ComplexRectangle window = ComplexRectangle(Complex(-M_PIf, -M_PI_2f), M_2_PIf, M_PIf);
    patternImage = renderPlanePattern(window, getSize(), EquirectangularProjection);
}

Image RiemannSphere::renderPlanePattern(ComplexRectangle window, Size size, Complex (*f)(Complex)) {
    ColorImage img = Image::zeros(size, CV_8UC3);
    ColorImage outimg;
    Complex point;

    //Compute the vertical and horizontal points per pixel.
    Real vscale =  window.getHeight()/static_cast<Real>(size.height);
    Real hscale = window.getWidth()/static_cast<Real>(size.width);
    //The scale for subsampling is 1/subsample the size.
    Real vscaless = vscale / static_cast<Real>(subsample);
    Real hscaless = hscale / static_cast<Real>(subsample);
    //For convenience, we take the midpoint of subsample.
    int mid = (subsample - 1) / 2;
    Real subsamplesq = static_cast<Real>(subsample*subsample);

    Colorf color;
    Colorf colorAccumulator;

    for( int row = 0; row < img.rows; row++){
        for ( int col = 0; col < img.cols; col++){
            //We take the average color of the subsample*subsample grid of subsampled colors.
            colorAccumulator = Colorf({0.0f, 0.0f, 0.0f});
            for(int ssrow = 0; ssrow < subsample; ssrow++ ){
                for(int sscol = 0; sscol < subsample; sscol++ ){
                    //We compute the current location.
                    point = window.bottomLeft
                            + Complex(hscale*(Real)col + hscaless*(Real)(sscol - mid),
                                      vscale*(Real)row + vscaless*(Real)(ssrow - mid));
                    colorAccumulator += gridColor(f(point));
                }
            }
            color = colorAccumulator/subsamplesq;
            img(row, col)[0] = cv::saturate_cast<unsigned char>(color[0]);
            img(row, col)[1] = cv::saturate_cast<unsigned char>(color[1]);
            img(row, col)[2] = cv::saturate_cast<unsigned char>(color[2]);
        }
    }

    cv::cvtColor(img, outimg, cv::COLOR_RGB2BGR);
    return outimg;
}

Colorf RiemannSphere::gridColor(Complex point) {
    //These are RGB colors.
    Colorf Black = Colorf({0.0f,0.0f,0.0f});
    Colorf White = Colorf({255.0f,255.0f,255.0f});
    Colorf Gray = Colorf({85.0f,85.0f,85.0f});
    Colorf DarkGray = Colorf({32.0f,32.0f,32.0f});
    //These are HSB colors.
//    Colorf Black = Colorf({0.0,0.0,0.0});
//    Colorf White = Colorf({0,0,0xff});
//    Colorf Gray = Colorf({0.0,0.0,85.0});
//    Colorf DarkGray = Colorf({0.0,0.0,32.0});

    Real re = point.real();
    Real im = point.imag();
    Real re2 = re*re;
    Real im2 = im*im;
    Real dist2 = re2 + im2;

    //Grid and White/Black disks.
    bool light = !static_cast<bool>(std::abs((fastFloori(re*16.0f) + fastFloori(im*16.0f))) % 2);

    //Ring at Unity or Infinity
    enum {
        None,
        UnitCircle,
        InfinityCircle
    } ring = None;

    if (dist2 <= 1.12890625f && dist2 >= 0.87890625f) {
        ring = UnitCircle;
    } else if (dist2 <= 0.00390625f) {
        return light ? White : Black;  // ring at 0
    } else if (dist2 >= 256.0f) {
        if ((re >= 0) != (im >= 0)) return DarkGray;
        ring = InfinityCircle;  // ring at infinity
    } else if (!light) {
        return Black;
    }

    Colorf color;
    Real dist = dist2 < 1.0f ? 1.0f : std::sqrt(re2 + im2);
    Real distvalue = 255.0f / dist; // Brightness decreases as distance increases.
    switch (ring) {
        case InfinityCircle:
            // ring at infinity
            color = hb2rgb(atan2color(im, -re), distvalue);
            break;
        case UnitCircle:
            // ring at unity
        {
            Real im12 = (std::abs(im) - 1.0f) * (std::abs(im) - 1.0f);
            Real re12 = (std::abs(re) - 1.0f) * (std::abs(re) - 1.0f);
            if (im2 > re2 ? re2 + im12 > 0.00390625f : re12 + im2 > 0.00390625f) {
                //Computes a real between 0 and 8 based on angle.
                Real octang = 4.0f * M_1_PIf * fastATan2(im, re) + 4.0f;
                //The second term ranges continuously from 0 to -85.0 as octang
                //ranges from 0 to 2, 2 to 4, etc.
                Real grayvalue = 170.0f - 85.0f * (std::fmod(octang, 2.0f) / 2.0f);
                return dist == 1.0f ? Colorf({grayvalue, grayvalue, grayvalue})
                                   : Colorf({grayvalue, grayvalue, grayvalue}) + Gray;
            }
        }
            if (!light) return Black;
            // fallthrough
        default:
            color = hb2rgb(atan2color(im, -re), distvalue);
    }

    return color;
}

//Real RiemannSphere::octang(Real x, Real y) {
//    return 4.0*M_1_PI*std::atan2(y, x) + 4.0;
//}

