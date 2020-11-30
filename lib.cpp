#include "lib.h"

double normalize_num(double oldValue, double oldMin, double oldMax, double newMin, double newMax)
{
	if (oldMin == oldMax)
	{
		std::cout << "Old range is zero.";
		return 0;
	}
	if (newMin == newMax)
	{
		std::cout << "New range is zero.";
		return 0;
	}
	double oldRange = oldMax - oldMin, newRange = newMax - newMin;
	double newValue = (((oldValue - oldMin) * newRange) / oldRange) + (double)newMin;
	return newValue;
}

void normalize_hist(const Mat& inputHist, Mat& outputHist, int min, int max)
{
	if (inputHist.data == NULL)
		return;
	int* piHistRowB = (int*)(inputHist.ptr<int>(0));
	int* piHistRowG = NULL;
	int* piHistRowR = NULL;
	if (inputHist.rows == 3) // Ảnh màu
	{
		piHistRowG = (int*)(inputHist.ptr<int>(1));
		piHistRowR = (int*)(inputHist.ptr<int>(2));
	}
	outputHist = Mat(Size(inputHist.cols, inputHist.rows), CV_32S);
	int* poHistRowB = (int*)(outputHist.ptr<int>(0));
	int* poHistRowG = NULL;
	int* poHistRowR = NULL;
	if (inputHist.rows == 3) // Ảnh màu
	{
		poHistRowG = (int*)(outputHist.ptr<int>(1));
		poHistRowR = (int*)(outputHist.ptr<int>(2));
	}
	// Tìm max của từng màu của histogram
	int maxB = piHistRowB[0];
	int maxG = NULL, maxR = NULL;
	if (inputHist.rows == 3)
	{
		maxG = piHistRowG[0];
		maxR = piHistRowR[0];
	}
	for (int i = 1; i < 256; i++)
	{
		if (piHistRowB[i] > maxB) maxB = piHistRowB[i];
		if (inputHist.rows == 3) // Ảnh màu
		{
			if (piHistRowG[i] > maxG) maxG = piHistRowG[i];
			if (piHistRowR[i] > maxR) maxR = piHistRowR[i];
		}
	}

	// Xét từng phần tử của inputHist, normalize vào range của min, max rồi gán vào outputHist
	for (int i = 0; i < 256; i++)
	{
		poHistRowB[i] = cvRound(normalize_num(piHistRowB[i], 0, maxB, min, max));
		if (inputHist.rows == 3) // Ảnh màu
		{
			poHistRowG[i] = cvRound(normalize_num(piHistRowG[i], 0, maxG, min, max));
			poHistRowR[i] = cvRound(normalize_num(piHistRowR[i], 0, maxR, min, max));
		}
	}
}
