/**
 * @file extract_features.h
 *
 * @brief Using feature engineering the feature of fastners extracted using threshold and contours
 * then using contours to determine each feature of object.
 *
 *
 * @author Abdulla Mohamed
 * Contact: abdll1@hotmail.com
 *
 */

#include<iostream>
#include <fstream>
#include "opencv2/opencv.hpp"
#include <opencv2/ml.hpp>

using namespace cv;
using namespace cv::ml;

class Extract_Fastner_features
{
    public:

    Extract_Fastner_features(cv::Mat _background)
    {
        background = _background.clone(); 
    }
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
        //[Next, Previous, First_Child, Parent] 
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
            float with_hole = 0;
            cv::Scalar color = cv::Scalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));
            if(hierarchy[i][3] == -1 )//|| hierarchy[i][2] != -1
            {   
                if(hierarchy[i][2] != -1 )
                    with_hole = 1;
                cv::drawContours(output_image, contours, i,color );
                double area = cv::contourArea(contours[i]);
                if ( area > 250)
                {
                    
                    cv::RotatedRect r = cv::minAreaRect(contours[i]);
                    float width = r.size.width;
                    float height = r.size.height;
                    float aspect_ration = (width<height)? height/width : width/height;

                    std::vector<float> data;
                    data.push_back(area);
                    data.push_back(aspect_ration);
                    data.push_back(with_hole);
                    // std::cout<<i+1<<": "<< area <<"," << aspect_ration <<" " << with_hole <<std::endl;
                }
            }
        }
        // std::cout<<"///////////////////////"<<std::endl;
        return output_image;
    }

    std::vector<std::vector<float>> extract_feature(cv::Mat image, std::vector<cv::Point2d>* points=NULL, std::vector<cv::Rect>* rectangles=NULL)
    {
        std::vector<std::vector<float>> output_features;
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(image, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
        cv::Mat output_image = cv::Mat::zeros(image.rows, image.cols, CV_8UC3);
        std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
        if(contours.size() == 0)
        {
            std::cout<<"[INFO] no object detected..."<<std::endl;
            return output_features;
        }
        for(int i=0; i<contours.size(); i++)
        {
            float with_hole = 0;
            if(hierarchy[i][3] == -1 )//|| hierarchy[i][2] != -1
            {   
                if(hierarchy[i][2] != -1 )
                    with_hole = 1;
                float area = cv::contourArea(contours[i]);
                if ( area > 350)
                {
                    cv::drawContours(output_image, contours, i,cv::Scalar(0,0,255) );
                    cv::RotatedRect r = cv::minAreaRect(contours[i]);
                    float width = r.size.width;
                    float height = r.size.height;
                    float aspect_ration = (width<height)? height/width : width/height;

                    std::vector<float> data;
                    data.push_back(area);
                    data.push_back(aspect_ration);
                    data.push_back(with_hole);
                    output_features.push_back(data);
                    if(points!=NULL){
                        points->push_back(r.center);
                    }
                    if(rectangles!= NULL)
                    {
                        approxPolyDP( contours[i], contours_poly[i], 3, true );
                        rectangles->push_back(cv::boundingRect( contours_poly[i] ));
                    }
                }
            }
        }
        // cv::imshow("output", output_image);
        // cv::waitKey(80);
        return output_features;
    }

    bool read_and_process_data(std::string directory, int label, int number_test, 
                      std::vector<float> &train_data, std::vector<float> &test_data,
                      std::vector<int> &train_label, std::vector<float> &test_label)
    {
        cv::VideoCapture images;
        if( images.open(directory) == false)
        {
            std::cout<<"[ERROR] cannot open the folder..."<<std::endl;
            return 0;
        }
        cv::Mat image;
        int image_indx = 0;
        // cv::Mat background = cv::imread("../../dataset/background.png"); 
        while(images.read(image))
        {
            cv::Mat processed_image = extract_background(image, background, 0);
            std::vector<std::vector<float>> features = extract_feature(processed_image);
            for (int i = 0; i < features.size(); i++)
            {
                if( image_indx >= number_test)
                {
                    train_data.push_back(features[i][0]);
                    train_data.push_back(features[i][1]);
                    train_data.push_back(features[i][2]);
                    train_label.push_back(label);
                }
                else
                {
                    test_data.push_back(features[i][0]);
                    test_data.push_back(features[i][1]);
                    test_data.push_back(features[i][2]);
                    test_label.push_back((float)label);
                }
            }
            
            image_indx++;
        }
        return true;
    }

    void train()
    {
        int number_test = 3;
        std::vector<float> train_data;
        std::vector<float> test_data;
        std::vector<int>   train_label;
        std::vector<float> test_label;


        read_and_process_data("../../dataset/washers/_%03d.png", 0, 
                              number_test, 
                              train_data,  test_data,
                              train_label, test_label);
        
        read_and_process_data("../../dataset/nuts/_%03d.png", 1, 
                              number_test, 
                              train_data,  test_data,
                              train_label, test_label);
        
        read_and_process_data("../../dataset/bolts/_%03d.png", 2, 
                              number_test, 
                              train_data,  test_data,
                              train_label, test_label);
        std::cout<<train_data.size()<<std::endl;

        cv::Mat train_data_mat(train_data.size()/3, 3, CV_32FC1, &train_data[0]);
        cv::Mat train_label_mat(train_label.size(), 1, CV_32S, &train_label[0]);

        cv::Mat test_data_mat(test_data.size()/3, 3, CV_32FC1, &test_data[0]);
        cv::Mat test_label_mat(test_label.size(), 1, CV_32FC1, &test_label[0]);

        cv::Ptr<cv::ml::SVM> svm;

        svm = cv::ml::SVM::create();

        svm->setType(cv::ml::SVM::C_SVC);
        svm->setKernel(cv::ml::SVM::CHI2);
        svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));

        svm->train(train_data_mat, cv::ml::ROW_SAMPLE, train_label_mat);

        cv::Mat test_predict;
        svm->predict(test_data_mat, test_predict);
        cv::Mat error_mat = test_predict != test_label_mat;
        float error = 100.0f * countNonZero(error_mat) / test_label.size();
        std::cout<<"Error: " << error<< "%" <<std::endl;

        svm->save("../../output.xml");
    }

    cv::Mat predict_image(cv::Mat image, std::vector<cv::Rect>* rectangles_output=NULL)
    {
        cv::Mat croped_image = crop_image(image, cropped_top, cropped_bottom);
        cv::Ptr<cv::ml::SVM> svm;
        svm= cv::Algorithm::load<ml::SVM>("../../output.xml"); // something is wrong
        Mat img_output= image.clone();
        std::vector<cv::Point2d> points;
        std::vector<cv::Rect> rectangles;
        cv::Mat processed_image = extract_background(croped_image, background, 0);
        std::vector<std::vector<float>> features = extract_feature(processed_image, &points, &rectangles);

        for (int i = 0; i < features.size(); i++)
        {
            Mat data_mat(1,3, CV_32FC1, &features[i][0]);
            float result= svm->predict(data_mat);
            // std::cout << result << std::endl;
            
            std::stringstream ss;
            cv::Scalar color;
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
            
            cv::Point2d correct_pose(points[i].x+cropped_top.x, points[i].y+cropped_top.y);
            cv::Rect correct_rect_pose(rectangles[i].x+cropped_top.x, rectangles[i].y+cropped_top.y, rectangles[i].width, rectangles[i].height);
            cv::putText(img_output, ss.str(), correct_pose, FONT_HERSHEY_SIMPLEX, 0.8, color);
            cv::rectangle(img_output, correct_rect_pose,color);
            cv::circle(img_output, correct_pose, 3,color, -1);

            if(rectangles_output != NULL)
            {
                rectangles_output->push_back(correct_rect_pose);
            }
        }
        return img_output;        
    }

    private:
        cv::Mat background;
        cv::Point2i cropped_top = cv::Point2i(128,111);
        cv::Point2i cropped_bottom = cv::Point2i(525,320);

};