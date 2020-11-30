#include "ColorTransformer.h"

int ColorTransformer::ChangeBrighness(const Mat& sourceImage, Mat& destinationImage, short b)
{
	if (sourceImage.data == NULL)
		return 1;
	int width = sourceImage.cols, height = sourceImage.rows;
	int sourceChannels = sourceImage.channels();
	destinationImage = Mat(Size(width, height), CV_8UC3);
	int destinationChannels = destinationImage.channels();
	// Range của b min là -10
	if (b < -10) b = -10; // Tối quá bị flip màu
	// Convert short number range to 1->2 number range
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
			// Contrast formula
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

int ColorTransformer::DrawHistogram(const Mat& histMatrix, Mat& histImage)
{
	if (histMatrix.data == NULL)
		return 0;
	int binWidth = 5; // Khoảng cách giữa các bin màu
	int histWidth = binWidth * 255, histHeight = 800, channels = histMatrix.rows;
	Mat outputHist = Mat(Size(histMatrix.cols, channels), CV_32S);
	histImage = Mat(histHeight + 20, histWidth, CV_8UC3, Scalar(255, 255, 255)); // Tạo hình output, height + 20 để dư phần trên một vài pixel
	normalize_hist(histMatrix, outputHist, 0, histHeight); // Normalize histogram input về độ cao max của hình
	// Các pointer trỏ đến các dòng của histogram
	int* poHistRowB = (int*)(outputHist.ptr<int>(0));
	int* poHistRowG = NULL;
	int* poHistRowR = NULL;
	if (channels == 3) // Ảnh màu
	{
		poHistRowG = (int*)(outputHist.ptr<int>(1));
		poHistRowR = (int*)(outputHist.ptr<int>(2));
	}
	for (int i = 1; i < histMatrix.cols; i++) // Vẽ các line histogram lên hình
	{
		/* Vẽ từ điểm (i-1, số bin màu i-1) đến (i, số bin màu i), vì OpenCV vẽ từ trên xuống nên phải trừ độ cao của hình để vẽ từ dưới lên (lật ngược hình)
		* Độ dày line: 2
		* Line_AA: line vẽ có khử răng cưa
		*/
		line(histImage, Point(binWidth * (i - 1), histHeight + 20 - poHistRowB[i - 1]),
			Point(binWidth * (i), histHeight + 20 - poHistRowB[i]), Scalar(255, 0, 0), 2, LINE_AA); // Vẽ histogram blue
		if (channels == 3) // Ảnh màu
		{
			line(histImage, Point(binWidth * (i - 1), histHeight + 20 - poHistRowG[i - 1]),
				Point(binWidth * (i), histHeight + 20 - poHistRowG[i]), Scalar(0, 255, 0), 2, LINE_AA); // Vẽ histogram green
			line(histImage, Point(binWidth * (i - 1), histHeight + 20 - poHistRowR[i - 1]),
				Point(binWidth * (i), histHeight + 20 - poHistRowR[i]), Scalar(0, 0, 255), 2, LINE_AA); // Vẽ histogram red
		}
	}
	return 1;
}

float ColorTransformer::CompareImage(const Mat& image1, Mat& image2)
{
	if (image1.data == NULL || image2.data == NULL)
		return NULL;
	int width1 = image1.cols, height1 = image1.rows, channels1 = image1.channels();
	int width2 = image2.cols, height2 = image2.rows, channels2 = image2.channels();
	if (channels1 != channels2)
	{
		std::cout << "Cannot compare a colored image with a grayscale image.";
		return NULL;
	}
	Mat _image1Hist, _image2Hist; 
	Mat image1Hist, image2Hist;
	CalcHistogram(image1, _image1Hist);
	normalize_hist(_image1Hist, image1Hist, 0, 500);
	int* p1HistRowB = (int*)(image1Hist.ptr<int>(0));
	int* p1HistRowG = NULL;
	int* p1HistRowR = NULL;
	
	if (channels1 == 3) // Ảnh màu
	{
		p1HistRowG = (int*)(image1Hist.ptr<int>(1));
		p1HistRowR = (int*)(image1Hist.ptr<int>(2));
	}
	CalcHistogram(image2, _image2Hist);
	normalize_hist(_image2Hist, image2Hist, 0, 500);
	int* p2HistRowB = (int*)(image2Hist.ptr<int>(0));
	int* p2HistRowG = NULL;
	int* p2HistRowR = NULL;
	if (channels1 == 3) // Ảnh màu
	{
		p2HistRowG = (int*)(image2Hist.ptr<int>(1));
		p2HistRowR = (int*)(image2Hist.ptr<int>(2));
	}
	float resultB = 0.0f;
	float resultG = 0.0f;
	float resultR = 0.0f;
	// Euclid Distance
	for (int i = 0; i < 256; i++)
	{
		resultB += (float)pow(p1HistRowB[i] - p2HistRowB[i], 2);
		if (channels1 == 3)
		{
			resultG += (float)pow(p1HistRowG[i] - p2HistRowG[i], 2);
			resultR += (float)pow(p1HistRowR[i] - p2HistRowR[i], 2);
		}
	}
	resultB = sqrt(resultB);
	resultG = sqrt(resultG);
	resultR = sqrt(resultR);
	return (resultB + resultG + resultR) / (3*500);
}

ColorTransformer::ColorTransformer()
{
}

ColorTransformer::~ColorTransformer()
{
}