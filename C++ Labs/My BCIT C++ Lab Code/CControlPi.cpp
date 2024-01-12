#include "pigpio.h"
#include <opencv2/opencv.hpp>
#include "CControlPi.h"
#include <thread>
#include <cstdlib>
#include <fstream>
#include <sstream>



enum {DIGITAL = 0, ANALOG, SERVO};
enum {Rotary_Switch = 27, Rotary_A = 22, Rotary_B = 17};

/*
GPIO Numbers

Pot switch =
Green button =
LED =
Servo = 21


Channel Numbers

POT SWITCH = 21
Green Button = 20
LEd = 12
SERVO = 0
horizontal = 0
vertical = 1

Controller
sw = 27
Left 17
right = 22

RGB LED

10
9
11




*/
CControlPi::CControlPi()
{

	for (int i = 0; i < sizeof(debounce)/sizeof(int); i++) {
		debounce[i] = 0;
		debounce_last[i] = 0;
		start_time[i] = cv::getTickCount();
	}
    gpioTerminate();
	gpioInitialise();
	gpioSetMode(27, PI_INPUT); // Control Switch
	gpioSetMode(17, PI_INPUT);// Left
	gpioSetMode(22, PI_INPUT);// Right

    gpioSetPullUpDown(27, PI_PUD_DOWN);
    gpioSetPullUpDown(17, PI_PUD_DOWN);
    gpioSetPullUpDown(22, PI_PUD_DOWN);

	gpioSetMode(21, PI_INPUT); // POT BUTTON
	gpioSetMode(20, PI_INPUT);// GREEN BUTTON
	gpioSetMode(12, PI_OUTPUT);// LED
	gpioSetPullUpDown(12, PI_PUD_DOWN);
	gpioSetPullUpDown(21, PI_PUD_UP);
	gpioSetMode(26, PI_OUTPUT);
	gpioSetMode(19, PI_OUTPUT);
	gpioSetMode(13, PI_OUTPUT);
	gpioSetMode(6, PI_OUTPUT);

	gpioSetMode(10, PI_OUTPUT);
	gpioSetMode(9, PI_OUTPUT);
	gpioSetMode(11, PI_OUTPUT);

}

CControlPi::~CControlPi()
{
gpioTerminate();
}


void CControlPi::init_com(int comport){


	//_com.open("COM"+std::to_string(comport));
	//_com.open("COM3");

	//std::cout << "COM port " + std::to_string(comport) + " Initialized\n";

	return;

}

bool CControlPi::get_data(int type, int channel, int& result) {
result =0;
if ( type == DIGITAL){
    if (channel == 20) // Gren SWITCH
    {
    result = gpioRead(20);
    return 1;
    }
    if (channel == 21){ // Pot button

    result = -1 * gpioRead(21) + 1;
    return 1;
    }

}

if ( type == ANALOG){


if (channel == 0){ //horizontal


unsigned char inBuf[3];
char cmd[] = { 1, 0b10010000, 0};

int handle = spiOpen(0, 200000, 3);
spiXfer(handle,cmd, (char*) inBuf, 3);
result = ((inBuf[1] & 3) << 8) | inBuf[2];
spiClose(handle);
//std::cout<< "horizontal" << result << "\n";

return 1;
}

if (channel == 1){ //vertical


unsigned char inBuf[3];
char cmd[] = { 1, 0b10100000, 0};

int handle = spiOpen(0, 200000, 3);
spiXfer(handle,cmd, (char*) inBuf, 3);
result = ((inBuf[1] & 3) << 8) | inBuf[2];
spiClose(handle);
//std::cout<< "vertical" << result << "\n";
return 1;
}


}
return 0;
}


bool CControlPi::set_data(int type, int channel, int val) {

if (type == DIGITAL){

gpioWrite(channel, val);

}

if (type == SERVO){

    if (channel == 26){ // TOP

    val = val*((1800-500)/180) + 600;

    gpioServo(26, val);

    }
     if (channel == 19){ //  RIGHT

    val = val*((2100-800)/180) + 800;

    gpioServo(19, val);

    }
     if (channel == 13){ //  LEFT

    val = val*((2100-500)/180) + 500;

    gpioServo(13, val);

    }
     if (channel == 6){ //CONTROL FLOW


    val = val*((2500-800)/180) + 800;
    gpioServo(6, val);

    }
}
}

bool CControlPi::get_analog(int channel, int& result)
{
	if (get_data(ANALOG, channel, result) == true)
	{
		return 1;
	}
	else
	{
		return 0;
	}


}

bool CControlPi::get_button(int channel, int& result)
{

	if (get_data(DIGITAL, channel, debounce[channel]))
	{

		if (debounce_last[channel] == 0 && debounce[channel] == 1 && (cv::getTickCount() - start_time[channel]) / cv::getTickFrequency() > 0.2)
		{
			start_time[channel] = cv::getTickCount();
			result = 1;
		}
		else
		{
			result = 0;
		}

		debounce_last[channel] = debounce[channel];
		return 1;
	}
	else
	{
        std::cout<< "Error getbutton";
		return 0;
	}
}

bool CControlPi::get_button(int channel, bool& result)
{

	if (get_data(DIGITAL, channel, debounce[channel]))
	{

		if (debounce_last[channel] == 0 && debounce[channel] == 1 && (cv::getTickCount() - start_time[channel]) / cv::getTickFrequency() > 0.2)
		{
			start_time[channel] = cv::getTickCount();
			result = 1;
		}
		else
		{
			result = 0;
		}

		debounce_last[channel] = debounce[channel];
		return 1;
	}
	else
	{
        std::cout<< "Error getbutton";
		return 0;
	}
}

bool CControlPi::get_data(int type, int channel, bool& result) {
result =0;
if ( type == DIGITAL){

    result = gpioRead(channel);
    return 1;
}
return 0;
}
