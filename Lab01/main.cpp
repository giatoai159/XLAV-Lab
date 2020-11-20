#include "lib.h"
#include "Converter.h"
#include "ColorTransformer.h"

int main(int argc, char* argv[])
{
    std::string command = argv[1];
    std::string _inputImage = argv[2];
    //std::string inputImage = "D:\\nom.jpg";
    //std::string commandArguments = argv[3];
    if (haveImageReader(_inputImage) == false)
    {
        std::cout << "Path to " << _inputImage << " is invalid.";
        return 0;
    }
    Mat inputImage = imread(_inputImage, IMREAD_COLOR);
    Mat outputImage;
    Converter convert;
    ColorTransformer transform;
    if (command == "--rgb2gray")
    {
        if (convert.Convert(inputImage, outputImage, 0))
        {
            std::cout << "Cannot convert your image from RGB to Grayscale.";
            return 0;
        }
    }
    else if (command == "--gray2rgb")
    {
        if (convert.Convert(inputImage, outputImage, 1))
        {
            std::cout << "Cannot convert your image from Grayscale to RGB.";
            return 0;
        }
    }
    else if (command == "--rgb2hsv")
    {
        if (convert.Convert(inputImage, outputImage, 2))
        {
            std::cout << "Cannot convert your image from RGB to HSV.";
            return 0;
        }
    }
    else if (command == "--hsv2rgb")
    {
        if (convert.Convert(inputImage, outputImage, 3))
        {
            std::cout << "Cannot convert your image from HSV to RGB.";
            return 0;
        }
    }
    else if (command == "--bright")
    {
        std::string commandArguments = argv[3];
        if (transform.ChangeBrighness(inputImage, outputImage, std::stoi(commandArguments)))
        {
            std::cout << "Cannot convert your image from HSV to RGB.";
            return 0;
        }
    }
    else if (command == "--contrast")
    {
        std::string commandArguments = argv[3];
        if (transform.ChangeContrast(inputImage, outputImage, std::stof(commandArguments)))
        {
            std::cout << "Cannot convert your image from HSV to RGB.";
            return 0;
        }
    }
    else
    {
        std::cout << "This command doesn't exist.";
        return 0;
    }
    namedWindow(ORIGINAL);
    imshow(ORIGINAL, inputImage);
    namedWindow(PROCESSED);
    imshow(PROCESSED, outputImage);
    waitKey(0);
    return 0;
}