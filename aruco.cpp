#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>
#include <string>

using namespace std;

class Aruco{
    public:
        void load_camera_parameters(std::string path)
        {
            FileStorage fs(path, FileStorage::READ);
            fs ["cameraMatrix"] >> interinsic;
            fs ["distCoeffs"] >> distCoeffs;
            fs.release();
        }

        void detect_aruco(cv::Mat input_image)
        {
            std::vector< int > markerIds;
            cv::aruco::detectMarkers(input_image, dictionary, markerCorners, markerIds);
            // compute the position of the markers
            cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.07, interinsic, distCoeffs, rvecs, tvecs);
            if (markerIds.size() > 0)
                detected_marker_succee = true;
            else
                detected_marker_succee = false;
        }

        cv::Mat show_aruco(cv::Mat input_image)
        {
            if (detected_marker_succee)
            {
                cv::Mat output_image;
                cv::Point2f center;
                cv::aruco::drawDetectedMarkers(inputImage, markerCorners, markerIds);
                center.x = (markerCorners[0][0].x + markerCorners[0][1].x + markerCorners[0][2].x+ markerCorners[0][3].x)/4;
                center.y = (markerCorners[0][0].y + markerCorners[0][1].y + markerCorners[0][2].y+ markerCorners[0][3].y)/4;
                circle(input_image,center,5,Scalar(0,0,255), 2, 8, 0);

                for(int i =0;i< markerIds.size();i++)
                {
                    cv::aruco::drawAxis(output_image, interinsic, distCoeffs, rvecs[i], tvecs[i], 0.1);
                    cout<<"Ide: " <<markerIds[i]  <<"Pos: "<<tvecs[i] <<endl; //<< "\tRot: " << rvecs[i]<<endl;
                }
                return output_image;
           }
           return NULL;
        }
        
        void create_arcue(int number){
            int dictionaryId = 1;
            int markerId = number;
            int borderBits = 1;
            int markerSize = 20;
            bool showImage = true;

            ostringstream convert;   // stream used for the conversion

            convert << number;      // insert the textual representation of 'Number' in the characters in the stream

            string Result = convert.str();

            String out = "../../config/" + Result + ".jpg";



            Ptr<aruco::Dictionary> dictionary =
                aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));

            Mat markerImg;
            aruco::drawMarker(dictionary, markerId, markerSize, markerImg, borderBits);

            if(showImage) {
                imshow("marker", markerImg);
                waitKey(0);
            }

            imwrite(out, markerImg);
        }

    private:
        cv::Mat interinsic = Mat(3, 3, CV_32FC1);
        cv::Mat distCoeffs;
        cv::Ptr<cv::aruco::Dictionary> dictionary=aruco::getPredefinedDictionary(1);
        std::vector< std::vector<cv::Point2f> > markerCorners, rejectedCandidates;
        std::vector<cv::Vec3d > rvecs, tvecs;
        bool detected_marker_succee = false;

};


int main(int argc, char *argv[]) {


    Mat interinsic = Mat(3, 3, CV_32FC1);
    Mat distCoeffs;

    //load the camera parameters
    string filename = "../../config/" + string("camera_calibration.xml");


    VideoCapture vid(0);

    if(!vid.isOpened()) {
        cout<<" The camera is no open!!"<<endl;
    }


    while(vid.grab()) 
    {
        cv::Mat input_image;
        vid.read(input_image);
        // implement the detection
        
        cv::imshow("out", input_image);
        char key = (char) cv::waitKey('q');
        if (key == 'q')
            break;
    }

    return 0;
}
