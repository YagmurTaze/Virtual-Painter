#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

Mat img;

// my colors
vector<vector<int>> colorsHSV = { {0, 18, 144, 255, 189, 255}, // orange
                                 {97, 121, 120, 255, 204, 255} }; // blue

vector<Scalar> colorsBGR{ {9,102, 255}, // orange BGR
                          {236, 178, 94}  }; // blue BGR

vector<vector<int>> newPoints;  // to store all drawing points

Point getContour(Mat mask, Mat& img) {

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy; // no use

    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<vector<Point>> conPoly(contours.size());
    vector<Rect> boundRect(contours.size());

    Point myPoint(0, 0);

    for (int i = 0; i < contours.size(); i++) {
        int area = contourArea(contours[i]);

        if (area > 1100) {

            float peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

            boundRect[i] = boundingRect(conPoly[i]);
            myPoint.x = boundRect[i].x;
            myPoint.y = boundRect[i].y;

            drawContours(img, contours, i, Scalar(255, 0, 255), 2);
        }
    }

    return myPoint;
}

vector<vector<int>> findColor(Mat& img) {
    Mat imgHSV, mask;

    cvtColor(img, imgHSV, COLOR_BGR2HSV);

    for (int i = 0; i < colorsHSV.size(); i++) {

        Scalar lower(colorsHSV[i][0], colorsHSV[i][2], colorsHSV[i][4]);
        Scalar upper(colorsHSV[i][1], colorsHSV[i][3], colorsHSV[i][5]);
        inRange(imgHSV, lower, upper, mask);
        Point contour = getContour(mask, img);
        if (contour.x != 0) {
            newPoints.push_back({ contour.x,contour.y,i });
        }
    }

    return newPoints;
}

void drawOnCam(vector<vector<int>> newPoints, vector<Scalar> colorsBGR)
{

    for (int i = 0; i < newPoints.size(); i++)
    {
        circle(img, Point(newPoints[i][0], newPoints[i][1]), 8, colorsBGR[newPoints[i][2]], FILLED);
    }
}


int main() {

    VideoCapture cap(0);

    while (true) {

        cap.read(img);
        flip(img, img, 1);

        newPoints = findColor(img);
        drawOnCam(newPoints, colorsBGR);

        imshow("Camera", img);
        waitKey(1);
    }

    return 0;
}
