#include <opencv2/opencv.hpp>
#include<opencv2/ccalib/omnidir.hpp>
#include <iostream>

using namespace cv;
using namespace std;



vector<vector<Point3f> > objectPoints;			// This is the phiscal point on the board
vector<vector<Point2f> > imagePoint;			// This to store the point in the image plane location of corner

int main(int argc, char* argv[]){


	// Define the input virable from the user
	int numBoard = 0;
	int numCornHer = 0;
	int numCornVer = 0;
	int sucsses = 0;	// How many sucess we have in finding the corner

	int patternWasFound = 0; //used later for corners

	// ask the user of the input
	cout << "Enter the number of board you need for the calibration: " << endl;
	cin >> numBoard;
	cout << "Enter number of corner in Horezintal: ";
	cin >> numCornHer;
	cout << "Enter number of conrenr in vertical: ";
	cin >> numCornVer;

	//evaluate the input data for future work
	int numSquare = numCornHer*numCornVer;
	Size boardSize = cv::Size(numCornHer, numCornVer);
	// Create a storage for point from the images for calibration



	// This storage for finding corner
	vector<cv::Point2f> corners;					// to hold the corner position
	int cornerCount = 0;							// Indicate how many corners are find

	// This storage for the actual coordinate refrence we can use the dimension
	// as (0,0,0),(0,1,0),(0,2,0),.....(1,4,0),....
	vector<Point3f> obj;
	for (int j = 0; j < numSquare; j++){
		obj.push_back(Point3f((j / numCornHer) * 30, (j%numCornHer) * 30, 0.0f));		// we use a unit because we are ni=ot the concern to dimension
		//cout << Point3f((j / numCornHer)*30, (j%numCornHer)*30, 0.0f) << endl;		// The dimisional is in mm
	}

    VideoCapture vid(0);		// Define the camera and assing it's number
	if (!vid.isOpened()){		// Check if the camera connected sucssufully
		cout << "[ERROR] The camera cannot be open..\n";
	}


	Mat frame;
	Mat grayImg;		// to store the gray image for process
	namedWindow("Left Window");

	// This loop to store the corner in the opbjectpoint
	bool bSucssCornerFind = true;
	while (bSucssCornerFind){

		bool bSucsCapture = vid.read(frame);
		if (bSucsCapture == false){
			cout << "Error unable to capture frame ...\n";
			return -1;
		}

		cvtColor(frame, grayImg, CV_BGR2GRAY);
		//findChessboardCorners(grayImg,patternSize,corners,cornerCount,CV_CALIB_CB_FAST_CHECK);
		patternWasFound = findChessboardCorners(grayImg, boardSize, corners, CV_CALIB_CB_ADAPTIVE_THRESH + CV_CALIB_CB_FILTER_QUADS);
		if (patternWasFound){
			cornerSubPix(grayImg, corners, Size(5, 5), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS, 30, 0.1));
		}

		drawChessboardCorners(frame, boardSize, corners, patternWasFound);

		imshow("Left Window", frame);

		char iKey = waitKey('0');

		if (iKey == 'q'){
			break;
			return 0;
		}

		//		if (iKey == 's' && patternWasFound != 0){	// if the corner found and the user press s it will store the corner data in the imagePoint
        if (iKey=='s' && patternWasFound != 0){	// if the corner found and the user press s it will store the corner data in the imagePoint


			imagePoint.push_back(corners);			// Store corners values in the image point for calibration
			objectPoints.push_back(obj);			// Stroe the phisical position in the object point for calibration
			cout << "Corners Stored suceessfully :" << sucsses << endl;
			sucsses++;
			iKey = '0';
			if (sucsses >= numBoard){
				bSucssCornerFind = false;
			}
		}	//End if statment
	}	//End the while loop

	destroyWindow("Left Window");

	// These storage for the intrinsic and distortion coffeicent
	Mat interinsic = Mat(3, 3, CV_32FC1);
	Mat distCoeffs;
	vector<Mat> Rvect;
	vector<Mat> Tvect;


	// Calibrate the camera
	//calibrateCamera(objectPoints, imagePoint, frame.size(), interinsic, distCoeffs, Rvect, Tvect);

	// This calibration for omni camera
	cv::Mat K, xi, D, idx;
	int flags = 6;
	cv::TermCriteria critia(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 200, 0.0001);
	std::vector<cv::Mat> rvecs, tvecs;
    double rms = cv::calibrateCamera(objectPoints,
                                     imagePoint,
                                     frame.size(),
                                     interinsic,
                                     distCoeffs,
                                     Rvect,
                                     Tvect);
	cout << "Error: " << rms<<endl;
	// Test the result to show undistrotion image
	while (1){

		Mat undistortFrame, Knew;
		Size new_size;
		vid.read(frame);
        undistort(frame, undistortFrame, interinsic, distCoeffs);
        //cv::omnidir::undistortImage(frame, undistortFrame, K, D, xi,1, Knew, new_size);
	//	imshow("Left Window", frame);
		imshow("Left Undistorted window", undistortFrame);

		char iKey = waitKey('q');
		if (iKey == 'q'){
			break;				// Exit the loop
		}
	}

	// Save the result in a xml file
    string filename = "../../config/" + string("calibration.xml");
	FileStorage fs(filename, FileStorage::WRITE);
	fs << "cameraMatrix" << interinsic;
	fs << "distCoeffs" << distCoeffs;
	fs.release();
	cout << "Saved calibration matrices to " << filename << endl;

	char IKey ='0';
	while (IKey != 'q'){
		IKey = waitKey('q');
	}
	return 0;
}
