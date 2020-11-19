#include "lib.h"
#include "Converter.h"
#include "ColorTransformer.h"

int main(int argc, char* argv[])
{
    //std::string command = argv[1];
    //std::string inputFile = argv[2];
    std::string inputFile = "D:\\nom.jpg";
    //std::string commandArguments = argv[3];
    Mat inputImage = imread(inputFile, IMREAD_COLOR);
    namedWindow(ORIGINAL);
    imshow(ORIGINAL, inputImage);
    Mat outputImage;
    Converter convert;

    convert.RGB2HSV(inputImage, outputImage);
    namedWindow("HSV");
    imshow("HSV", outputImage);
    Mat out2;
    convert.HSV2RGB(outputImage, out2);
    namedWindow(PROCESSED);
    imshow(PROCESSED, out2);
    waitKey(0);
    return 0;
}