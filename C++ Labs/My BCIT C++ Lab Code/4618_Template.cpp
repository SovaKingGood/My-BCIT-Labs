////////////////////////////////////////////////////////////////
// ELEX 4618 Template project for BCIT
// Created Oct 5, 2016 by Craig Hennessey
// Last updated Dec 2, 2022
////////////////////////////////////////////////////////////////

enum { DIGITAL = 0, ANALOG, SERVO };
enum { TOP = 26, RIGHT = 19, LEFT = 13, FLOW = 6};
enum {Rotary_Switch = 27, Rotary_A = 22, Rotary_B = 17};
enum {LED_G = 10, LED_R = 9, LED_B = 11};
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
// Add simple GUI elements
#define CVUI_DISABLE_COMPILATION_NOTICES
#define CVUI_IMPLEMENTATION
#include "cvui.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <cstdlib>
#include<algorithm>


//#include "Client.h"
//#include "Server.h"
#include "CControlPi.h"
#include "CBase4618.h"
#include "CSketch.h"
#include "CPong.h"
#include "CAsteroidGame.h"
#include "CRobot.h"
#include "pigpio.h"
#include "X11/Xlib.h"
#include <raspicam/raspicam_cv.h>

// Must include Windows.h after Winsock2.h, so Serial must be included after Client/Server
//#include "Serial.h"

#define CANVAS_NAME "Display Image"

void threadQuit(){

    while (1){
    if (cv::waitKey(10) == 'q'){

    gpioTerminate();
    cv::destroyAllWindows();
    }
    }




}
////////////////////////////////////////////////////////////////
// Lab 1
////////////////////////////////////////////////////////////////
void lab1()
{
int x;
}

////////////////////////////////////////////////////////////////
// Lab 2
////////////////////////////////////////////////////////////////
void lab2()
{

}


////////////////////////////////////////////////////////////////
// Lab 3
////////////////////////////////////////////////////////////////
void lab3()
{
//    Serial _com;
    XInitThreads();

    std::thread thread2(threadQuit);
    thread2.detach();

    CControlPi lab3;

    lab3.init_com(3); //COM PORT


    std::string input;

    cv::Mat _canvas = cv::Mat::zeros(cv::Size(100,100), CV_8UC3);

    cv::imshow("Q to terminate GPIO", _canvas);


    std::cout << "Analog Test (A)\n";
    std::cout << "Digital Test (D)\n";
    std::cout << "Button Test (B)\n";
    std::cout << "Servo Test (S)\n";


    do {
        std::getline(std::cin, input);
        if (input == "A" || input == "a") {
            int vertical;
            int horizontal;
            float vertical_f;
            float horizontal_f;


            std::cout << "Analog\n";

            do {

                cv::waitKey(100);
                lab3.get_analog(0, horizontal); //x
                cv::waitKey(100);
                lab3.get_analog(1, vertical); //y

                horizontal_f = horizontal;

                vertical_f = vertical;

               std::cout << std::setprecision(4) << "ANALOG TEST: CH0 = " << horizontal << "x("<< (horizontal_f ) / (1024) * 100 << "%)" << "CH1 = " << vertical << "y(" << (vertical_f) / (1024) * 100 << "%)" << "\n";



                //Sleep(10);

            } while (1);
        }
        if (input == "B" || input == "b") {
            int count = 0;
            int result = 0;
            do {
                lab3.get_button(27, result);
                if (result == true)
                {

                count++;
                std::cout << "Debounce Test: " << count << "\n";

                }
            }while(1);
        }

        if (input == "D" || input == "d") {
            bool result = 0;

            do {

                lab3.get_data(DIGITAL, Rotary_B, result);//get button state

                lab3.set_data(DIGITAL, LED_G, result); //send button state to led
                std::cout << "DIGITAL TEST: CH32 " << result << "\n";
                cv::waitKey(100);

            } while (1);
        }
        if (input == "S" || input == "s") {
            int position = 0;

            std::cout << "SERVO TEST: CH0 \n (S)et position or (A)uto cycle from max to min:  \n";
            std::getline(std::cin, input);

            if (input == "s" || input == "S") {
                std::cout << "SET\n";

                do {

                    std::cout << "Enter value between 0-180\n";

                    std::getline(std::cin, input);
                    position = stoi(input);


                    lab3.set_data(SERVO, TOP, position);


                    std::cout << "SET POS: " << position << "\n\n";


                } while (1);
            }
            if (input == "a" || input == "A") {

                do {



                    lab3.set_data(SERVO, 6, position);

                    std::cout << "SET POS: " << position << "\n\n";


                    if (position == 0) {
                        position = 180;
                    }
                    else {
                        position = 0;
                    }
                    cv::waitKey(1000);




                } while (1);
            }

        }
    } while (input != "q" || input != "Q");
 }


////////////////////////////////////////////////////////////////
// Lab 4
////////////////////////////////////////////////////////////////
void lab4()
{



    cv::Size size(800, 800);

    CSketch sketch(size, 3);

    sketch.run();


}

////////////////////////////////////////////////////////////////
// Lab 5
////////////////////////////////////////////////////////////////
void lab5()
{

    cv::Size size(1000, 800);

    CPong pong(size, 3);

    pong.run();


}

////////////////////////////////////////////////////////////////
// Lab 6
////////////////////////////////////////////////////////////////
void lab6()
{

    cv::Size size(1200, 800);

    CAsteroidGame Asteroid(size, 3);

    Asteroid.run();


}
std::string gstreamer_pipeline(int capture_width, int capture_height, int framerate, int display_width, int display_height) { //https://github.com/Qengineering/Libcamera-OpenCV-RPi-Bullseye-64OS/blob/main/main.cpp
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

/*
void lab8(){





CControlPi CControl;


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
//vid.set(cv::CAP_PROP_BUFFERSIZE, 3);




cvui::init("Camera");
//cvui::init("HSV Green");
//cvui::init("HSV Pink");
//cvui::init("HSV Red");
cvui::init("Control");
cvui::init("Green");
cvui::init("Pink");
cvui::init("Red");
cvui::init("Blue");
cvui::init("Trackbars");


cv::Scalar green_low;
cv::Scalar green_high;

cv::Scalar pink_low;
cv::Scalar pink_high;

cv::Scalar red_low;
cv::Scalar red_high;

cv::Scalar blue_low;
cv::Scalar blue_high;

int hue_green_low = 44;
int hue_green_high = 87;
int hue_pink_low = 9;
int hue_pink_high = 22;
int hue_red_low = 136;
int hue_red_high = 179;
int hue_blue_low = 99;
int hue_blue_high = 136;

int sat_green_low = 42;
int sat_green_high = 255;
int sat_pink_low = 46;
int sat_pink_high =255;
int sat_red_low =35;
int sat_red_high=255;
int sat_blue_low=131;
int sat_blue_high=255;

int val_green_low=0;
int val_green_high=255;
int val_pink_low=0;
int val_pink_high=255;
int val_red_low=0;
int val_red_high=255;
int val_blue_low=0;
int val_blue_high=255;

int green_max_area = 0;
int blue_max_area=0;
int pink_max_area=0;
int red_max_area=0;

int max1;
int max2;

bool control_flow = false;
bool power_button = false; // false = manual

bool bin1 = false;
bool bin2 = false;
bool bin3 = false;
bool bin4 = false;

bool Rotary_Switch_State = false;
bool Rotary_A_State = false;
bool Rotary_B_State = false;

bool Rotary_Switch_Last_State = false;
bool Rotary_A_Last_State = false;
bool Rotary_B_Last_State = false;

int Rotary_Count = 0;
int ball_count;


if (vid.isOpened() == true){
//cv::waitKey(3);
std::cout << "Vid open";

cv::Mat frame;

cv::Mat green_hsv;
cv::Mat pink_hsv;
cv::Mat red_hsv;
cv::Mat blue_hsv;

cv::Mat green;
cv::Mat pink;
cv::Mat red;
cv::Mat blue;
cv::Mat control;

double start_time_open = cv::getTickCount();
double start_time_close = cv::getTickCount();
double start_time_flow = cv::getTickCount();


double start_time_rotary_switch = cv::getTickCount();




CControl.set_data(SERVO, TOP, 45);
CControl.set_data(SERVO, LEFT, 45);
CControl.set_data(SERVO, RIGHT, 45);
CControl.set_data(SERVO, FLOW, 50);




do{
//reset mat
cv::Mat Trackbar = cv::Mat::zeros(cv::Size(600,600), CV_8UC3);
cv::Mat control = cv::Mat::zeros(cv::Size(600,600), CV_8UC3);


// Button Check

CControl.get_data(DIGITAL, Rotary_A, Rotary_A_State);
CControl.get_data(DIGITAL, Rotary_B, Rotary_B_State);
CControl.get_data(DIGITAL, Rotary_Switch, Rotary_Switch_State);

//Rotary_A_State = !Rotary_A_State;
//Rotary_B_State = !Rotary_B_State;
//Rotary_Switch_State = !Rotary_Switch_State;


if(Rotary_Switch_State == false){

    start_time_rotary_switch = cv::getTickCount();

}

if(Rotary_Switch_State== true){

    if (  (cv::getTickCount() - start_time_rotary_switch) / cv::getTickFrequency() > 5){
    power_button = !power_button;
    start_time_rotary_switch = cv::getTickCount();
    start_time_open = cv::getTickCount() + cv::getTickFrequency() * 5;
    ball_count = 0;
    }

}
if (Rotary_Switch_State == false && Rotary_Switch_Last_State == true){ //switch released

    if(power_button == false){

        switch(Rotary_Count){

        case 1: bin1 = true; break;
        case 2: bin2 = true; break;
        case 3: bin3 = true; break;
        case 4: bin4 = true; break;

        }
    }

}
if (Rotary_A_State == false && Rotary_A_Last_State == true){ //left pressed
Rotary_Count++;
}
if (Rotary_B_State == false && Rotary_B_Last_State == true){ //Right pressed
Rotary_Count--;
}
 //switch released

    if (Rotary_Count > 4){
    Rotary_Count = 1;
    }
    if (Rotary_Count < 1){
    Rotary_Count = 4;
    }



Rotary_A_Last_State = Rotary_A_State;
Rotary_B_Last_State = Rotary_B_State;
Rotary_Switch_Last_State = Rotary_Switch_State;



//GUI control
cvui::context("Control");
cv::Point2i gui = cv::Point2i(10,10);
if (power_button == true){ //if auto mode

    cvui::text(control, gui.x, gui.y,"Auto Mode",0.5,0x00FF00);
    gui += cv::Point2i(0,50);
    if (cvui::button(control, gui.x, gui.y, 100, 100, "Off / Manual Mode")){
    power_button = false;
    start_time_open = cv::getTickCount() + cv::getTickFrequency() * 5;
    ball_count = 0;
    }
    gui += cv::Point2i(100,0);
    cvui::printf(control, gui.x, gui.y, 1.5, 0xFFFFFF, "Ball Counter: %d ", ball_count);


}
else { //manual
    cvui::text(control, gui.x, gui.y,"Manual Mode",0.5,0x00FF00);
    gui += cv::Point2i(0,50);
    if (cvui::button(control, gui.x, gui.y, 100, 100, "Off / Manual Mode")){
    power_button =true;
    }

    gui += cv::Point2i(0,300);
    cvui::printf(control, gui.x, gui.y, 1.5, 0xFFFFFF, "Bin Selected = %d", Rotary_Count);
    gui -= cv::Point2i(0,300);


    gui += cv::Point2i(200,10);
    cvui::text(control, gui.x, gui.y,"Select BIN",0.5,0x00FF00);

    gui += cv::Point2i(0,50);
    if (cvui::button(control, gui.x, gui.y, 60, 30, "BIN 1")){
    bin1 = true;
    }
    if (power_button == false){
        if (Rotary_Count == 1){
        gui += cv::Point2i(100,10);
        cv::circle(control, gui, 20, cv::Scalar(255,255,255), -1);
        gui -= cv::Point2i(100,10);
        CControl.set_data(DIGITAL, LED_G, 1);
        CControl.set_data(DIGITAL, LED_R, 0);
        CControl.set_data(DIGITAL, LED_B, 0);
        }
    }


    gui += cv::Point2i(0,50);
    if (cvui::button(control, gui.x, gui.y, 60, 30, "BIN 2")){
    bin2 = true;
    }
    if (power_button == false){
        if (Rotary_Count == 2){
        gui += cv::Point2i(100,10);
        cv::circle(control, gui, 20, cv::Scalar(255,255,255), -1);
        gui -= cv::Point2i(100,10);
        CControl.set_data(DIGITAL, LED_G, 1);
        CControl.set_data(DIGITAL, LED_R, 1);
        CControl.set_data(DIGITAL, LED_B, 0);
        }
    }




    gui += cv::Point2i(0,50);
     if (cvui::button(control, gui.x, gui.y, 60, 30, "BIN 3")){
    bin3 = true;
    }
    if (power_button == false){
        if (Rotary_Count == 3){
        gui += cv::Point2i(100,10);
        cv::circle(control, gui, 20, cv::Scalar(255,255,255), -1);
        gui -= cv::Point2i(100,10);
        CControl.set_data(DIGITAL, LED_G, 0);
        CControl.set_data(DIGITAL, LED_R, 1);
        CControl.set_data(DIGITAL, LED_B, 0);
        }
    }


    gui += cv::Point2i(0,50);
     if (cvui::button(control, gui.x, gui.y, 60, 30, "BIN 4")){
    bin4 = true;
    }
    if (power_button == false){
        if (Rotary_Count == 4){
        gui += cv::Point2i(100,10);
        cv::circle(control, gui, 20, cv::Scalar(255,255,255), -1);
        gui -= cv::Point2i(100,10);
        CControl.set_data(DIGITAL, LED_G, 0);
        CControl.set_data(DIGITAL, LED_R, 0);
        CControl.set_data(DIGITAL, LED_B, 1);
        }
    }

    }
cvui::update("Control");


 //GUI Trackbar
std::vector<cv::Vec4i> hierarchy_red;
std::vector<cv::Vec4i> hierarchy_green;
std::vector<cv::Vec4i> hierarchy_pink;
std::vector<cv::Vec4i> hierarchy_blue;
std::vector<std::vector<cv::Point>> contours_red;
std::vector<std::vector<cv::Point>> contours_green;
std::vector<std::vector<cv::Point>> contours_pink;
std::vector<std::vector<cv::Point>> contours_blue;
std::vector<cv::Rect> r_green;
std::vector<cv::Rect> r_blue;
std::vector<cv::Rect> r_red;
std::vector<cv::Rect> r_pink;

cvui::context("Trackbars");
 //GREEN
gui = cv::Point2i(10,10);
cvui::text(Trackbar, gui.x, gui.y,"Green Ball",0.5,0x00FF00);
gui += cv::Point2i(0,20);
cvui::text(Trackbar, gui.x, gui.y,"Hue",0.4);
gui += cv::Point2i(0,15);
cvui::text(Trackbar, gui.x, gui.y,"Lower Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &hue_green_low, 0, 179);
gui += cv::Point2i(0,50);
cvui::text(Trackbar, gui.x, gui.y,"Upper Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &hue_green_high, 0, 179);

gui+= cv::Point2i(0,70);

cvui::text(Trackbar, gui.x, gui.y,"Saturation",0.4);
gui += cv::Point2i(0,15);
cvui::text(Trackbar, gui.x, gui.y,"Lower Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &sat_green_low, 0, 255);
gui += cv::Point2i(0,50);
cvui::text(Trackbar, gui.x, gui.y,"Upper Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &sat_green_high, 0, 255);

gui+= cv::Point2i(0,70);

cvui::text(Trackbar, gui.x, gui.y,"Value",0.4);
gui += cv::Point2i(0,15);
cvui::text(Trackbar, gui.x, gui.y,"Lower Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &val_green_low, 0, 255);
gui += cv::Point2i(0,50);
cvui::text(Trackbar, gui.x, gui.y,"Upper Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &val_green_high, 0, 255);


green_low = cv::Scalar(hue_green_low, sat_green_low, val_green_low);
green_high = cv::Scalar(hue_green_high, sat_green_high, val_green_high);


 //BLUE
gui = cv::Point2i(150,10);
cvui::text(Trackbar, gui.x, gui.y,"Blue Ball",0.5,0x0000FF);
gui += cv::Point2i(0,20);
cvui::text(Trackbar, gui.x, gui.y,"Hue",0.4);
gui += cv::Point2i(0,15);
cvui::text(Trackbar, gui.x, gui.y,"Lower Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &hue_blue_low, 0, 179);
gui += cv::Point2i(0,50);
cvui::text(Trackbar, gui.x, gui.y,"Upper Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &hue_blue_high, 0, 179);

gui+= cv::Point2i(0,70);

cvui::text(Trackbar, gui.x, gui.y,"Saturation",0.4);
gui += cv::Point2i(0,15);
cvui::text(Trackbar, gui.x, gui.y,"Lower Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &sat_blue_low, 0, 255);
gui += cv::Point2i(0,50);
cvui::text(Trackbar, gui.x, gui.y,"Upper Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &sat_blue_high, 0, 255);

gui+= cv::Point2i(0,70);

cvui::text(Trackbar, gui.x, gui.y,"Value",0.4);
gui += cv::Point2i(0,15);
cvui::text(Trackbar, gui.x, gui.y,"Lower Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &val_blue_low, 0, 255);
gui += cv::Point2i(0,50);
cvui::text(Trackbar, gui.x, gui.y,"Upper Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &val_blue_high, 0, 255);


blue_low = cv::Scalar(hue_blue_low, sat_blue_low, val_blue_low);
blue_high = cv::Scalar(hue_blue_high, sat_blue_high, val_blue_high);

 //PINK
gui = cv::Point2i(300,10);
cvui::text(Trackbar, gui.x, gui.y,"Pink Ball",0.5,0xFF0F0F);
gui += cv::Point2i(0,20);
cvui::text(Trackbar, gui.x, gui.y,"Hue",0.4);
gui += cv::Point2i(0,15);
cvui::text(Trackbar, gui.x, gui.y,"Lower Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &hue_pink_low, 0, 179);
gui += cv::Point2i(0,50);
cvui::text(Trackbar, gui.x, gui.y,"Upper Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &hue_pink_high, 0, 179);

gui+= cv::Point2i(0,70);

cvui::text(Trackbar, gui.x, gui.y,"Saturation",0.4);
gui += cv::Point2i(0,15);
cvui::text(Trackbar, gui.x, gui.y,"Lower Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &sat_pink_low, 0, 255);
gui += cv::Point2i(0,50);
cvui::text(Trackbar, gui.x, gui.y,"Upper Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &sat_pink_high, 0, 255);

gui+= cv::Point2i(0,70);

cvui::text(Trackbar, gui.x, gui.y,"Value",0.4);
gui += cv::Point2i(0,15);
cvui::text(Trackbar, gui.x, gui.y,"Lower Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &val_pink_low, 0, 255);
gui += cv::Point2i(0,50);
cvui::text(Trackbar, gui.x, gui.y,"Upper Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &val_pink_high, 0, 255);


pink_low = cv::Scalar(hue_pink_low, sat_pink_low, val_pink_low);
pink_high = cv::Scalar(hue_pink_high, sat_pink_high, val_pink_high);

 //RED
gui = cv::Point2i(450,10);
cvui::text(Trackbar, gui.x, gui.y,"Red Ball",0.5,0xFF0000);
gui += cv::Point2i(0,20);
cvui::text(Trackbar, gui.x, gui.y,"Hue",0.4);
gui += cv::Point2i(0,15);
cvui::text(Trackbar, gui.x, gui.y,"Lower Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &hue_red_low, 0, 179);
gui += cv::Point2i(0,50);
cvui::text(Trackbar, gui.x, gui.y,"Upper Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &hue_red_high, 0, 179);

gui+= cv::Point2i(0,70);

cvui::text(Trackbar, gui.x, gui.y,"Saturation",0.4);
gui += cv::Point2i(0,15);
cvui::text(Trackbar, gui.x, gui.y,"Lower Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &sat_red_low, 0, 255);
gui += cv::Point2i(0,50);
cvui::text(Trackbar, gui.x, gui.y,"Upper Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &sat_red_high, 0, 255);

gui+= cv::Point2i(0,70);

cvui::text(Trackbar, gui.x, gui.y,"Value",0.4);
gui += cv::Point2i(0,15);
cvui::text(Trackbar, gui.x, gui.y,"Lower Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &val_red_low, 0, 255);
gui += cv::Point2i(0,50);
cvui::text(Trackbar, gui.x, gui.y,"Upper Bound",0.3);
gui += cv::Point2i(0,10);
cvui::trackbar(Trackbar, gui.x,gui.y,100, &val_red_high, 0, 255);


red_low = cv::Scalar(hue_red_low, sat_red_low, val_red_low);
red_high = cv::Scalar(hue_red_high, sat_red_high, val_red_high);
vid.read(frame);
cvui::update("Trackbars");
if (power_button ==true){

    //cv::waitKey(1);

    cv::cvtColor(frame, green_hsv, cv::COLOR_BGR2HSV);
    cv::cvtColor(frame, pink_hsv, cv::COLOR_BGR2HSV);
    cv::cvtColor(frame, red_hsv, cv::COLOR_BGR2HSV);
    cv::cvtColor(frame, blue_hsv, cv::COLOR_BGR2HSV);




    cv::inRange(green_hsv, green_low, green_high, green);
    cv::inRange(red_hsv, red_low, red_high, red);
    cv::inRange(blue_hsv, blue_low, blue_high, blue);
    cv::inRange(pink_hsv, pink_low, pink_high, pink);

    cv::dilate(green, green, cv::Mat());
    cv::erode(green, green, cv::Mat());


    cv::dilate(red, red, cv::Mat());
    cv::erode(red, red, cv::Mat());



    cv::dilate(blue, blue, cv::Mat());
    cv::erode(blue, blue, cv::Mat());



    cv::dilate(pink, pink, cv::Mat());
    cv::erode(pink, pink, cv::Mat());




    cv::findContours (red, contours_red, hierarchy_red, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    cv::findContours (green, contours_green, hierarchy_green, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    cv::findContours (pink, contours_pink, hierarchy_pink, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    cv::findContours (blue, contours_blue, hierarchy_blue, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    red_max_area =0;

    for (unsigned int i = 0; i < contours_red.size(); i++)
    {
        r_red.push_back(boundingRect(contours_red.at(i)));
        //cv::drawContours(frame, contours_red, i , cv::Scalar(255,255,255), cv::FILLED, 8, hierarchy_red);

        red_max_area += r_red.at(i).area();

    }
    blue_max_area =0;
    for (unsigned int i = 0; i < contours_blue.size(); i++)
    {
        r_blue.push_back(boundingRect(contours_blue.at(i)));
        //cv::drawContours(frame, contours_blue, i , cv::Scalar(255,255,255), cv::FILLED, 8, hierarchy_blue);

     blue_max_area += r_blue.at(i).area();

    }
    pink_max_area = 0;
    for (unsigned int i = 0; i < contours_pink.size(); i++)
    {
        r_pink.push_back(boundingRect(contours_pink.at(i)));
        //cv::drawContours(frame, contours_pink, i , cv::Scalar(255,255,255), cv::FILLED, 8, hierarchy_pink);

        pink_max_area += r_pink.at(i).area();


    }
    green_max_area = 0;
    for (unsigned int i = 0; i < contours_green.size(); i++)
    {
        r_green.push_back(boundingRect(contours_green.at(i)));
        //cv::drawContours(frame, contours_green, i , cv::Scalar(255,255,255), cv::FILLED, 8, hierarchy_green);

       green_max_area += r_green.at(i).area();


    }

    max1 = std::max(green_max_area, blue_max_area);
    max2 = std::max(pink_max_area, red_max_area);

    if (max1 < max2){
    max1 = max2;



    }
}


if ((cv::getTickCount() - start_time_flow) / cv::getTickFrequency() > 0.2){ //center top servo
        CControl.set_data(SEvoid threadQuit(){

    while (1){
    if (cv::waitKey(10) == 'q'){

    gpioTerminate();
    cv::destroyAllWindows();
    }
    }




}RVO, TOP, 95);
    }


if (power_button == true){ //auto mode
    if (max1 < 10000){

    cvui::text(frame, 10,10, "No Balls Detected", 1, 0xFFFFFF);

    }
    else {
    ball_count++;
        if (max1 == green_max_area){

        cvui::text(frame, 10,10, "Detected Green !", 1, 0x00FF00);
        CControl.set_data(SERVO, TOP, 50);
        CControl.set_data(SERVO, LEFT, 60);
        //CControl.set_data(SERVO, RIGHT, 0);
        start_time_flow = cv::getTickCount();
        CControl.set_data(DIGITAL, LED_G, 1);
        CControl.set_data(DIGITAL, LED_R, 0);
        CControl.set_data(DIGITAL, LED_B, 0);

        }
        if (max1 == pink_max_area){
        cvui::text(frame, 10,10, "Detected Yellow !", 1,0xFFF00);
        CControl.set_data(SERVO, TOP, 50);
        CControl.set_data(SERVO, LEFT, 180);
        //CControl.set_data(SERVO, RIGHT, 0);
        start_time_flow = cv::getTickCount();
        CControl.set_data(DIGITAL, LED_G, 1);
        CControl.set_data(DIGITAL, LED_R, 1);
        CControl.set_data(DIGITAL, LED_B, 0);

        }
        if (max1 == red_max_area){
        cvui::text(frame, 10,10, "Detected Red !", 1, 0xFF0000);

        CControl.set_data(SERVO, TOP, 140);
        //CControl.set_data(SERVO, LEFT, 180);
        CControl.set_data(SERVO, RIGHT, 0);
        start_time_flow = cv::getTickCount();
        CControl.set_data(DIGITAL, LED_G, 0);
        CControl.set_data(DIGITAL, LED_R, 1);
        CControl.set_data(DIGITAL, LED_B, 0);

        }
        if (max1 == blue_max_area){
        cvui::text(frame, 10,10, "Detected Purple !", 1, 0xA020F0);
        CControl.set_data(SERVO, TOP, 140);
       // CControl.set_data(SERVO, LEFT, 0);
        CControl.set_data(SERVO, RIGHT, 100);
        start_time_flow = cv::getTickCount();
        CControl.set_data(DIGITAL, LED_G, 0);
        CControl.set_data(DIGITAL, LED_R, 0);
        CControl.set_data(DIGITAL, LED_B, 1);
        }
    }
}

else { //manual mode
    if (bin1 == true){
        CControl.set_data(SERVO, TOP, 50);
        CControl.set_data(SERVO, LEFT, 60);
        //CControl.set_data(SERVO, RIGHT, 0);
        start_time_flow = cv::getTickCount();
        bin1 = false;

        start_time_close = cv::getTickCount();
        CControl.set_data(SERVO, FLOW, 30);
        control_flow = true;

    }
     if (bin2 == true){
        CControl.set_data(SERVO, TOP, 50);
        CControl.set_data(SERVO, LEFT, 180);
        //CControl.set_data(SERVO, RIGHT, 0);
        start_time_flow = cv::getTickCount();
        bin2 = false;

        start_time_close = cv::getTickCount();
        CControl.set_data(SERVO, FLOW, 30);
        control_flow = true;



    }
     if (bin3 == true){
        CControl.set_data(SERVO, TOP, 140);
        //CControl.set_data(SERVO, LEFT, 180);
        CControl.set_data(SERVO, RIGHT, 0);
        start_time_flow = cv::getTickCount();
        bin3 = false;
void threadQuit(){

    while (1){
    if (cv::waitKey(10) == 'q'){

    gpioTerminate();
    cv::destroyAllWindows();
    }
    }




}
        start_time_close = cv::getTickCount();
        CControl.set_data(SERVO, FLOW, 30);
        control_flow = true;

    }
      if (bin4 == true){
        CControl.set_data(SERVO, TOP, 140);
       // CControl.set_data(SERVO, LEFT, 0);
        CControl.set_data(SERVO, RIGHT, 100);
        start_time_flow = cv::getTickCount();
        bin4 = false;

        start_time_close = cv::getTickCount();
        CControl.set_data(SERVO, FLOW, 30);
        control_flow = true;

    }


}


if (power_button == true){
cvui::printf(frame, 10, 50,0.5, 0x0000FF, "B: %d R: %d P: %d G %d", blue_max_area, red_max_area, pink_max_area, green_max_area);




//cv::imshow("HSV Green", green_hsv);
cv::imshow("Green", green);
cv::imshow("Pink", pink);
cv::imshow("Red", red);
cv::imshow("Blue", blue);
}
cv::imshow("Camera", frame);
cv::imshow("Control",control);
cv::imshow("Trackbars",Trackbar);


if (power_button == true && control_flow == false && (cv::getTickCount() - start_time_open) / cv::getTickFrequency() > 0.85){ //OPEN


    start_time_close = cv::getTickCount();
    CControl.set_data(SERVO, FLOW, 30);
    control_flow = true;

}
if (control_flow == true && (cv::getTickCount() - start_time_close) / cv::getTickFrequency() > 0.15){ //CLOSE

    start_time_open = cv::getTickCount();
    CControl.set_data(SERVO, FLOW, 50);
    control_flow = false;

}




}while (cv::waitKey(1) != 'q');
cv::destroyAllWindows();

}

else {


std::cout << "Camera not open";
}


}

*/


void robot(){

CRobot Robot;

Robot.Run();
}

void print_menu()
{
	std::cout << "\n***********************************";
	std::cout << "\n* ELEX Projects";
	std::cout << "\n***********************************";
  std::cout << "\n(6) Lab 6 - Classic Arcade Game";
  std::cout << "\n(7) Lab 7 - Linux Port";
  std::cout << "\n(8) Lab 8 - Sorting";
  std::cout << "\n(9) Lab 9 - Sockets";
    std::cout << "\n(69) Robot Project";
	std::cout << "\n(0) Exit";
	std::cout << "\nCMD> ";
}

int main(int argc, char* argv[])
{

robot();
/*
	int cmd = -1;
	do
	{
		print_menu();
		std::cin >> cmd;
		switch (cmd)
		{
    case 1: lab1(); break;
    case 2: lab2(); break;
    case 3: lab3(); break;
    case 4: lab4(); break;
    case 5: lab5(); break;
    case 6: lab6(); break;
    //case 8: lab8(); break;
    case 69: robot(); break;

		}
	} while (cmd != 0);
	*/
}


