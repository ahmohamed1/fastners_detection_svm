#include<iostream>
#include <fstream>
#include "opencv2/opencv.hpp"
#include "include/extract_features.h"
#include "include/tracking.h"

using namespace std;
using namespace cv;




int main(){

    cv::Mat background = cv::imread("../../dataset/background.png");
    Extract_Fastner_features eff(background);
    Centroid_Tracker centroid_tracker;
    
    cv::VideoCapture cap(0) ;
    cap.set(cv::CAP_PROP_AUTOFOCUS, 0); // turn the autofocus off
    if (!cap.isOpened())
    {
        cout << "[ERROR] Video is not streaming..." <<endl;
        return -1;
    }
     while(true)
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
        
        std::vector<cv::Rect> rectangles;
        cv::Mat output = eff.predict_image(frame, &rectangles);
 
        auto new_rectangles = centroid_tracker.update(rectangles);
        for (int i = 0; i < new_rectangles.size(); i++)
        {
            std::stringstream ss;
            ss << "ID:";
            ss << new_rectangles[i].id;
            cv::rectangle(frame, new_rectangles[i].rectangle, cv::Scalar(0,255,0));
            cv::putText(frame, ss.str(), new_rectangles[i].center, FONT_HERSHEY_SIMPLEX, 0.9, cv::Scalar(0,0,255));
            cv::circle(frame, new_rectangles[i].center, 3,cv::Scalar(0,0,255), -1);
        }
        
        cv::imshow("output", frame);
        cv::imshow("output detection", output);
        char ikey = cv::waitKey(100);
        if(ikey == 'q')
        {
            break;
        }
    }

    return 0;
}