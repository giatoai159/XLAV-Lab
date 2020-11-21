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
		return 0;
	int width = sourceImage.cols, height = sourceImage.rows;
	int sourceChannels = sourceImage.channels();
	// Tạo 1 matrix với dữ liệu mặc định là 0
	histMatrix = Mat::zeros(Size(256, sourceChannels), CV_32S); 
	// Pointer trỏ đến từng dòng của ma trận
	int* pHistRowB = (int*)(histMatrix.ptr<int>(0));
	int* pHistRowG = NULL;
	int* pHistRowR = NULL;
	if (sourceChannels == 3) // Ảnh màu
	{
		pHistRowG = (int*)(histMatrix.ptr<int>(1));
		pHistRowR = (int*)(histMatrix.ptr<int>(2));
	}
	
	for (int y = 0; y < height; y++)
	{
		uchar* pSourceRow = (uchar*)(sourceImage.ptr<uchar>(y));
		for (int x = 0; x < width; x++, pSourceRow += sourceChannels)
		{
			if (sourceChannels == 3) // Ảnh màu
			{
				// Lấy thông số màu RGB của từng pixel
				int binIndex_B = (int)(pSourceRow[0]);
				int binIndex_G = (int)(pSourceRow[1]);
				int binIndex_R = (int)(pSourceRow[2]);
				// Lấy thông số màu nhận được từ pixel làm index cho matrix histogram
				pHistRowB[binIndex_B] += 1;
				pHistRowG[binIndex_G] += 1;
				pHistRowR[binIndex_R] += 1;
			}
			else // Ảnh xám
			{
				int binIndex = (int)(pSourceRow[0]);
				pHistRowB[binIndex]++;
			}
		}
	}
	/*
	for (int y = 0; y < histMatrix.rows; y++)
	{
		for (int x = 0; x < histMatrix.cols; x++)
		{
			std::cout << histMatrix.at<int>(y, x) << " ";
		}
		std::cout << std::endl;
	}
	*/
	return 1;
}

int ColorTransformer::HistogramEqualization(const Mat& sourceImage, Mat& destinationImage)
{
	if (sourceImage.data == NULL)
		return 0;
	int width = sourceImage.cols, height = sourceImage.rows;
	int sourceChannels = sourceImage.channels();
	Mat histMatrix;
	CalcHistogram(sourceImage, histMatrix); // Histogram của hình vào biến histMatrix
	int* pHistRowB = (int*)(histMatrix.ptr<int>(0));
	int* pHistRowG = NULL;
	int* pHistRowR = NULL;
	if (sourceChannels == 3)
	{
		pHistRowG = (int*)(histMatrix.ptr<int>(1));
		pHistRowR = (int*)(histMatrix.ptr<int>(2));
	}
	Mat tMatrix = Mat::zeros(Size(256, sourceChannels), CV_32S);
	int* pTMatRowB = (int*)(tMatrix.ptr<int>(0));
	int* pTMatRowG = NULL;
	int* pTMatRowR = NULL;
	if (sourceChannels == 3)
	{
		pTMatRowG = (int*)(tMatrix.ptr<int>(1));
		pTMatRowR = (int*)(tMatrix.ptr<int>(2));
	}
	/*
	Tính ma trận T
	T[0]=H[0]
	T[p]=T[p-1]+H[p]
	*/
	pTMatRowB[0] = pHistRowB[0];
	if (sourceChannels == 3)
	{
		pTMatRowG[0] = pHistRowG[0];
		pTMatRowR[0] = pHistRowR[0];
	}

	for (int i = 1; i < 256; i++)
	{
		pTMatRowB[i] = pTMatRowB[i - 1] + pHistRowB[i];
		if (sourceChannels == 3)
		{
			pTMatRowG[i] = pTMatRowG[i - 1] + pHistRowG[i];
			pTMatRowR[i] = pTMatRowR[i - 1] + pHistRowR[i];
		}
	}
	/*
	Chuẩn hóa T về đoạn [0,255]
	T[p]=round(((nG-1)/NM)*T[p])
	*/
	for (int i = 0; i < 256; i++)
	{
		pTMatRowB[i] = cvRound((255.0 / ((double)width * (double)height)) * (double)(pTMatRowB[i]));
		if (sourceChannels == 3)
		{
			pTMatRowG[i] = cvRound((255.0 / ((double)width * (double)height)) * (double)(pTMatRowG[i]));
			pTMatRowR[i] = cvRound((255.0 / ((double)width * (double)height)) * (double)(pTMatRowR[i]));
		}
	}
	destinationImage = Mat(Size(width, height), CV_8UC3);
	if (sourceChannels == 3)
		destinationImage = Mat(Size(width, height), CV_8UC3); // Ảnh màu
	else
		destinationImage = Mat(Size(width, height), CV_8UC1); // Ảnh xám
	int destinationChannels = destinationImage.channels();
	/*
	Tạo ảnh kết quả:
	g(x,y)=T[f(x,y)]
	*/
	for (int y = 0; y < height; y++)
	{
		uchar* pSourceRow = (uchar*)(sourceImage.ptr<uchar>(y));
		uchar* pDestinationRow = (uchar*)(destinationImage.ptr<uchar>(y));
		for (int x = 0; x < width; x++, pSourceRow += sourceChannels, pDestinationRow += destinationChannels)
		{
			if (sourceChannels == 3)
			{
				uchar B = pSourceRow[0];
				uchar G = pSourceRow[1];
				uchar R = pSourceRow[2];
				pDestinationRow[0] = (uchar)pTMatRowB[(int)B];
				pDestinationRow[1] = (uchar)pTMatRowG[(int)G];
				pDestinationRow[2] = (uchar)pTMatRowR[(int)R];
			}
			else
			{
				uchar gray = pSourceRow[0];
				pDestinationRow[0] = (uchar)pTMatRowB[(int)gray];
			}
		}
	}
	return 1;
}

ColorTransformer::ColorTransformer()
{
}

ColorTransformer::~ColorTransformer()
{
}