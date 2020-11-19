#include "Converter.h"

int Converter::RGB2GrayScale(const Mat& sourceImage, Mat& destinationImage)
{
    if (sourceImage.data == NULL)
        return 1;
    int width = sourceImage.cols, height = sourceImage.rows;
    int sourceChannels = sourceImage.channels();
    destinationImage = Mat(Size(width, height), CV_8UC1);
    int destinationChannels = destinationImage.channels();

    for (int y = 0; y < height; y++)
    {
        uchar* pSourceRow = (uchar*)(sourceImage.ptr<uchar>(y));
        uchar* pDestinationRow = (uchar*)(destinationImage.ptr<uchar>(y));
        for (int x = 0; x < width; x++, pSourceRow += sourceChannels, pDestinationRow += destinationChannels)
        {
            uchar B = pSourceRow[0];
            uchar G = pSourceRow[1];
            uchar R = pSourceRow[2];
            uchar gray = (uchar)((0.299 * R) + (0.587 * G) + (0.114 * B));
            //uchar gray = (uchar)((R + G + B) / 3);
            pDestinationRow[0] = gray;
        }
    }
    
    return 0;
}

int Converter::GrayScale2RGB(const Mat& sourceImage, Mat& destinationImage)
{
    if (sourceImage.data == NULL)
        return 1;
    int width = sourceImage.cols, height = sourceImage.rows;
    int sourceChannels = sourceImage.channels();
    destinationImage = Mat(Size(width, height), CV_8UC3);
    int destinationChannels = destinationImage.channels();
    for (int y = 0; y < height; y++)
    {
        uchar* pSourceRow = (uchar*)(sourceImage.ptr<uchar>(y));
        uchar* pDestinationRow = (uchar*)(destinationImage.ptr<uchar>(y));
        for (int x = 0; x < width; x++, pSourceRow += sourceChannels, pDestinationRow += destinationChannels)
        {
            uchar gray = pSourceRow[0];
            pDestinationRow[0] = gray;
            pDestinationRow[1] = gray;
            pDestinationRow[2] = gray;
        }
    }
    return 0;
}

int Converter::RGB2HSV(const Mat& sourceImage, Mat& destinationImage)
{
    if (sourceImage.data == NULL)
        return 1;
    int width = sourceImage.cols, height = sourceImage.rows;
    int sourceChannels = sourceImage.channels();
    destinationImage = Mat(Size(width, height), CV_8UC3);
    int destinationChannels = destinationImage.channels();
    for (int y = 0; y < height; y++)
    {
        uchar* pSourceRow = (uchar*)(sourceImage.ptr<uchar>(y));
        uchar* pDestinationRow = (uchar*)(destinationImage.ptr<uchar>(y));
        for (int x = 0; x < width; x++, pSourceRow += sourceChannels, pDestinationRow += destinationChannels)
        {
            // Formula: https://docs.opencv.org/3.4/de/d25/imgproc_color_conversions.html
            float B = pSourceRow[0] / 255.0f;
            float G = pSourceRow[1] / 255.0f;
            float R = pSourceRow[2] / 255.0f;
            float V = max_f(B, G, R);
            float S = 0;
            float H = 0;

            if (V == 0.0f)
            {
                S = 0;
                H = 0;
            }
            else if (V - min_f(B, G, R) == 0.0f)
            {
                S = 0;
                H = 0;
            }
            else
            {
                S = (V - min_f(B, G, R)) / V;
                if (V == B)
                    H = ((60 * (R - G)) / (V - min_f(B, G, R))) + 240;
                if (V == G)
                    H = ((60 * (B - R)) / (V - min_f(B, G, R))) + 120;
                if (V == R)
                    H = (60 * (G - B)) / (V - min_f(B, G, R));
            }
            if (H < 0) H += 360.0f;
            pDestinationRow[0] = (uchar)(V*255); // 0-255
            pDestinationRow[1] = (uchar)(S*255); // 0-255
            pDestinationRow[2] = (uchar)(H/2); // 0-180
        }
    }
    return 0;
}

int Converter::HSV2RGB(const Mat& sourceImage, Mat& destinationImage)
{
    if (sourceImage.data == NULL)
        return 1;
    int width = sourceImage.cols, height = sourceImage.rows;
    int sourceChannels = sourceImage.channels();
    destinationImage = Mat(Size(width, height), CV_8UC3);
    int destinationChannels = destinationImage.channels();
    for (int y = 0; y < height; y++)
    {
        uchar* pSourceRow = (uchar*)(sourceImage.ptr<uchar>(y));
        uchar* pDestinationRow = (uchar*)(destinationImage.ptr<uchar>(y));
        for (int x = 0; x < width; x++, pSourceRow += sourceChannels, pDestinationRow += destinationChannels)
        {
            // Formula: https://en.wikipedia.org/wiki/HSL_and_HSV
            float H = pSourceRow[2] * 2.0f; // 0-360
            float S = pSourceRow[1] / 255.0f; // 0.0-1.0
            float V = pSourceRow[0] / 255.0f; // 0.0-1.0
            float R, G, B;
            float C = V * S;
            float X = C * (1 - abs(fmod(H / 60.0f, 2) - 1)); // X = C * (1 - |H' mod 2 - 1|), H' = H/60;
            float m = V - C;
            if (H >= 0 && H < 60) {
                R = C, G = X, B = 0;
            }
            else if (H >= 60 && H < 120) {
                R = X, G = C, B = 0;
            }
            else if (H >= 120 && H < 180) {
                R = 0, G = C, B = X;
            }
            else if (H >= 180 && H < 240) {
                R = 0, G = X, B = C;
            }
            else if (H >= 240 && H < 300) {
                R = X, G = 0, B = C;
            }
            else {
                R = C, G = 0, B = X;
            }
            // R,G,B : 0.0-1.0
            pDestinationRow[0] = (uchar)((B+m) * 255); // 0-255
            pDestinationRow[1] = (uchar)((G+m) * 255); // 0-255
            pDestinationRow[2] = (uchar)((R+m) * 255); // 0-255
            /* Another approach
            int hi = (int)(H / 60.0f) % 6;
            float f = (H / 60.0f) - hi;
            float p = V * (1.0f - S);
            float q = V * (1.0f - S * f);
            float t = V * (1.0f - S * (1.0f - f));

            switch (hi) {
            case 0: R = V, G = t, B = p; break;
            case 1: R = q, G = V, B = p; break;
            case 2: R = p, G = V, B = t; break;
            case 3: R = p, G = q, B = V; break;
            case 4: R = t, G = p, B = V; break;
            case 5: R = V, G = p, B = q; break;
            }
            pDestinationRow[0] = (uchar)(B * 255);
            pDestinationRow[1] = (uchar)(G * 255);
            pDestinationRow[2] = (uchar)(R * 255);
            */
        }
    }
    return 0;
}

Converter::Converter()
{
}

Converter::~Converter()
{
}
