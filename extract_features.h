#include<iostream>
#include <fstream>
#include "opencv2/opencv.hpp"

class Extract_Fastner_features
{
    public:
    // Process image and exctract the fastners only
    cv::Mat extract_background(cv::Mat image, cv::Mat background);
    cv::Mat process_image(cv::Mat image)
    {
        cv::Mat output;

        // step 1: remove the noise using medianBlur
        cv::medianBlur(image, output, 3);

    }

    cv::Mat region_of_interest(cv::Mat img, int[2] top_corner, int[2] bottom_corner)
    {
        // Range(start_row, end_row), Range(start_col, end_col)
        cv::Mat output_image = img(cv::Range(top_corner[0], bottom_corner[0]), 
                                   cv::Range(top_corner[1], bottom_corner[1]));
        
        return output_image;
    }
    
    std::vector<std::vector<cv::Point>> extract features(cv::Mat thresh_image)
    {
        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(thresh_image, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

        return contours;
    }



    private:

};