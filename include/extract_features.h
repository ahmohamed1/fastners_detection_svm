#include<iostream>
#include <fstream>
#include "opencv2/opencv.hpp"

class Extract_Fastner_features
{
    public:
    // Process image and exctract the fastners only
    cv::Mat extract_background(cv::Mat image, cv::Mat background, int method = 1)
    {
        cv::Mat output, img32, background32;
        // step 1: remove the noise using medianBlur
        cv::medianBlur(image, output, 3);
        if(method == 1)
        {
            output.convertTo(img32, CV_32F);
            background.convertTo(background32, CV_32F);

            cv::Mat light_output = 1-(background32/img32);
            light_output = light_output*255;
            light_output.convertTo(light_output, CV_8U);
            cv::threshold(light_output, output, 75, 255, cv::THRESH_BINARY);
            cv::cvtColor( output, output, cv::COLOR_BGR2GRAY);
        }else{
            output = image - background ;
            cv::threshold(output, output, 30, 255, cv::THRESH_BINARY);
            cv::cvtColor( output, output, cv::COLOR_BGR2GRAY);
        }
        return output;
    }


    cv::Mat crop_image(cv::Mat img, cv::Point2i top_corner, cv::Point2i bottom_corner)
    {
        cv::Mat output_image = img(cv::Range(top_corner.y, bottom_corner.y), 
                                   cv::Range(top_corner.x, bottom_corner.x));
        
        return output_image;
    }
    
    cv::Mat connected_component(cv::Mat input_image)
    {
        cv::Mat labels;
        cv::Mat output_image = cv::Mat::zeros(input_image.rows, input_image.cols, CV_8UC3);
        int number_objects = cv::connectedComponents(input_image, labels);
        if (number_objects < 2)
        {
            std::cout<<"[ERROR] no object was found!!"<<std::endl;
            return output_image;
        }
        else
        {
            std::cout<<"[INFO] number of object found is "<<number_objects - 1<<std::endl;
        }
        
        cv::RNG rng(0xFFFFFFF);
        for (int i = 1; i <number_objects; i++)
        {
            cv::Mat mask = labels==i;
            cv::Scalar color = cv::Scalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));
            output_image.setTo(color, mask);
        }
        return output_image;
    }
    cv::Mat connected_component_status(cv::Mat input_image)
    {
        cv::RNG rng(0xFFFFFFF);
        cv::Mat labels, states, centroids;
        cv::Mat output_image = cv::Mat::zeros(input_image.rows, input_image.cols, CV_8UC3);
        int number_objects = cv::connectedComponentsWithStats(input_image, labels, states, centroids);
        if (number_objects < 2)
        {
            std::cout<<"[ERROR] no object was found!!"<<std::endl;
            return output_image;
        }
        else
        {
            std::cout<<"[INFO] number of object found is "<<number_objects - 1<<std::endl;
        }
        
        
        for (int i = 1; i <number_objects; i++)
        {
            cv::Mat mask = labels==i;
            cv::Scalar color = cv::Scalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));
            output_image.setTo(color, mask);
            std::cout << "Object: " << i << " pose: " << centroids.at<cv::Point2d>(i) <<
            " Area: " << states.at<int>(i, cv::CC_STAT_AREA)<<std::endl;

            // Put status
            std::stringstream ss;
            ss <<"area: "<<states.at<int>(i, cv::CC_STAT_AREA);

            cv::putText(output_image,
            ss.str(),
            centroids.at<cv::Point2d>(i),
            cv::FONT_HERSHEY_SIMPLEX,
            0.4,
            cv::Scalar(255,255,255));
        }
        return output_image;
    }

    cv::Mat find_contours(cv::Mat input_image)
    {
        cv::RNG rng(0xFFFFFFF);
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(input_image, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
        cv::Mat output_image = cv::Mat::zeros(input_image.rows, input_image.cols, CV_8UC3);
        if(contours.size() == 0)
        {
            std::cout<<"[INFO] no object detected..."<<std::endl;
            return output_image;
        }
        for(int i=0; i<contours.size(); i++)
        {
            cv::Scalar color = cv::Scalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));
            cv::drawContours(output_image, contours, i,color );
        }
        return output_image;
    }

    private:
        

};