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
const string dataset_root = "../../dataset/";
const string fastners_name [3] = {"bolts", "nuts", "washers"};
int images_number[3] = {0,0,0};


string check_file(int fastners)
{
    char number_str[3];
    sprintf(number_str, "_%03i", ++images_number[fastners]);
    std::string savingName = dataset_root + "/"  + fastners_name[fastners] + "/" + fastners_name[fastners] + number_str + ".png";
    cout<<savingName<<endl;
    while (1)
    {
        FILE *file;
        std::ifstream ifile(savingName.c_str());
        if (ifile)
        {
            sprintf(number_str,"_%03i", std::to_string(++images_number[fastners]));
            savingName = dataset_root + fastners_name[fastners] + "/" + fastners_name[fastners] + number_str + ".png";
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
        char ikey = cv::waitKey(1);

        if (ikey == 'q')
            break;
        else if (ikey == 'b')
        {
            string image_name = check_file(0);
            cv::imwrite(image_name, frame);
            cout << "[INFO] bolts was captured successfully.." <<endl;
        }
        else if (ikey == 'n')
        {
            string image_name = check_file(1);
            cv::imwrite(image_name, frame);
            cout << "[INFO] nuts was captured successfully.." <<endl;
        }
        else if (ikey == 'w')
        {
            string image_name = check_file(2);
            cv::imwrite(image_name, frame);
            cout << "[INFO] washer was captured successfully.." <<endl;
        }

    }

    cap.release();
    cv::destroyAllWindows();
    
    return 0;
}