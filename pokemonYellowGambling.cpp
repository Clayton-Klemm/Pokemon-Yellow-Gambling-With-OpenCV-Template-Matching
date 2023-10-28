#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <windows.h>
#include <map>
#include <filesystem>

// state definition
enum States {
	START = 0,
	ALL_MOVING = 1,
	FIRST_STOPPED = 2,
	SECOND_STOPPED = 3,
	ALL_STOPPED = 4
};


// to store our templates (the bar regions)
std::map<std::string, cv::Mat> templates;

// function for simulating a key press in windows
void SimulateKeyPress(char key) {
	INPUT input[2]; // INPUT is a type of struct used by SendInput to store information 
	// for synthesizing input events such as keystrokes, mouse movement, and mouse clicks
	// MOUSEINPUT mi, KEYBDINPUT ki, HARDWAREINPUT hi, 

	// Set up the INPUT for key down
	input[0].type = INPUT_KEYBOARD;
	input[0].ki.wVk = VkKeyScan(key); // Virtual key code
	input[0].ki.wScan = MapVirtualKey(input[0].ki.wVk, 0);
	input[0].ki.dwFlags = 0; // Key down
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
	Sleep(50); // Sleep for 50 milliseconds
	SendInput(1, &input[1], sizeof(INPUT)); // Key up
}

// function for capturing a screenshot of our region of interest
cv::Mat capture_region(const cv::Rect& region, HWND hwnd) {
	// with and height of the capture region
	int width = region.width;
	int height = region.height;
	cv::Mat screenshot(height, width, CV_8UC4);
	HDC hdcWindow = GetDC(hwnd);
	if (!hdcWindow) {
		std::cerr << "Failed to get device context!" << std::endl;
		return screenshot; // Return empty screenshot
	}
	HDC hdcMemory = CreateCompatibleDC(hdcWindow);
	if (!hdcMemory) {
		std::cerr << "Failed to create compatible device context!" << std::endl;
		ReleaseDC(hwnd, hdcWindow);
		return screenshot; // Return empty screenshot
	}
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, width, height);
	if (!hBitmap) {
		std::cerr << "Failed to create compatible bitmap!" << std::endl;
		DeleteDC(hdcMemory);
		ReleaseDC(hwnd, hdcWindow);
		return screenshot; // Return empty screenshot
	}
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmap);
	if (!hOldBitmap) {
		std::cerr << "Failed to select object into memory device context!" << std::endl;
		DeleteObject(hBitmap);
		DeleteDC(hdcMemory);
		ReleaseDC(hwnd, hdcWindow);
		return screenshot; // Return empty screenshot
	}
	BitBlt(hdcMemory, 0, 0, width, height, hdcWindow, region.x, region.y, SRCCOPY);
	BITMAPINFOHEADER bi = { 0 };
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	GetDIBits(hdcMemory, hBitmap, 0, height, screenshot.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
	// Clean up resources here
	SelectObject(hdcMemory, hOldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hdcMemory);
	ReleaseDC(hwnd, hdcWindow);
	return screenshot;
}
struct RegionData {
	cv::Rect roi;
	cv::Mat templateImg;
};


// function for checking if the template is matching
bool check_template(const RegionData& regionData, HWND hwnd) {
	cv::Mat captured = capture_region(regionData.roi, hwnd);
	cv::Mat result;
	cv::matchTemplate(captured, regionData.templateImg, result, cv::TM_SQDIFF_NORMED);
	double minVal;
	cv::minMaxLoc(result, &minVal, nullptr, nullptr, nullptr);
	return minVal < .1;
}

// Modified check_state function to use the RegionData
States check_state(States current_state, const std::map<std::string, RegionData>& regions, HWND hwnd) {
	switch (current_state) {
	case START:
		Sleep(8000);
		return ALL_MOVING;
	case ALL_MOVING:
		if (check_template(regions.at("bar1"), hwnd)) {
			std::cout << "Match for the first roller!" << std::endl;
			SimulateKeyPress('x');
			Sleep(1000);
			return FIRST_STOPPED;
		}
		break;
	case FIRST_STOPPED:
		if (check_template(regions.at("bar2"), hwnd)) {
			std::cout << "Match for the second roller!" << std::endl;
			SimulateKeyPress('x');
			Sleep(1000);
			return SECOND_STOPPED;
		}
		break;
	case SECOND_STOPPED:
		if (check_template(regions.at("bar3"), hwnd)) {
			std::cout << "Match for the third roller!" << std::endl;
			SimulateKeyPress('x');
			Sleep(1000);
			return ALL_STOPPED;
		}
		break;
	default:
		break;
	}
	return current_state;
}
int main()
{
	// Find the window using its title
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
	// Create an OpenCV matrix to hold the screenshot
	cv::Mat screenshot(height, width, CV_8UC4);
	HDC hdcWindow = GetDC(hwnd);
	HDC hdcMemory = CreateCompatibleDC(hdcWindow); // creates a virtual drawing surface in memory that is compatible with the provided device context
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, width, height);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmap);
	// Copy the window's image to the memory device context
	BitBlt(hdcMemory, 0, 0, width, height, hdcWindow, 0, 0, SRCCOPY);
	BITMAPINFOHEADER bi = { 0 };
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;  // Negative to ensure it's top-down
	bi.biPlanes = 1;
	bi.biBitCount = 32;     // Using 32-bit color
	bi.biCompression = BI_RGB;
	// Transfer data from the memory device context to the screenshot matrix
	GetDIBits(hdcMemory, hBitmap, 0, height, screenshot.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
	std::map<std::string, RegionData> regions;
	try {
		// Create directory if it doesn't exist
		std::filesystem::path dir("roi_templates");
		if (!std::filesystem::exists(dir)) {
			std::filesystem::create_directory(dir);
		}

		for (const auto name : { "bar1", "bar2", "bar3" }) {
			cv::Rect selectedRegion = cv::selectROI("Select " + std::string(name) + " bounding box location", capture_region(cv::Rect(0, 0, width, height), hwnd));
			cv::Mat temp = capture_region(selectedRegion, hwnd);
			regions[name] = { selectedRegion, temp };

			// save our captured regions for diagnostic purposes
			std::filesystem::path filePath = dir / name;
			filePath += ".png";
			cv::imwrite(filePath.string(), temp);
			cv::destroyAllWindows();
		}
	}
	catch (const cv::Exception& e) {
		std::cerr << "OpenCV Error: " << e.what() << std::endl;
	}
	States currentState = START;
	while (currentState != ALL_STOPPED) {
		currentState = check_state(currentState, regions, hwnd);
		Sleep(1);
	}
	return 0;
}
