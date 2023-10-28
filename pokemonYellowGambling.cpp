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
	// Retrieve width and height from the specified capture region.
	int width = region.width;
	int height = region.height;

	// Create a cv::Mat (an image container) to hold the screenshot data.
	cv::Mat screenshot(height, width, CV_8UC4);

	// Get the device context for the specified window (hwnd).
	HDC hdcWindow = GetDC(hwnd);
	if (!hdcWindow) {
		std::cerr << "Failed to get device context!" << std::endl;
		return screenshot; // If failed, return an empty screenshot.
	}

	// Create a memory device context that is compatible with the window's device context.
	HDC hdcMemory = CreateCompatibleDC(hdcWindow);
	if (!hdcMemory) {
		std::cerr << "Failed to create compatible device context!" << std::endl;
		ReleaseDC(hwnd, hdcWindow);
		return screenshot; // If failed, return an empty screenshot and release acquired resources.
	}

	// Create a bitmap compatible with the window's device context to hold the screenshot.
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcWindow, width, height);
	if (!hBitmap) {
		std::cerr << "Failed to create compatible bitmap!" << std::endl;
		DeleteDC(hdcMemory); // Release the memory device context.
		ReleaseDC(hwnd, hdcWindow); // Release the window's device context.
		return screenshot; // If failed, return an empty screenshot and release acquired resources.
	}

	// Select the new bitmap into the memory device context.
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmap);
	if (!hOldBitmap) {
		std::cerr << "Failed to select object into memory device context!" << std::endl;
		DeleteObject(hBitmap); // Delete the bitmap.
		DeleteDC(hdcMemory); // Release the memory device context.
		ReleaseDC(hwnd, hdcWindow); // Release the window's device context.
		return screenshot; // If failed, return an empty screenshot and release acquired resources.
	}

	// Copy the specified region from the window's device context to the memory device context.
	BitBlt(hdcMemory, 0, 0, width, height, hdcWindow, region.x, region.y, SRCCOPY);

	// Initialize the BITMAPINFOHEADER structure for retrieving the bitmap data.
	BITMAPINFOHEADER bi = { 0 };
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height; // Negative height indicates top-down orientation.
	bi.biPlanes = 1;
	bi.biBitCount = 32; // 32 bits-per-pixel.
	bi.biCompression = BI_RGB; // No compression.

	// Retrieve the bitmap data and store it in the cv::Mat container.
	GetDIBits(hdcMemory, hBitmap, 0, height, screenshot.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	// Clean up: Restore the old bitmap, delete created objects, and release device contexts.
	SelectObject(hdcMemory, hOldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hdcMemory);
	ReleaseDC(hwnd, hdcWindow);

	return screenshot; // Return the captured screenshot.
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
	return minVal < .05;
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
