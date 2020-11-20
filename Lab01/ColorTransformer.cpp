#include "ColorTransformer.h"

int ColorTransformer::ChangeBrighness(const Mat& sourceImage, Mat& destinationImage, short b)
{
	if (sourceImage.data == NULL)
		return 1;
	int width = sourceImage.cols, height = sourceImage.rows;
	int sourceChannels = sourceImage.channels();
	destinationImage = Mat(Size(width, height), CV_8UC3);
	int destinationChannels = destinationImage.channels();
	// Convert short number range to 1->2 number range
	if (b < -10) b = -10; // Tối quá bị flip màu
	float value = (b / 10.0f) + 1.0f;
	for (int y = 0; y < height; y++)
	{
		uchar* pSourceRow = (uchar*)(sourceImage.ptr<uchar>(y));
		uchar* pDestinationRow = (uchar*)(destinationImage.ptr<uchar>(y));
		for (int x = 0; x < width; x++, pSourceRow += sourceChannels, pDestinationRow += destinationChannels)
		{
			float B = pSourceRow[0];
			float G = pSourceRow[1];
			float R = pSourceRow[2];
			uchar B_brightness = (uchar)(fminf(255, B * value));
			uchar G_brightness = (uchar)(fminf(255, G * value));
			uchar R_brightness = (uchar)(fminf(255, R * value));
			pDestinationRow[0] = B_brightness;
			pDestinationRow[1] = G_brightness;
			pDestinationRow[2] = R_brightness;
		}
	}
    return 0;
}

int ColorTransformer::ChangeContrast(const Mat& sourceImage, Mat& destinationImage, float c)
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
			float B = pSourceRow[0] / 255.0f;
			float G = pSourceRow[1] / 255.0f;
			float R = pSourceRow[2] / 255.0f;
			float B_contrast = (((B - 0.5f) * c) + 0.5f) * 255.0f;
			float G_contrast = (((G - 0.5f) * c) + 0.5f) * 255.0f;
			float R_contrast = (((R - 0.5f) * c) + 0.5f) * 255.0f;
			if (R_contrast > 255) R_contrast = 255;
			if (R_contrast < 0) R_contrast = 0;
			if (G_contrast > 255) G_contrast = 255;
			if (G_contrast < 0) G_contrast = 0;
			if (B_contrast > 255) B_contrast = 255;
			if (B_contrast < 0) B_contrast = 0;
			pDestinationRow[0] = (uchar)B_contrast;
			pDestinationRow[1] = (uchar)G_contrast;
			pDestinationRow[2] = (uchar)R_contrast;
		}
	}
	return 0;
}

int ColorTransformer::CalcHistogram(const Mat& sourceImage, Mat& histMatrix)
{
	if (sourceImage.data == NULL)
		return 1;
	int width = sourceImage.cols, height = sourceImage.rows;
	int sourceChannels = sourceImage.channels();
	return 0;
}

ColorTransformer::ColorTransformer()
{
}

ColorTransformer::~ColorTransformer()
{
}
