#include "ImageProcess.h"

using namespace cv;
using namespace tld;

// Window name definition.
const char ImageProcess::WINDOWNAME[] = "Drone Project";

// Default constructor for the ImageProcess class.
ImageProcess::ImageProcess(NodeHandle& node, string& importModel, bool cameraOnly)
{
	// Create the window that will show the camera feed.
	namedWindow(this->WINDOWNAME, WINDOW_AUTOSIZE);

	// Set the node handle.
	this->nodeHandle = node;

	// Create a new instance of the flight class.
	this->flight = new Flight(this->nodeHandle);

	this->tld = new tld::TLD();

	if (importModel != "")
	{
		this->tld->readFromFile(importModel.c_str());
		this->modelImported = true;
	}
	else
	{
		this->modelImported = false;
	}

	// Register mouse callback to draw the tracking box
	this->currentStateData = new StateData(*this->tld, this->WINDOWNAME, *this->flight, this->modelImported, cameraOnly);

	// Set the current state to start state.
	this->currentState = new StartState();

	// States whether the writer to save video is initialised.
	this->writerInitialized = false;

}

// Destructor for the ImageProcess class.
ImageProcess::~ImageProcess(void)
{
	delete this->currentStateData;
	delete this->currentState;
	delete this->writer;
	delete this->flight;
	delete this->tld;
	destroyWindow(this->WINDOWNAME);
}

// Sets the current state data.
void ImageProcess::CurrentStateData(StateData* stateData)
{
	this->currentStateData = stateData;
}

// Gets the current state data.
StateData* ImageProcess::CurrentStateData(void)
{
	return this->currentStateData;
}

// Processes the keyboard input.
void ImageProcess::ProcessKeyInput(int input)
{
	if (input == 1048678 || input == 102)
	{
		// Pressed the f key.
		// If it is currently true, set it to false and stop tracking.
		if (this->flight->FlightStatus())
		{
			this->flight->FlightAllowed(false);
			ROS_INFO("Tracking Stopped.");
		}
		else
		{
			// Else set it to true and start flying.
			this->flight->FlightAllowed(true);
			ROS_INFO("Target-Track-Flight loop initiated.");
		}
	}
	else if (input == 1048684 || input == 108)
	{
		// Pressed the l key.
		this->flight->Land();

	}
	else if (input == 1048692 || input == 116)
	{
		// Pressed the t key.
		this->flight->TakeOff();

	}
	else if (input == 1048690 || input == 114)
	{
		// Pressed the r key.
		// Increase the Prop for PID.

		this->flight->prop += 1;
		cout << "Prop increased to: "  << this->flight->prop << endl;
		//this->flight->Reset();
		//ROS_INFO("Reset.");
	}
	else if (input == 1048677 || input == 101)
	{
		// Pressed the e key.
		// Export model to file.
		this->tld->writeToFile("model.txt");
		ROS_INFO("Written model to file.");
	}
	else if (input == 117)
	{
		// Pressed the u button.

		// Increase the altitude.
		this->flight->LinearZ(this->flight->LinearZ() + 0.1);
		this->flight->SendFlightCommand();
		cout << "Increased LinearZ to " << this->flight->LinearZ() << endl;

	}
	else if (input == 104)
	{
		// Pressed the h button.

		// Hover.
		this->flight->Hover();
		this->flight->AngularZ(0.0);
		cout << "Sent hover command" << endl;
	}
	else if (input == 106)
	{
		// Pressed the j button.

		// Decrease the altitude.
		this->flight->LinearZ(this->flight->LinearX() - 0.1);
		this->flight->SendFlightCommand();
		cout << "Decreased LinearZ to " << this->flight->LinearZ() << endl;
	}
	else if (input == 1048608 || input == 32)
	{
		// Pressed the Space bar.
		this->flight->EmergencyStop();
		ROS_INFO("EMERGENCY STOP.");

	}
	else if (input == 115)
	{
		// Pressed the s key.
		this->tld->learningEnabled = !this->tld->learningEnabled;
		cout << "Learning " << (this->tld->learningEnabled == true ? "On" : "Off") << endl;
	}

}