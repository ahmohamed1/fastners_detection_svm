/**
 * @file Centroid_Tracker.h
 *
 * @brief This is an implementation to track an object using center of object
 *
 *
 * @author Abdulla Mohamed
 * Contact: abdll1@hotmail.com
 *
 */

#include <iostream>
#include "opencv2/opencv.hpp"


struct Centroid{
    cv::Rect rectangle;
    cv::Point2d center;
    int id;
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



class Centroid_Tracker{
    public:

    Centroid_Tracker() = default;
    Centroid_Tracker(cv::Point2d _line_point_1, cv::Point2d _line_point_2) : 
                     line_point_1(_line_point_1), line_point_2(_line_point_2){}

    int get_count()
    {
        return count;
    }
    std::vector<Centroid> update(std::vector<cv::Rect> rectangles)
    {
        // remove objects and start count to remove the center
        if(rectangles.empty())
        {
            for (int i = 0; i < tracked_objects.size(); i++)
            {
                tracked_objects[i].disapeared += 1;
                if (tracked_objects[i].disapeared > max_disappeared)
                {
                    deregister_object(tracked_objects[i].id);
                }
            }
            return tracked_objects;
        }

        // if first time and the objects is empty assigne new ideas
        if(tracked_objects.empty())
        {
            register_loop(rectangles);
        }
        // match the new rectangle to the old rectangles
        else
        {
            for (int i = 0; i < tracked_objects.size(); i++)
            {
                bool object_exsits = false;
                for (int j = 0; j < rectangles.size(); j++)
                {
                    if(compute_eclidean_distance(tracked_objects[i].rectangle, rectangles[j]) < max_distance)
                    {
                        tracked_objects[i].set_rectangle(rectangles[j]);
                        object_exsits = true;
                        pass_line(tracked_objects[i]);
                        rectangles.erase(rectangles.begin() + j);
                        continue;
                    }
                }

                if(object_exsits == false)
                {
                    if(tracked_objects[i].disapeared >= max_disappeared)
                    {
                        deregister_object(tracked_objects[i].id);
                    }else
                    {
                        tracked_objects[i].disapeared += 1;
                    }
                }
            }
            register_loop(rectangles);
            return tracked_objects;
        }   
        return tracked_objects;     
    }

    void pass_line(Centroid &center)
    {
        if(center.center.x >= line_point_1.x && center.counted == false)
        {
            center.counted = false;
        }
        else
        {
            if(center.counted == false)
            {
                center.counted = true;
                count ++;
            }
        }
    }


    private:
        int object_id = 0;
        std::vector<Centroid> tracked_objects;
        int max_disappeared = 50;
        int max_distance = 20;
        cv::Point2d line_point_1;
        cv::Point2d line_point_2;
        int count = 0;
        
        void register_object(cv::Rect rectangle)
        {
            auto center = compute_center(rectangle);
            tracked_objects.push_back(Centroid(rectangle, center, object_id, 0));
            object_id++;
        }

        void deregister_object(int object_id)
        {
            // tracked_objects.erase(remove(tracked_objects.begin(), tracked_objects.end(), object_id), tracked_objects.end());
            // Use std::remove_if to move the matching element(s) to the end of the vector
            auto it = std::remove_if(tracked_objects.begin(), tracked_objects.end(), [object_id](const Centroid& d) {
                return d.id == object_id;
            });

            // Use std::erase to remove the matching element(s) from the vector
            tracked_objects.erase(it, tracked_objects.end());
        }

        cv::Point2d compute_center(cv::Rect rectangle)
        {
            
            float x = (rectangle.x + (rectangle.x+rectangle.width))/2;
            float y = (rectangle.y + (rectangle.y+rectangle.height))/2;
            cv::Point2d center(x,y);
            return center;
        }

        float compute_eclidean_distance(cv::Rect rect_1, cv::Rect rect_2)
        {
            auto center_1 =  compute_center(rect_1);
            auto center_2 =  compute_center(rect_2);
            return sqrt(pow(center_1.x-center_2.x, 2)+ pow(center_1.y-center_2.y,2));
        }

        void register_loop(std::vector<cv::Rect> rectangles)
        {
            for (int x = 0; x < rectangles.size(); x++)
            {
                register_object(rectangles[x]);
            }
        }



};