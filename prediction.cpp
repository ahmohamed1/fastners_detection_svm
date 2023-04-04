/**
 * @file prediction.cpp
 *
 * @brief using svm to track the fasteners
 *
 *
 * @author Abdulla Mohamed
 * Contact: abdll1@hotmail.com
 *
 */
#include<iostream>
#include <fstream>
#include "opencv2/opencv.hpp"
#include "include/extract_features.h"
#include "include/Centroid_Tracker.h"

using namespace std;
using namespace cv;


const string fastners_name [4] = {"washers", "nuts", "bolts", "screws"};
string get_count(int count, int id)
{
    std::stringstream ssd;
    ssd << fastners_name[id] << ":";
    ssd << count;
    return ssd.str();
}


int main(){
    cv::Point2d point_1(320,0);
    cv::Point2d point_2(320,400);

    cv::Mat background = cv::imread("../../dataset/background.png");
    Extract_Fastner_features eff(background);
    Centroid_Tracker centroid_tracker(point_1, point_2, 3);
    
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
        std::vector<int> classes_list;
        cv::Mat output = eff.predict_image(frame, &rectangles, &classes_list);
 
        auto new_rectangles = centroid_tracker.update(rectangles, classes_list);
        cv::line(frame,point_1, point_2, cv::Scalar(0,0,255));
        cv::rectangle(frame, cv::Point2d(0,0), cv::Point2d(200,100), cv::Scalar(255,255,255),-1);
        for (int i = 0; i < new_rectangles.size(); i++)
        {
            std::stringstream ss;
            cv::Scalar color;
            int result = new_rectangles[i].class_number;
            if(result==0){
            color= cv::Scalar(0,255,0); 
            ss << "WASHER";
            }
            else if(result==1){
            color= cv::Scalar(255,0,0); 
            ss << "NUT" ;
            }
            else if(result==2){
            color= cv::Scalar(0,0,255); 
            ss << "BOLT";
            }
            ss << ", ID:";
            ss << new_rectangles[i].id;
            cv::rectangle(frame, new_rectangles[i].rectangle, color);
            cv::putText(frame, ss.str(), new_rectangles[i].center, FONT_HERSHEY_SIMPLEX, 0.9, color);
            cv::circle(frame, new_rectangles[i].center, 3,color, -1);
        }

        for (int i = 0; i < 3; i++)
        {
            string counting = get_count(centroid_tracker.get_class_count(i), i);
            cv::putText(frame, counting, cv::Point2d(10,30+(i*30)), FONT_HERSHEY_TRIPLEX , 0.9, cv::Scalar(0,0,255));
        }
        

        cv::imshow("output", frame);
        char ikey = cv::waitKey(100);
        if(ikey == 'q')
        {
            break;
        }
    }

    return 0;
}

