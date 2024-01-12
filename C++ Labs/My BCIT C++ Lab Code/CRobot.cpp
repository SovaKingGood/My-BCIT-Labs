#include "pigpio.h"
#include <opencv2/opencv.hpp>
#include "CRobot.h"
#include "cvui.h"
#include <thread>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>


CRobot::CRobot() {

    gpioInitialise();

    gpioSetMode(26,PI_OUTPUT); //Input 1
    gpioSetMode(19,PI_OUTPUT); //Input 2
    gpioSetMode(13,PI_OUTPUT); //Input 3
    gpioSetMode(6,PI_OUTPUT);  //Input 4

    gpioSetPullUpDown(26, PI_PUD_DOWN);
    gpioSetPullUpDown(19, PI_PUD_DOWN);
    gpioSetPullUpDown(13, PI_PUD_DOWN);
    gpioSetPullUpDown(6, PI_PUD_DOWN);

    cvui::init("Robot");
    cv::Size size(1920/2, 1080/2);
    frame = cv::Mat::zeros(size, CV_8UC3);

    std::cout << "Opening Camera";
    //vid.open(0, cv::CAP_V4L);
    //vid.set(cv::CAP_PROP_FRAME_WIDTH, 1920/2);
    //vid.set(cv::CAP_PROP_FRAME_HEIGHT, 1080/2);
    //vid.set(cv::CAP_PROP_FPS, 30);
   // vid.set(cv::CAP_PROP_BUFFERSIZE , 1);


    count = 10;




}
std::string CRobot::gstreamer_pipeline(int capture_width, int capture_height, int framerate, int display_width, int display_height) { //https://github.com/Qengineering/Libcamera-OpenCV-RPi-Bullseye-64OS/blob/main/main.cpp
    return
            " libcamerasrc ! video/x-raw, "
            " width=(int)" + std::to_string(capture_width) + ","
            " height=(int)" + std::to_string(capture_height) + ","
            " framerate=(fraction)" + std::to_string(framerate) +"/1 !"
            " videoconvert ! videoscale !"
            " video/x-raw,"
            " width=(int)" + std::to_string(display_width) + ","
            " height=(int)" + std::to_string(display_height) + " ! appsink";
}
CRobot::~CRobot() {

    vid.release();
    gpioTerminate();
    cv::destroyAllWindows();
}

void CRobot::Run() {

    std::thread FL(&CRobot::wheel_thread_FL, this);
	//std::thread FR(&CRobot::wheel_thread_FR, this);
	//std::thread BL(&CRobot::wheel_thread_BL, this);
	//std::thread BR(&CRobot::wheel_thread_BR, this);

    //pipeline parameters
    int capture_width = 640/4; //1280 ;
    int capture_height = 480/4; //720 ;
    int framerate = 5;
    int display_width = 640; //1280 ;
    int display_height = 480; //720 ;

    //reset frame average
    std::string pipeline = gstreamer_pipeline(capture_width, capture_height, framerate,
                                              display_width, display_height);
    std::cout << "Using pipeline: \n\t" << pipeline << "\n\n\n";



cv::VideoCapture vid(pipeline, cv::CAP_GSTREAMER);


    do {


       if (vid.isOpened() == true){
       frame = cv::Mat::zeros(size, CV_8UC3);
       vid >> frame;
/*
       cvui::printf(frame, 100,150,1,0xFFFFFF, "Speed = %d", count);
       cvui::context("Robot");

       if (cvui::button(frame, 50,0,"Increase")){
       count--;
       }

       if (cvui::button(frame, 50,75,"Decrease")){
       count++;
       }
       if (cvui::button(frame, 50,150,"Quit")){

       }

       cvui::update("Robot");

*/
       cv::imshow("Robot",frame);

       //cv::waitKey(100);

}
else{

break;
}
    }while(cv::waitKey(5) != 'q');

    std::cout<< "Closing Program...\n";
    _thread_exit = 1;

    FL.join();
    //FR.join();
    //BL.join();
    //BR.join();

}



void CRobot::wheel_thread_FL(CRobot* ptr)
{
	while (ptr->_thread_exit == false){
		ptr->wheel_FL();

	}
}

void CRobot::wheel_thread_FR(CRobot* ptr)
{
	while (ptr->_thread_exit == false){
		ptr->wheel_FR();

	}
}

void CRobot::wheel_thread_BL(CRobot* ptr)
{
	while (ptr->_thread_exit == false){
		ptr->wheel_BL();

	}
}

void CRobot::wheel_thread_BR(CRobot* ptr)
{
	while (ptr->_thread_exit == false){
		ptr->wheel_BR();

	}
}

void CRobot::wheel_FL(){

        cv::waitKey(count);
       // std::cout << "Step: 1\n";

        gpioWrite(26, 1);
        gpioWrite(19, 0);
        gpioWrite(13, 0);
        gpioWrite(6, 0);

        cv::waitKey(count);
        //std::cout << "Step: 2\n";
        gpioWrite(26, 0);
        gpioWrite(19, 1);
        gpioWrite(13, 0);
        gpioWrite(6, 0);

        cv::waitKey(count);
        //std::cout << "Step: 3\n";
        gpioWrite(26, 0);
        gpioWrite(19, 0);
        gpioWrite(13, 1);
        gpioWrite(6, 0);

        cv::waitKey(count);
        //std::cout << "Step: 4\n";
        gpioWrite(26, 0);
        gpioWrite(19, 0);
        gpioWrite(13, 0);
        gpioWrite(6, 1);

}


void CRobot::wheel_FR(){

}

void CRobot::wheel_BL(){

}

void CRobot::wheel_BR(){

}

