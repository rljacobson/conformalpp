#include <iostream>
#include <complex>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "RiemannSphere.h"

void ishow(cv::Mat img){
    cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Display window", img );
    cv::waitKey();
}

int main(int argc, char** argv)
{
    cv::Mat img;
    RiemannSphere sphere;
    //std::string inputFile("/Users/rljacobson/Downloads/out.png");
    //std::string outputFile("/Users/rljacobson/Downloads/out.jpg");

    // Load an image from file.
    //img = cv::imread(inputFile, CV_LOAD_IMAGE_UNCHANGED);

    std::cout << "Rendering projected pattern..." << std::endl;

    img = sphere.getProjectedImage(ComplexRectangle::fromCenterWidthHeight(Complex(0, 0), 10.0, 5.0), sphere.getSize());

    std::cout << "Done." << std::endl;
    ishow(img);

    std::cout << "Rendering equirectangle..." << std::endl;
    img = sphere.getImage();
    ishow(img);

    return EXIT_SUCCESS;
}