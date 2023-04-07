# Fastners detection with Support Vector Machine

Fasteners, such as nuts, bolts and washers, are essential components in many mechanical systems. In this repostory, I will explore how to use OpenCV C++ to program software to automatically detect fasteners in an image using a Support Vector Machine (SVM) and feature engineering techniques.

## Case : 
In manufacturing facilities, it is common to use conveyor belts to transport products from one place to another. Fasteners such as nuts, bolts, and washers are often used in manufacturing processes, and it is important to detect and count them accurately to ensure quality control. In this blog post, we will see how to use OpenCV and C++ to detect fasteners on a conveyor belt using machine learning techniques.
## Feature Extractions
![Alt text](/figures/predictions.png?raw=true "Centroid class")

The first step in detecting fasteners is to preprocess the images captured by the camera on the conveyor belt. We want to remove any background noise so that we can focus on the fasteners only. To do this, we can use background subtraction techniques.

```
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
```
This function subtract the background from the actual image. The output of this function is a binary image.

## Extracting Features
Next, we will extract features from the binary image using contour analysis. Contours are the boundaries of objects in an image and can be used to extract various features of objects, such as area, perimeter, and shape. We will use the cv::findContours() function to find the contours in the binary image:

```
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
                }
            }
        }
        return output_image;
    }
```

To retrieve the contours from the preprocessed image, we use the cv::findContours() function with the cv::RETR_CCOMP flag, indicating that we want to retrieve all of the contours in the image with hierarchical relationships. We also use the cv::CHAIN_APPROX_SIMPLE flag to compress the contours by removing redundant points and approximating straight edges with only their endpoints.

Once we have the contours, we can extract features from each one. In this example, we will use three features: area, width-height ratio, and the presence of a hole. We can compute the area of a contour using the cv::contourArea() function.

To compute the width-height ratio, we use cv::minAreaRect() to get the bounding rectangle of the contour, from which we can obtain the width and height of the contour. We use a conditional statement to ensure that the ratio is not affected by the orientation of the contour:

```
(width < height) ? height/width : width/height;
```
Finally, we use the hierarchy information to determine if the contour has a child or not, and to skip any child contours. The hierarchy is represented as an array of four integers for each contour, indicating its next contour, previous contour, first child contour, and parent contour, respectively:

```
[Next, Previous, First_Child, Parent]
```
By extracting these features from the contours and using them to train an SVM classifier, we can accurately detect fasteners on a conveyor belt in real-world applications.

## Traning SVM 

```
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

```

In summary, this code performs the following steps:

1. Reads image data from three different directories: "washers", "nuts", and "bolts".
2. Splits the image data into training and testing sets.
3. Converts the training and testing data into matrices using cv::Mat class.
4. Creates an SVM model using cv::ml::SVM::create() function and sets its properties.
5. Trains the SVM model using the training data and their corresponding labels.
6. Predicts the labels of the test data using the trained SVM model.
7. Calculates the prediction error.
8. Saves the trained SVM model to an XML file named "output.xml".


## Prediction
```
 cv::Mat predict_image(cv::Mat image, std::vector<cv::Rect>* rectangles_output=NULL, std::vector<int>* classes_list=NULL)
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
                if(classes_list != NULL)
                    classes_list->push_back(result);
            }
        }
        return img_output;        
    }

```
This code defines a function predict_image that takes an input image, and returns an output image with annotations and labels for each detected mechanical part in the input image. The function uses an SVM model trained on mechanical part images to predict the type of each detected part.

The input image is first cropped. Then, the function loads the trained SVM model from an XML file using cv::Algorithm::load<ml::SVM>("../../output.xml").

The function then extracts the background from the cropped image using the extract_background function, and extracts features from the resulting image using the extract_feature function. The features are passed through the SVM model to predict the type of each detected part.

For each detected part, the function adds a text label and a bounding box to the output image indicating the predicted type of the part. The function also adds the bounding box and the predicted type to the rectangles_output and classes_list vectors, if these are provided as input arguments.

Finally, the function returns the output image with annotations and labels.

# Counting
To determine the number of objects in the system, we utilize centroid tracking algorithms to continuously track the objects present in the image sequences. Each object is assigned a unique ID to facilitate data processing. To streamline the information, we created a data structure that contains all the necessary information.

```
struct Centroid{
    cv::Rect rectangle;
    cv::Point2d center;
    int id;
    int class_number;
    int disapeared;
    bool counted;

    void set_rectangle(cv::Rect rect)
    {
        rectangle = rect;
        float x = (rectangle.x + (rectangle.x+rectangle.width))/2;
        float y = (rectangle.y + (rectangle.y+rectangle.height))/2;
        center = cv::Point2d(x,y);
    }
};
```
The centroid tracking algorithm is used to track objects in image sequences based on the Euclidean distance between the objects in consecutive frames. Initially, each object in the first frame is assigned a unique ID. In the second frame, the assigned IDs are compared to the newly detected objects using Euclidean distance. If most of the objects in the old frame match with the new objects, the remaining objects are assigned new IDs and added to the list.

In case an object is lost in the tracking process, the ID remains in the frame for a few frames in case there was any issue in the detection process. Otherwise, after the maximum_frame, the object is removed from the list. To count the objects, a line is drawn to compare the position of the objects. If the objects pass the line, the count is incremented, and the object is marked as counted. Note that the object count depends on the movement of the object, and the system can count each class separately.

![Alt text](/figures/centroid_tracking.png?raw=true "Centroid class")

## Sources:
[1] OpenCV By Example by Prateek Joshi, David Millán Escrivá and Vinícius Godoy


## Steps:
1. Image collection
2. Feature Extractions
3. Train SVM
4. Implement the trained model
5. Track fastners at speed of conveyor
6. Count system
