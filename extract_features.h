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
    



    private:

};