#include<iostream>
#include <fstream>
#include "opencv2/opencv.hpp"
#include "include/extract_features.h"


using namespace std;
using namespace cv;




int main(){

    cv::Mat background = cv::imread("../../dataset/background.png");
    cv::Mat img = cv::imread("../../dataset/washers/_017.png");
    Extract_Fastner_features eff;
    cv::VideoCapture cap(0) ;
    cap.set(cv::CAP_PROP_AUTOFOCUS, 0); // turn the autofocus off
    if (!cap.isOpened())
    {
        cout << "[ERROR] Video is not streaming..." <<endl;
        return -1;
    }
     while(1)
    {
        // capture image 
        cv::Mat frame, rectangle_image ;
        cap >> frame;
        // Check if we capture image
        if (frame.empty())
        {
            cout<< "[ERROR] No image was captureed" <<endl;
            break;
        }
        cv::Mat croped_image = eff.crop_image(frame, cv::Point2i(128, 111), cv::Point2i(525,320));
        cv::Mat output = eff.extract_background(croped_image, background, 0);
        output = eff.connected_component_status(output);

        cv::imshow("output", output);
        cv::imshow("frame", frame);
        char ikey = cv::waitKey(1);
        if(ikey == 'q')
        {
            break;
        }
    }

    return 0;
}