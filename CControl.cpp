#include "stdafx.h"
#include "CControl.h"

enum {DIGITAL = 0, ANALOG, SERVO};



CControl::CControl()
{
	
	for (int i = 0; i < sizeof(debounce)/sizeof(int); i++) {
		debounce[i] = 0;
		debounce_last[i] = 0;
		start_time[i] = cv::getTickCount();
	}
}

CControl::~CControl()
{
}

void CControl::init_com(int comport){
	

	//_com.open("COM"+std::to_string(comport));
	_com.open("COM4");

	std::cout << "COM port " + std::to_string(comport) + " Initialized\n";
	
    
	return;

}

bool CControl::get_data(int type, int channel, int& result) {
	
	//_com.open("COM3");
	
	if (_com.is_open() == false) {
		std::cout << "COM Closed";
		return 0;
	}
	std::string tx_str = "";
	std::string rx_str;
	char buff[2];
	
	tx_str = "G " + std::to_string(type) + " " + std::to_string(channel) + "\n";


	_com.flush();
		// Send TX string
		_com.write(tx_str.c_str(), tx_str.length());
		
		Sleep(10); // wait for ADC conversion, etc. May not be needed?

		rx_str = "";
		// start timeout count
		double start_time = cv::getTickCount();

		buff[0] = 0;
		
		// Read 1 byte and if an End Of Line then exit loop
	// Timeout after 1 second, if debugging step by step this will cause you to exit the loop
		while (buff[0] != '\n' && (cv::getTickCount() - start_time) / cv::getTickFrequency() < 1.0)
		{
			if (_com.read(buff, 1) > 0)
			{
				rx_str = rx_str + buff[0];
			}
		}
		
			
			if (rx_str.length() > 0) {
			
				std::size_t found = rx_str.find_last_of(" ");
				rx_str = rx_str.substr(found);
				result = std::stoi(rx_str);

				result = std::stoi(rx_str.substr(rx_str.find_last_of(" ")));
				return 1;
			}
			else {
				return 0;
			}
		}


bool CControl::set_data(int type, int channel, int val) {
	std::string tx_str = "";
		tx_str = "S " + std::to_string(type) + " " + std::to_string(channel) + " " + std::to_string(val) + "\n";

		
		if (_com.is_open() == true) {
			_com.write(tx_str.c_str(), tx_str.length());
			_com.flush();
			return 1;
	}
		else
		{
			std::cout << "COM port closed";
			return 0;
		}
	
}

bool CControl::get_analog(int channel, int& result)
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

bool CControl::get_button(int channel, int& result)
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
		
		return 0;
	}
}
