#include <iostream>
#include <complex>
//#include <stdio.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "mathpresso/mathpresso.h"
#include "RiemannSphere.h"

void ishow(cv::Mat img){
    cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Display window", img );
    cv::waitKey();
}

// FROM MATHPRESSO:
// By inheriting `OutputLog` one can create a way how to handle possible errors
// and report them to humans. The most interesting and used message type is
// `kMessageError`, because it signalizes an invalid expression. Other message
// types are used mostly for debugging.
struct MyOutputLog : public mathpresso::OutputLog {
    MyOutputLog() {}
    virtual ~MyOutputLog() {}
    virtual void log(unsigned int type, unsigned int line, unsigned int column, const char* message, size_t len) {
        switch (type) {
            case kMessageError:
                printf("[ERROR]: %s (line %u, column %u)\n", message, line, column);
                break;

            case kMessageWarning:
                printf("[WARNING]: %s (line %u, column %u)\n", message, line, column);
                break;

            case kMessageAstInitial:
                printf("[AST-INITIAL]\n%s", message);
                break;

            case kMessageAstFinal:
                printf("[AST-FINAL]\n%s", message);
                break;

            case kMessageAsm:
                printf("[ASSEMBLY]\n%s", message);
                break;

            default:
                printf("[UNKNOWN]\n%s", message);
                break;
        }
    }
};

int mainold(int argc, char** argv)
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
    std::cout << "Done." << std::endl;
    ishow(img);

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
    mathpresso::Context ctx;
    mathpresso::Expression e;

    MyOutputLog outputLog;

    Variables variables;
    variables.x = 0.0;
    variables.y = 0.0;
    variables.z = 0.0;

    ctx.addBuiltIns();
    ctx.addVariable("x", MATHPRESSO_OFFSET(Variables, x));
    ctx.addVariable("y", MATHPRESSO_OFFSET(Variables, y));
    ctx.addVariable("z", MATHPRESSO_OFFSET(Variables, z));

    fprintf(stdout, "=========================================================\n");
    fprintf(stdout, "MPEval - MathPresso's Command Line Evaluator\n");
    fprintf(stdout, "---------------------------------------------------------\n");
    fprintf(stdout, "You can use variables 'x', 'y' and 'z'. Initial values of\n");
    fprintf(stdout, "these variables are 0.0. Assignment operator '=' can be\n");
    fprintf(stdout, "used to change the initial values.\n");
    fprintf(stdout, "=========================================================\n");

    for (;;) {
        char buffer[4096];
        fgets(buffer, 4095, stdin);
        if (buffer[0] == 0) break;

        mathpresso::Error result = e.compile(ctx, buffer, mathpresso::kOptionVerbose, &outputLog);
        if (result == mathpresso::kErrorOk)
            fprintf(stdout, "%f\n", e.evaluate(&variables));
        if (result == mathpresso::kErrorNoExpression)
            break;
    }

    return EXIT_SUCCESS;
}