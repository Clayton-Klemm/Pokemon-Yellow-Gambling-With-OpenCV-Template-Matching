#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <windows.h>
#include <map>
#include <filesystem>
//#include <conio.h> // Include for _kbhit()
#include <cstdlib> // For system()

// Function for simulating a key press in Windows.
void SimulateKeyPress(char key) {
	INPUT input[2]; // INPUT is a struct used by SendInput to store information 
                    // for synthesizing input events (keystrokes, mouse movement, etc.)
	
	// Set up the INPUT for key down
	input[0].type = INPUT_KEYBOARD;
	input[0].ki.wVk = VkKeyScan(key); // Retrieves the virtual-key code and OEM scan code for a given key.
	input[0].ki.wScan = MapVirtualKey(input[0].ki.wVk, 0); // Translates virtual-key code into a scan code.
	input[0].ki.dwFlags = 0; // Indicates a key down event.
	input[0].ki.time = 0;
	input[0].ki.dwExtraInfo = 0;
	// Set up the INPUT for key up
	input[1].type = INPUT_KEYBOARD;
	input[1].ki.wVk = VkKeyScan(key);
	input[1].ki.wScan = MapVirtualKey(input[1].ki.wVk, 0);
	input[1].ki.dwFlags = KEYEVENTF_KEYUP; // Key up
	input[1].ki.time = 0;
	input[1].ki.dwExtraInfo = 0;
	// Send the input
	SendInput(1, &input[0], sizeof(INPUT)); // Key down
	Sleep(50); // Introduce a short delay to simulate the duration of a key press. In this case 50 milliseconds.
	SendInput(1, &input[1], sizeof(INPUT)); // Key up
}

// Function to capture a screenshot of a specified region.
// Parameters:
// - region: The rectangle defining the region to capture.
// - hwnd: Handle to the window we are capturing from.
cv::Mat capture_region(const cv::Rect& region, HWND hwnd) {
	int width = region.width;
	int height = region.height;
	HDC hdcWindow = GetDC(hwnd);
	HDC hdcMemDC = CreateCompatibleDC(hdcWindow);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, width, height);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMemDC, hBitmap);
	BitBlt(hdcMemDC, 0, 0, width, height, hdcWindow, region.x, region.y, SRCCOPY);
	SelectObject(hdcMemDC, hOldBitmap);
	BITMAPINFOHEADER bi = { 0 };
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height; // Negative indicates a top-down DIB
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	cv::Mat mat(height, width, CV_8UC4);
	GetDIBits(hdcMemDC, hBitmap, 0, height, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
	// Convert to grayscale
	cv::Mat matGray;
	cv::cvtColor(mat, matGray, cv::COLOR_BGR2GRAY);
	DeleteObject(hBitmap);
	DeleteDC(hdcMemDC);
	ReleaseDC(hwnd, hdcWindow);
	return matGray; // Return the grayscale image
}

bool check_template(const cv::Rect roi_search, const cv::Mat autoCropBar, double precision,
	cv::Mat &last_captured,HWND hwnd) {
	cv::Mat captured = capture_region(roi_search, hwnd);
	last_captured = captured;
	cv::Mat result;

	cv::matchTemplate(captured, autoCropBar, result, cv::TM_SQDIFF_NORMED);
	double minVal;
	cv::minMaxLoc(result, &minVal, nullptr, nullptr, nullptr);
	return minVal < precision;
}

int main()
{
	const char* windowTitle = "RetroArch SameBoy 0.15.4 0913833";
	HWND hwnd = FindWindowA(NULL, windowTitle);
	// Ensure the target window is found
	if (hwnd == NULL) {
		std::cout << "Window not found." << std::endl;
		return -1;
	}
	// Obtain the window's client area dimensions
	RECT windowRect;
	GetClientRect(hwnd, &windowRect);
	int width = windowRect.right - windowRect.left;
	int height = windowRect.bottom - windowRect.top;

	cv::Mat screenshot = capture_region(cv::Rect(0, 0, width, height), hwnd);



	/////////////////////////////////////// fine tuning regions ////////////////////////////////////////
	//cv::Rect roi_temp = cv::selectROI("select region",screenshot);
	//cv::destroyAllWindows();

	//// save our selected region and print the coordinates out, are the coordinates window relative?
	//std::cout << "Region of window: " << std::endl;
	//std::cout << "X: " << windowRect.left << std::endl;
	//std::cout << "Y: " << windowRect.top << std::endl;
	//std::cout << "Width: " << windowRect.right << std::endl;
	//std::cout << "Height: " << windowRect.bottom << std::endl;

	//std::cout << "Region of fine tune: " << std::endl;
	//std::cout << "X: " << roi_temp.x << std::endl;
	//std::cout << "Y: " << roi_temp.y << std::endl;
	//std::cout << "Width: " << roi_temp.width << std::endl;
	//std::cout << "Height: " << roi_temp.height << std::endl;
	////////////////////////////////////////////////////////////////////////////////////////////////////


	// create a directory to store our images to disk for debugging purposes
	try {
		// Create directory if it doesn't exist
		std::filesystem::path dir("roi_templates");
		if (!std::filesystem::exists(dir)) {
			std::filesystem::create_directory(dir);
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error in detecting or creating folder" << e.what() << std::endl;
	}
	
	// our calculated auto crop for bar 1
	int calc_x_bar1 = .2458 * windowRect.right; // this hardcoded value was tediously found via selecting region
										   // and experimentation. Current strategy selecting middle of bars
	int calc_y = .3333 * windowRect.bottom;
	int calc_width = .1094 * windowRect.right;
	int calc_height = .1169 * windowRect.bottom;
	cv::Mat autoCropBar1 = screenshot(cv::Rect(calc_x_bar1, calc_y, calc_width, calc_height));
	cv::imwrite("roi_templates/auto_crop_bar1.png", autoCropBar1);


	// our calculated auto crop for bar 2
	int calc_x_bar2 = .4479 * windowRect.right;
	cv::Mat autoCropBar2 = screenshot(cv::Rect(calc_x_bar2, calc_y, calc_width, calc_height));
	cv::imwrite("roi_templates/auto_crop_bar2.png", autoCropBar2);

	// our calculated auto crop for bar 3
	int calc_x_bar3 = .6479 * windowRect.right;
	cv::Mat autoCropBar3 = screenshot(cv::Rect(calc_x_bar3, calc_y, calc_width, calc_height));
	cv::imwrite("roi_templates/auto_crop_bar3.png", autoCropBar3);


	// make sure window is active
	SetForegroundWindow(hwnd);
	Sleep(1000);
	SimulateKeyPress('x');

	// rect of our template
	cv::Rect bar1_middle_rect = cv::Rect(calc_x_bar1, calc_y, calc_width, calc_height);
	cv::Rect bar2_middle_rect = cv::Rect(calc_x_bar2, calc_y, calc_width, calc_height);
	cv::Rect bar3_middle_rect = cv::Rect(calc_x_bar3, calc_y, calc_width, calc_height);


	
	// so we can debug what the last thing that has been seen after a match
	cv::Mat lastCaptured;
	Sleep(2000);
	bool roller1 = false;
	while (!roller1) {
		roller1 = check_template(bar1_middle_rect, autoCropBar1, .125, lastCaptured, hwnd);
		if (roller1) {
			// try and get 7 in bottom position.
			Sleep(50);
			SimulateKeyPress('x');
			cv::imwrite("roi_templates/matched_bar1.png", lastCaptured);
		}
	}
	Sleep(1000);
	bool roller2 = false;
	while (!roller2) {
		roller2 = check_template(bar2_middle_rect, autoCropBar2, .05, lastCaptured, hwnd);
		if (roller2) {
			Sleep(50);
			SimulateKeyPress('x');
			cv::imwrite("roi_templates/matched_bar2.png", lastCaptured);
		}
	}
	Sleep(1000);
	bool roller3 = false;
	while (!roller3) {
		roller3 = check_template(bar3_middle_rect, autoCropBar3, .1, lastCaptured, hwnd);
		if (roller3) {
			//Sleep(10);
			SimulateKeyPress('x');
			cv::imwrite("roi_templates/matched_bar3.png", lastCaptured);
		}
	}


	//system("pause");
	return 0;
}
