#pragma once
#include <iostream>
#include <string>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types_c.h>

using namespace cv;

#define ORIGINAL "Original image"
#define PROCESSED "Processed image"

#define min_f(a,b,c) (fminf(a,fminf(b,c)))
#define max_f(a,b,c) (fmaxf(a,fmaxf(b,c)))
// Chuyển 1 số nằm trong 1 range này vào 1 range khác
int normalize_num(int oldValue, int oldMin, int oldMax, int newMin, int newMax); 
// Chuyển histogram vào 1 range nhất định
void normalize_hist(const Mat& inputHist, Mat& outputHist, int min, int max);