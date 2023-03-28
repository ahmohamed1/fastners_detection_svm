#include<iostream>
#include <fstream>
#include "opencv2/opencv.hpp"
#include "include/extract_features.h"


using namespace std;
using namespace cv;




int main(){

    cv::Mat background = cv::imread("../../dataset/background.png");
    Extract_Fastner_features eff(background);
    eff.train();
    return 0;
}