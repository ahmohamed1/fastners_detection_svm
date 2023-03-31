# Fastners detection with Support Vector Machine

Fasteners, such as nuts, bolts and washers, are essential components in many mechanical systems. In this repostory, I will explore how to use OpenCV C++ to program software to automatically detect fasteners in an image using a Support Vector Machine (SVM) and feature engineering techniques.

## Case : 
In manufacturing facilities, it is common to use conveyor belts to transport products from one place to another. Fasteners such as nuts, bolts, and washers are often used in manufacturing processes, and it is important to detect and count them accurately to ensure quality control. In this blog post, we will see how to use OpenCV and C++ to detect fasteners on a conveyor belt using machine learning techniques.
## Feature Extractions
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



## Sources:
[1] OpenCV By Example by Prateek Joshi, David Millán Escrivá and Vinícius Godoy


## Steps:
1. Image collection
2. Feature Extractions
3. Train SVM
4. Implement the trained model
5. Track fastners at speed of conveyor
6. Count system
