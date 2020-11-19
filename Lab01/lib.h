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