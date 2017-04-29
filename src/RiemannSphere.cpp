//
// Created by Robert Jacobson on 4/22/17.
//

#include <cmath>
#include "RiemannSphere.h"


//Convenience function to map a point to its color based on
//the argument and modulus of the point.
Colori RiemannSphere::atan2color(Real y, Real x, Real dist){
    //Computes a hue value in[0,1].
    Real h = (fastATan2(y, x) + M_PIf)*M_1_PI_2f;
    //Converts the hue into an index into the colorTable.
    int index = static_cast<int>(h*static_cast<Real>(RiemannSphere_COLORS));
    //Reciprocal of distance is proportional to brightness.
    Real invdist = 1.0f/dist;
    Colori color = colorTable[index];
    color[0] *= invdist;
    color[1] *= invdist;
    color[2] *= invdist;
    return color;
}

// Does the math to compute hsb(h, 1, v). Assumes h in [0,1].
Colori RiemannSphere::h2rgb(Real h) {
    h = ((h - fastFloorf(h)) * 6.0f);
    //Three tent functions times v.
//    int x = static_cast<int>(static_cast<Real>(v) * (1.0f - std::abs(std::fmod(h, 2.0f) - 1.0f)));
    int x = static_cast<int>(255.0f*(1.0f - std::abs(std::fmod(h, 2.0f) - 1.0f)));
    int hi = static_cast<int>(h);
    switch(hi) {
        case 0:
            return Colori({255, x, 0});
        case 1:
            return Colori({x, 255, 0});
        case 2:
            return Colori({0, 255, x});
        case 3:
            return Colori({0, x, 255});
        case 4:
            return Colori({x, 0, 255});
        default:
//        case 5:
            return Colori({255, 0, x});
    }
//    //case 5:
//    return Colori({v, 0, x});
}


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


RiemannSphere::RiemannSphere(): hasImage(false), preferredSize(RiemannSphere_DEFAULT_SIZE), subsample(RiemannSphere_SUBSAMPLE){
    precomputeColorTable();
}

RiemannSphere::RiemannSphere(Image img): image(img), hasImage(true), preferredSize(RiemannSphere_DEFAULT_SIZE), subsample(RiemannSphere_SUBSAMPLE){
    precomputeColorTable();
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
    ComplexRectangle window = ComplexRectangle(Complex(-M_PIf, -M_PI_2f), M_2PIf, M_PIf);
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
    Real mid = static_cast<Real>(subsample - 1)*0.5f;
    int subsamplesq = subsample*subsample;

    Colori color;
    Colori colorAccumulator;

    for( int row = 0; row < img.rows; row++){
        for ( int col = 0; col < img.cols; col++){
            //We take the average color of the subsample*subsample grid of subsampled colors.
            colorAccumulator = Colori({0, 0, 0});
            for(int ssrow = 0; ssrow < subsample; ssrow++ ){
                for(int sscol = 0; sscol < subsample; sscol++ ){
                    //We compute the current location.
                    point = window.bottomLeft
                            + Complex(hscale*(Real)col + hscaless*((Real)sscol - mid),
                                      vscale*(Real)row + vscaless*((Real)ssrow - mid));
                    color = gridColor(f(point));
                    //This is how we avoid a cv::saturate_cast<> call, which is expensive for some reason.
                    colorAccumulator[0] += color[0];
                    colorAccumulator[1] += color[1];
                    colorAccumulator[2] += color[2];
                }
            }
//            color = (colorAccumulator/subsamplesq);
            img(row, col)[0] = static_cast<unsigned char>(colorAccumulator[0]/subsamplesq);
            img(row, col)[1] = static_cast<unsigned char>(colorAccumulator[1]/subsamplesq);
            img(row, col)[2] = static_cast<unsigned char>(colorAccumulator[2]/subsamplesq);
        }
    }

    cv::cvtColor(img, outimg, cv::COLOR_RGB2BGR);
    return outimg;
}

Colori RiemannSphere::gridColor(Complex point) {
    //These are RGB colors.
    static const Colori Black = Colori({0,0,0});
    static const Colori White = Colori({255,255,255});
    static const Colori Gray = Colori({85,85,85});
    static const Colori DarkGray = Colori({32,32,32});

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

    Colori color;
    Real dist = dist2 < 1.0f ? 1.0f : (std::sqrt(re2 + im2));
//    int distvalue = static_cast<int>(255.0f/dist); // Brightness decreases as distance increases.
    switch (ring) {
        case InfinityCircle:
            // ring at infinity
            color = atan2color(im, re, dist);
            break;
        case UnitCircle:
            // ring at unity
        {
            Real im12 = (std::abs(im) - 1.0f) * (std::abs(im) - 1.0f);
            Real re12 = (std::abs(re) - 1.0f) * (std::abs(re) - 1.0f);
            if (im2 > re2 ? re2 + im12 > 0.00390625f : re12 + im2 > 0.00390625f) {
                //Computes a real between 0 and 8 based on angle.
                Real octang = 4.0f * M_1_PIf * fastATan2(im, -re) + 4.0f;
                //The second term ranges continuously from 0 to -85.0 as octang
                //ranges from 0 to 2, 2 to 4, etc.
                int grayvalue = static_cast<int>(170.0f - 85.0f * (std::fmod(octang, 2.0f) * 0.5f));
                return dist == 1.0 ? Colori({grayvalue, grayvalue, grayvalue})
                                   : Colori({grayvalue, grayvalue, grayvalue}) + Gray;
            }
        }
            if (!light) return Black;
            // fallthrough
        default:
            color = atan2color(im, re, dist);
    }

    return color;
}


// Precomputes color values for a predetermined number of angles.
void RiemannSphere::precomputeColorTable() {
    /*
     * The function h2rgb already assumes h is scaled to be in [0,1].
     * We split the interval [0, 1] into RiemannSphere_COLORS
     * pieces.
     */
    Real delta = 1.0f / static_cast<Real>(RiemannSphere_COLORS);
    Real h;

    for(int j = 0; j < RiemannSphere_COLORS; j++){
        h = static_cast<Real>(j)*delta;
        colorTable[j] = h2rgb(h);
    }
}

