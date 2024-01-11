////////////////////////////////////////////////////////////////
// ELEX 4618 Template project for BCIT
// Created Oct 5, 2016 by Craig Hennessey
// Last updated Dec 2, 2022
////////////////////////////////////////////////////////////////

#include "stdafx.h"


// Add simple GUI elements
#define CVUI_DISABLE_COMPILATION_NOTICES
#define CVUI_IMPLEMENTATION
#include "cvui.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>



#include "Client.h"
#include "Server.h"
#include "CControl.h"
#include "CBase4618.h"
#include "CSketch.h"
#include "CPong.h"
#include "CAsteroidGame.h"

// Must include Windows.h after Winsock2.h, so Serial must be included after Client/Server
#include "Serial.h" 

#define CANVAS_NAME "Display Image"

enum { DIGITAL = 0, ANALOG, SERVO };

std::string server_ip = "192.168.137.166";   // Server IP address  (You can change this by changing this to the IP address of your Raspberry Pi)
int server_port = 4618;                   // Server Port Number (Same deal as above but the default address is pretty good already)

////////////////////////////////////////////////////////////////
// Can be used as a replacement for cv::waitKey() to display cv::imshow() images, Windows Only
////////////////////////////////////////////////////////////////
void process_msg()
{
  MSG msg;
  while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
  {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }
}

////////////////////////////////////////////////////////////////
// Generate ARUCO markers
////////////////////////////////////////////////////////////////
void generate_marks()
{
  std::string str;
  cv::Mat im;
  int mark_size = 250;

  cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

  for (int marker_id = 0; marker_id < 250; marker_id++)
  {
    cv::aruco::drawMarker(dictionary, marker_id, mark_size, im, 1);
    str = "mark" + std::to_string(marker_id) + ".png";
    cv::imwrite(str, im);
  }
}

////////////////////////////////////////////////////////////////
// Serial Communication
////////////////////////////////////////////////////////////////
void test_com()
{
  // Comport class (change port to your MSP device port)
  Serial com;
  com.open("COM3");

  // TX and RX strings
  std::string tx_str = "G 1 11\n";
  std::string rx_str;

  // temporary storage
  char buff[2];
  do
  {
    // Send TX string
		com.write(tx_str.c_str(), tx_str.length());
    Sleep(10); // wait for ADC conversion, etc. May not be needed?
  
    rx_str = "";
    // start timeout count
    double start_time = cv::getTickCount();

    buff[0] = 0;
		// Read 1 byte and if an End Of Line then exit loop
    // Timeout after 1 second, if debugging step by step this will cause you to exit the loop
    while (buff[0] != '\n' && (cv::getTickCount() - start_time) / cv::getTickFrequency() < 1.0)
    {
      if (com.read(buff, 1) > 0)
      {
        rx_str = rx_str + buff[0];
      }
    }

    printf ("\nRX: %s", rx_str.c_str());
    cv::waitKey(1);
  } 
  while (1);
}

////////////////////////////////////////////////////////////////
// Display Image on screen
////////////////////////////////////////////////////////////////
void do_image()
{
  cv::Mat im;

  // initialize GUI system
  cvui::init(CANVAS_NAME);
  cv::Point gui_position;

  // Load test image
  im = cv::imread("BCIT.jpg");

  // Seed random number generator with time
  srand(time(0));

  // Draw 500 circles and dots on the image
  for (int i = 0; i < 500; i++)
  {
    gui_position = cv::Point(10, 10);
    cvui::window(im, gui_position.x, gui_position.y, 200, 40, "Image Test");
    gui_position += cv::Point(5, 25);
    cvui::text(im, gui_position.x, gui_position.y, "Number of Circles: " + std::to_string(i));

    float radius = 50 * rand() / RAND_MAX;
    cv::Point center = cv::Point(im.size().width*rand() / RAND_MAX, im.size().height*rand() / RAND_MAX);
    
    cv::circle(im, center, radius, cv::Scalar(200, 200, 200), 1, cv::LINE_AA);
    
    im.at<char>(i,i) = 255;
    
    // Show image and delay (all display image to update)
    cv::imshow(CANVAS_NAME, im);
    cv::waitKey(1);
  }
}

////////////////////////////////////////////////////////////////
// Display Video on screen
////////////////////////////////////////////////////////////////
void do_video()
{
  cv::VideoCapture vid;

  vid.open(0, cv::CAP_DSHOW);
    
  bool do_canny = true;
  bool do_aruco = false;
  int canny_thresh = 30;
  bool do_exit = false;

  cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

  // initialize GUI system
  cvui::init(CANVAS_NAME);
  cv::Point gui_position;
  
  std::vector<cv::Scalar> color_vec;
  color_vec.push_back(cv::Scalar(255, 255, 255));
  color_vec.push_back(cv::Scalar(255, 0, 0));
  color_vec.push_back(cv::Scalar(0, 255, 0));
  color_vec.push_back(cv::Scalar(0, 0, 255));
  int color_index = 0;
  
  if (vid.isOpened() == TRUE)
  {
    do
    {
      cv::Mat frame, edges;

      // Capture video frame
      vid >> frame;

      // Make sure video frame exists
      if (frame.empty() == false)
      { 

        // ARUCO marker tracking
        if (do_aruco == true)
        {
          std::vector<int> ids;
          std::vector<std::vector<cv::Point2f> > corners;
          cv::aruco::detectMarkers(frame, dictionary, corners, ids);
          if (ids.size() > 0)
          {
            cv::aruco::drawDetectedMarkers(frame, corners, ids);
          }
        }

        // Canny edge detection
        if (do_canny == true)
        {
          cv::cvtColor(frame, edges, cv::COLOR_BGR2GRAY);
          cv::GaussianBlur(edges, edges, cv::Size(7, 7), 1.5, 1.5);
          cv::Canny(edges, edges, 0, canny_thresh, 3);
          cv::add(frame, color_vec.at(color_index), frame, edges);
        }

        // GUI Menu
        gui_position = cv::Point(10, 10);
        cvui::window(frame, gui_position.x, gui_position.y, 200, 190, "Video Test");
        gui_position += cv::Point(5, 25);
        cvui::checkbox(frame, gui_position.x, gui_position.y, "Canny Filter", &do_canny);
        gui_position += cv::Point(0, 25);
        cvui::checkbox(frame, gui_position.x, gui_position.y, "ArUco", &do_aruco);
        gui_position += cv::Point(0, 25);
        cvui::text(frame, gui_position.x, gui_position.y, "Canny Threshold");
        gui_position += cv::Point(0, 15);
        cvui::trackbar(frame, gui_position.x, gui_position.y, 180, &canny_thresh, 5, 120);
        gui_position += cv::Point(0, 50);
        if (cvui::button(frame, gui_position.x, gui_position.y, 100, 30, "Colour Switch"))
        {
          color_index++;
          if (color_index >= color_vec.size()) { color_index = 0; }
        }
        gui_position += cv::Point(120, 0);
        if (cvui::button(frame, gui_position.x, gui_position.y, 50, 30, "Exit"))
        {
          do_exit = true;
        }

        cvui::update();
        cv::imshow(CANVAS_NAME, frame);
      }
    }
    while (cv::waitKey(1) != 'q' && do_exit == false);
  }      
}		

////////////////////////////////////////////////////////////////
// Demo client server communication
////////////////////////////////////////////////////////////////
void serverthread(CServer* server)
{
  // Start server
  server->start(4618);
}

void do_clientserver()
{
  char inputchar = 0;
  std::vector<std::string> cmds;

  cv::VideoCapture vid;
  CServer server;

  // Start server thread
  std::thread t(&serverthread, &server);
  t.detach();

  vid.open(0);

  while (inputchar != 'q')
  {
    inputchar = cv::waitKey(100);

    server.get_cmd(cmds);
    if (cmds.size() > 0)
    {
      // Process different commands received by the server
      for (int i = 0; i < cmds.size(); i++)
      {
        if (cmds.at(i) == "im")
        {
          std::cout << "\nServer Rx: " << cmds.at(i);

          std::string reply = "Hi there from Server";
          server.send_string(reply);
        }
        else
        {
          std::cout << "\nServer Rx: " << cmds.at(i);

          std::string reply = "Got some other message";
          server.send_string(reply);
        }
      }
    }

		// Update server image with the latest camera image
		if (vid.isOpened() == true)
		{
			cv::Mat frame;
			vid >> frame;
			if (frame.empty() == false)
			{
				imshow("Server Image", frame);
				process_msg();
				server.set_txim(frame);
			}
		}
  };

  server.stop();
  
  Sleep(100);
}

////////////////////////////////////////////////////////////////
// Lab 1
////////////////////////////////////////////////////////////////
void lab1()
{
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
    Serial _com;
    
    CControl lab3;

    lab3.init_com(4); //COM PORT
      

    std::string input;
    


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

                lab3.get_analog(0, horizontal); //x
                lab3.get_analog(1, vertical); //y

                horizontal_f = horizontal;

                vertical_f = vertical;

                std::cout << std::setprecision(4) << "ANALOG TEST: CH0 = " << horizontal << "x("<< (horizontal_f ) / (4096) * 100 << "%)" << "CH1 = " << vertical << "y(" << (vertical_f) / (4096) * 100 << "%)" << "\n";

                

                Sleep(10);

            } while (1);
        }
        if (input == "B" || input == "b") {
            int count = 0;
            int result = 0;
            do {
                lab3.get_button(32, result);
                if (result == true)
                {

                count++;
                std::cout << "Debounce Test: " << count << "\n";
             
                }
            }while(1);
        }
        
        if (input == "D" || input == "d") {
            int result = -1;
            
            do {
                
                lab3.get_data(DIGITAL, 32, result);//get button state
                
                lab3.set_data(DIGITAL, 38, result); //send button state to led
                std::cout << "DIGITAL TEST: CH32 " << result << "\n";
                Sleep(100);
                
                
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

                    if (position > 180) {
                        position = 180;
                    }
                    if (position < 0) {

                        position = 0;
                    }

                    lab3.set_data(SERVO, 0, position);


                    std::cout << "SET POS: " << position << "\n\n";


                } while (1);
            }
            if (input == "a" || input == "A") {

                do {



                    lab3.set_data(SERVO, 0, position);

                    std::cout << "SET POS: " << position << "\n\n";


                    if (position == 0) {
                        position = 180;
                    }
                    else {
                        position = 0;
                    }
                    Sleep(1000);




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


////////////////////////////////////////////////////////////////
// ELEX 4699 Robot Client
////////////////////////////////////////////////////////////////

std::atomic<int> cmd(-1);
std::atomic<int> msg(-1);
std::mutex wifi;
/*
void keypress_handler()
{
   while (cmd != 27) // 27 represents the ESC key
   {
   }
}
*/
float timeout_start;
void send_command(CClient&, std::string);
std::chrono::milliseconds duration(200);
cv::Mat im;
void send_command(CClient& client, std::string cmd)
{
    std::string str;

    
    client.tx_str(cmd);
    std::cout << "\nClient Tx: " << cmd;

    if (cmd == "im")
    {
        
        if (client.rx_im(im) == true)
        {
            timeout_start = cv::getTickCount();
            if (im.empty() == false)
            {
                std::cout << "\nClient Rx: Image received";
                
                
                

           
            }
        }
        else
        {
            std::cout << "Failed to receive image from server" << std::endl;
            if ((cv::getTickCount() - timeout_start) / cv::getTickFrequency() > 1000)
            {
                // No response, disconnect and reconnect
                timeout_start = cv::getTickCount();
                client.close_socket();
                client.connect_socket(server_ip, server_port);
            }
        }
    }
    else
    {

        if (client.rx_str(str) == true)
        {
            timeout_start = cv::getTickCount();
            std::cout << "\nClient Rx: " << str;
        }
        else
        {
            if ((cv::getTickCount() - timeout_start) / cv::getTickFrequency() > 1000)
            {
                // No response, disconnect and reconnect
                timeout_start = cv::getTickCount();
                client.close_socket();
                client.connect_socket(server_ip, server_port);
            }
        }
    }
}

void video_command_handler(CClient& client)
{
    while (msg != 27)
    {
        //wifi.lock();
        send_command(client, "im");
        //wifi.unlock();
        
        std::this_thread::sleep_for(duration);
    }
}



void print_menu_client_server()
{
    std::cout << "\n***********************************";
    std::cout << "\n* ELEX4618 Client Project";
    std::cout << "\n***********************************";
    std::cout << "\n(1) Send image command";
    std::cout << "\n(2) Send other command";
    std::cout << "\n(0) Exit";
    std::cout << "\nCMD> ";
}


void CRobot_Client()
{
    std::string str;
    CClient clientvid_obj;
    CClient clientcmd_obj;
   // CControl ccontrol_obj;

    cvui::init("rx");
    int horizontal;
    int vertical;

    clientvid_obj.connect_socket(server_ip, server_port);
    clientcmd_obj.connect_socket(server_ip, (server_port + 1));

    std::cout << "\n|| MANUAL MODE VEHICLE COMMANDS ||";

    std::cout << "\n Press (w) to move forward    ";
    std::cout << "\n Press (a) to turn left      ";
    std::cout << "\n Press (s) to reverse        ";
    std::cout << "\n Press (d) to turn right   \n";

    // Create separate threads to handle keypress events and video stream
    //std::thread keypress_thread(keypress_handler);
    std::thread video_command_thread(video_command_handler, std::ref(clientvid_obj));

    //keypress_thread.detach();
    video_command_thread.detach();
   
    do
    {
        
        //ccontrol_obj.get_analog(0, horizontal);
        //ccontrol_obj.get_analog(1, vertical);
        msg = cv::waitKey(100);
 
        //wifi.lock();
        if (im.empty() == false) {

            cv::imshow("rx", im);
            cvui::update();
            cv::waitKey(1);


        }

        if (msg == 'w') {
            send_command(clientcmd_obj, "Forward\n");
        }

        if (msg == 'a') {
            send_command(clientcmd_obj, "Left\n");
        }

        if (msg == 's') {
            send_command(clientcmd_obj, "Reverse\n");
        }

        if (msg == 'd') {
            send_command(clientcmd_obj, "Right\n");
        }

        if (msg == '+') {
           send_command(clientcmd_obj, "Increase\n");
        }
        if (msg == '-') {
           send_command(clientcmd_obj, "Decrease\n");
        }
        if (msg == 'o') {
           send_command(clientcmd_obj, "up_servo\n");
        }
        if (msg == 'p') {
           send_command(clientcmd_obj, "down_servo\n");
        }
        if (msg == '1') {
           send_command(clientcmd_obj, "1\n");
        }
        if (msg == '2') {
           send_command(clientcmd_obj, "2\n");
        }
        if (msg == '3') {
           send_command(clientcmd_obj, "3\n");
        }
        if (msg == '4') {
           send_command(clientcmd_obj, "4\n");
        }
        if (msg == '5') {
           send_command(clientcmd_obj, "5\n");
        }
        if (msg == '6') {
           send_command(clientcmd_obj, "6\n");
        }
        if (msg == 'q') {
           send_command(clientcmd_obj, "q\n");
        }
        if (msg == 'k') {
           send_command(clientcmd_obj, "k\n");
        }
        if (msg == 'l') {
           send_command(clientcmd_obj, "l\n");
        }
        if (msg == 'r') {
           send_command(clientcmd_obj, "l\n");
        }
        if (msg == 't') {
           send_command(clientcmd_obj, "t\n");
        }
        //wifi.unlock();
    } while (msg != 27);
    
    

    // Join the keypress and video command threads to ensure they exit before the main thread
    //if (keypress_thread.joinable()) {
       //keypress_thread.join();
   // }
   // 
   // if (video_command_thread.joinable()) {
     //  video_command_thread.join();
    //}
}


void print_menu()
{
    std::cout << "\n***********************************";
    std::cout << "\n* ELEX4618 Template Project";
    std::cout << "\n***********************************";
    std::cout << "\n(1) Lab 1 - User Input";
    std::cout << "\n(2) Lab 2 - Grading";
    std::cout << "\n(3) Lab 3 - Embedded Control";
    std::cout << "\n(4) Lab 4 - Etch-A-Sketch";
    std::cout << "\n(5) Lab 5 - Pong";
    std::cout << "\n(6) Lab 6 - Classic Arcade Game";
    std::cout << "\n(7) Lab 7 - Linux Port";
    std::cout << "\n(8) Lab 8 - Sorting";
    std::cout << "\n(9) Lab 9 - Sockets";
    std::cout << "\n(10) Test serial COM communication";
    std::cout << "\n(11) Show image manipulation";
    std::cout << "\n(12) Show video manipulation";
    std::cout << "\n(13) Test client/server communication";
    std::cout << "\n(14) 4699 Robot Client";
    std::cout << "\n(0) Exit";
    std::cout << "\nCMD> ";
}

int main(int argc, char* argv[])
{
  generate_marks();
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
    case 10: test_com(); break;
		case 11: do_image(); break;
		case 12: do_video(); break;
    case 13: do_clientserver(); break;
    case 14: CRobot_Client(); break;
    
		}
	} while (cmd != 0);
}
