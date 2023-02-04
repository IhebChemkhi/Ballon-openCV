#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include  <cstdlib>
#include <Windows.h>

#define w 400

using namespace cv;
using namespace std;
int game()
{
    VideoCapture cap(0); //capture the video from webcam

    if (!cap.isOpened())
    {
        cout << "erreur" << endl;
        return -1;
    }
    Mat image = imread("ballon.png");
    int height = image.cols;
    int width = image.rows;
    
    float tx = float(width) / 4;
    float ty = float(height) / 4;
    float warp_values[] = { 1.0, 0.0, tx, 0.0, 1.0, ty };

    int iLowH = 170;
    int iHighH = 179;

    int iLowS = 150;
    int iHighS = 255;

    int iLowV = 60;
    int iHighV = 255;

    int x = w / 2;
    int y = w;

    int iLastX = -1;
    int iLastY = -1;


    Mat imgTmp;
    cap.read(imgTmp);




    int score = 0;
    bool gameover = false;
    while (true)
    {
        if (gameover == false && GetKeyState('A') & 0x8000)
        {
            y = w;
        }
        else if (gameover == true && GetKeyState('A') & 0x8000) {
            return game();
        }
        Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);;
        Mat mask = Mat::zeros(imgTmp.size(), CV_8UC3);;
        Mat imgOriginal;
        bool bSuccess = cap.read(imgOriginal);
        if (gameover == false) {
            putText(imgOriginal, "Score: " + to_string(score), cv::Point(10, 50), cv::FONT_HERSHEY_DUPLEX, 1.0, CV_RGB(118, 185, 0), 2);
            circle(imgLines, Point(x, y), w / 32, Scalar(0, 0, 255), FILLED, LINE_8);
            y = y - 5;
        }
        else {
            putText(imgOriginal, "Score: " + to_string(score), cv::Point(imgOriginal.rows / 2, imgOriginal.rows / 2), cv::FONT_HERSHEY_DUPLEX, 1.0, CV_RGB(118, 185, 0), 2);
        }
        if (!bSuccess)
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }

        Mat imgHSV;

        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);
        Mat imgThresholded;

        inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        vector<vector<Point>>contours;
        vector<Vec4i> hierarchy;

        findContours(imgThresholded, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);




        //Calculate the moments of the thresholded image
        Moments oMoments = moments(imgThresholded);

        double dM01 = oMoments.m01;
        double dM10 = oMoments.m10;
        double dArea = oMoments.m00;
        

        // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
        if (dArea > 10000)
        {
            //calculate the position of the ball
            int posX = dM10 / dArea;
            int posY = dM01 / dArea;

            if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
            {

                // circle(imgLines, Point(iLastX, iLastY), 10, Scalar(0, 0, 255), 2);
                for (size_t cC = 0; cC < contours.size(); ++cC)
                    for (size_t cP = 0; cP < contours[cC].size(); cP++)
                    {
                        if (contours[cC][cP].x == x && contours[cC][cP].y == y)
                        {
                            score++;
                            x = rand() % w;
                            y = w;
                        }

                    }
            }

            iLastX = posX;
            iLastY = posY;
        }




        imshow("Thresholded Image", imgThresholded);


        imgOriginal = imgOriginal + imgLines + mask;
        imshow("Original", imgOriginal);
        if (y <= 0) {
            gameover = true;
        }
        if (waitKey(30) == 27)
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }

    }
    destroyAllWindows();
    return 0;
}
int main(int argc, char** argv)
{
    game();
}