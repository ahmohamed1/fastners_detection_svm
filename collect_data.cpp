/*
To run the code using g++ compiler: 
g++ collect_data.cpp -I C:/src/vcpkg/installed/x64-windows/include -L C:/src/vcpkg/installed/x64-windows/lib -lopencv_core -lopencv_highgui

*/



#include<iostream>
#include <fstream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

// Define link to data files
const string dataset_root = "dataset";
const string fastners_name [3] = {"bolts", "nuts", "washers"};
int images_number[3] = {0,0,0};


string check_file(int fastners)
{
    std::string savingName = dataset_root + "/"  + fastners_name[fastners] + std::to_string(++images_number[fastners]) + ".png";
    while (1)
    {
        FILE *file;
        std::ifstream ifile(savingName.c_str());
        if (ifile)
        {
            savingName = dataset_root + "/"  + fastners_name[fastners] + std::to_string(++images_number[fastners]) + ".png";
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
    if (!cap.isOpened())
    {
        cout << "[ERROR] Video is not streaming..." <<endl;
        return -1;
    }

    while(1)
    {
        // capture image 
        cv::Mat frame ;
        cap >> frame;

        // Check if we capture image
        if (frame.empty())
        {
            cout<< "[ERROR] No image was captureed" <<endl;
            break;
        }
 

        cv::imshow("Frame", frame);
        char ikey = cv::waitKey(0);

        if (ikey == 'q')
            break;
        else if (ikey == 'b')
        {
            string image_name = check_file(0);
            cv::imwrite(image_name, frame);
        }
        else if (ikey == 'n')
        {
            string image_name = check_file(1);
            cv::imwrite(image_name, frame);
        }
        else if (ikey == 'w')
        {
            string image_name = check_file(2);
            cv::imwrite(image_name, frame);
        }

    }

    cap.release();
    cv::destroyAllWindows();
    
    return 0;
}