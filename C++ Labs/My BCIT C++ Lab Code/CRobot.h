#pragma once


class CRobot {

private:


cv::Size size;
cv::Mat frame;
cv::VideoCapture vid;

int count;

bool _thread_exit = 0;

public:


    CRobot();
    ~CRobot();

    void Run();
    void Update();
    void Draw();

    static void wheel_thread_FL(CRobot* ptr); //front left
    static void wheel_thread_FR(CRobot* ptr);
    static void wheel_thread_BL(CRobot* ptr);
    static void wheel_thread_BR(CRobot* ptr); //back right

    void wheel_FL();
    void wheel_FR();
    void wheel_BL();
    void wheel_BR();

    std::string gstreamer_pipeline(int capture_width, int capture_height, int framerate, int display_width, int display_height);



};
