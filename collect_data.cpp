/*
To run the code using g++ compiler: 
g++ collect_data.cpp -I C:/src/vcpkg/installed/x64-windows/include -L C:/src/vcpkg/installed/x64-windows/lib -lopencv_core -lopencv_highgui

*/


#include<iostream>
#include <fstream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;


cv::Point2i top_corner(0,0), bottom_corner(0,0);
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == cv::EVENT_LBUTTONDOWN && top_corner.x == 0)
    {
        cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
        top_corner.x = x;
        top_corner.y = y;
    } else if  ( event == cv::EVENT_LBUTTONDOWN && bottom_corner.x == 0)
    {
        cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
        bottom_corner.x = x;
        bottom_corner.y = y;
    } 
}

// Define link to data files
const string dataset_root = "../../dataset/";
const string fastners_name [4] = {"bolts", "nuts", "washers", "screws"};
int images_number[4] = {0,0,0,0};


string check_file(int fastners)
{
    char number_str[5];
    sprintf(number_str, "_%03i", images_number[fastners]);
    std::string savingName = dataset_root + fastners_name[fastners] + "/" + number_str + ".png";
    cout<<savingName<<endl;
    while (1)
    {
        FILE *file;
        std::ifstream ifile(savingName.c_str());
        if (ifile)
        {
            sprintf(number_str, "_%03i", ++images_number[fastners]);
            savingName = dataset_root + fastners_name[fastners] + "/" + number_str + ".png";
            cout<< "Saved name: " <<savingName <<endl;
        }
        else
        {
            return savingName;
        }
    }
    
}

int main()
{
    cv::VideoCapture cap(0) ;
    cap.set(cv::CAP_PROP_AUTOFOCUS, 0); // turn the autofocus off
    cv::namedWindow("Frame");
    cv::setMouseCallback("Frame", CallBackFunc);

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
        rectangle_image = frame.clone();
        cv::Mat output_image;
        // Check if we capture image
        if (frame.empty())
        {
            cout<< "[ERROR] No image was captureed" <<endl;
            break;
        }
 
        if ( bottom_corner.x != 0 && top_corner.x != 0)
        {
            cv::rectangle(rectangle_image, top_corner,bottom_corner, cv::Scalar(0,255,0));
            output_image = frame(cv::Range(top_corner.y, bottom_corner.y), 
                                   cv::Range(top_corner.x, bottom_corner.x));
        }
        cv::imshow("Frame", rectangle_image);
        char ikey = cv::waitKey(1);

        if (ikey == 'q')
            break;
        else if (ikey == 'b')
        {
            string image_name = check_file(0);
            cv::imwrite(image_name, output_image);
            cout << "[INFO] bolts was captured successfully.." <<endl;
        }
        else if (ikey == 'n')
        {
            string image_name = check_file(1);
            cv::imwrite(image_name, output_image);
            cout << "[INFO] nuts was captured successfully.." <<endl;
        }
        else if (ikey == 'w')
        {
            string image_name = check_file(2);
            cv::imwrite(image_name, output_image);
            cout << "[INFO] washer was captured successfully.." <<endl;
        }
        else if (ikey == 's')
        {
            string image_name = check_file(3);
            cv::imwrite(image_name, output_image);
            cout << "[INFO] screw was captured successfully.." <<endl;
        }
        else if(ikey == 'i')
        {
            cv::imwrite("../../dataset/background.png", output_image);
            cout << "[INFO] background was captured successfully.." <<endl;
        }else if(ikey == 'o')
        {
            bottom_corner.x = 0;
            top_corner.x = 0;
        }

    }

    cap.release();
    cv::destroyAllWindows();
    
    return 0;
}