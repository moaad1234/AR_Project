// Import the required header files
#include <tchar.h>
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include <string.h>
#include "XPLMDataAccess.h"
#include "XPLMMenus.h"
#include <charconv>

#if IBM
	#include <windows.h>
#endif
#if LIN
	#include <GL/gl.h>
#elif __GNUC__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif
#ifndef XPLM300
	#error This is made to be compiled against the XPLM300 SDK
#endif
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <fstream>
#include <strsafe.h>
#include <conio.h>
#include <windows.h>
using namespace std;
typedef int XPLMNavRef;


// Define all datarefs to be pulled in the initialisation
static XPLMDataRef		distanceRef, gDataRef, simSpeedRef, acWeightRef, simLiftRef, simDragRef, acThrustRef, acFlapRef, rhoRef, flapRef, runwayPatchyCheck, runwayFriction = NULL;
static void	MyMenuHandlerCallback(
	void* inMenuRef,
	void* inItemRef);
		


// An opaque handle to the diagnostics window
static XPLMWindowID	g_window;

// Callbacks to use when creating diagnostic X-Plane window
void				draw_hello_world(XPLMWindowID in_window_id, void * in_refcon);
int					dummy_mouse_handler(XPLMWindowID in_window_id, int x, int y, int is_down, void * in_refcon) { return 0; }
XPLMCursorStatus	dummy_cursor_status_handler(XPLMWindowID in_window_id, int x, int y, void * in_refcon) { return xplm_CursorDefault; }
int					dummy_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void * in_refcon) { return 0; }
void				dummy_key_handler(XPLMWindowID in_window_id, char key, XPLMKeyFlags flags, char virtual_key, void * in_refcon, int losing_focus) { }

// Plugin start settings
PLUGIN_API int XPluginStart(
							char *		outName,
							char *		outSig,
							char *		outDesc)
{
	XPLMMenuID	myMenu;
	int			mySubMenuItem;
	strcpy(outName, "Takeoff performance"); // Plugin name
	strcpy(outSig, "xpsdk.examples.helloworld3plugin"); // Plugin signature
	strcpy(outDesc, "Live takeoff performance tool"); // Plugin description

	mySubMenuItem = XPLMAppendMenuItem(
		XPLMFindPluginsMenu(),	/* Put in plugins menu */
		"Sim Data",				/* Item Title */
		0,						/* Item Ref */
		1);						/* Force English */
	myMenu = XPLMCreateMenu(
		"Sim Data",
		XPLMFindPluginsMenu(),
		mySubMenuItem, 			/* Menu Item to attach to. */
		MyMenuHandlerCallback,	/* The handler */
		0);						/* Handler Ref */
	
	
	XPLMCreateWindow_t params;
	params.structSize = sizeof(params);
	params.visible = 1; // Make the diagnostic window visible
	params.drawWindowFunc = draw_hello_world;
	params.handleMouseClickFunc = dummy_mouse_handler;
	params.handleRightClickFunc = dummy_mouse_handler;
	params.handleMouseWheelFunc = dummy_wheel_handler;
	params.handleKeyFunc = dummy_key_handler;
	params.handleCursorFunc = dummy_cursor_status_handler;
	params.refcon = NULL;
	params.layer = xplm_WindowLayerFloatingWindows;
	params.decorateAsFloatingWindow = xplm_WindowDecorationRoundRectangle;
	
	// Set the window's initial bounds
	// Note that we're not guaranteed that the main monitor's lower left is at (0, 0)...
	// We'll need to query for the global desktop bounds!
	int left, bottom, right, top;
	XPLMGetScreenBoundsGlobal(&left, &top, &right, &bottom);
	params.left = left + 50;
	params.bottom = bottom + 50;
	params.right = params.left + 300;
	params.top = params.bottom + 150;
	
	g_window = XPLMCreateWindowEx(&params);
	
	// Position the window as a "free" floating window, which the user can drag around
	XPLMSetWindowPositioningMode(g_window, xplm_WindowPositionFree, -1);
	// Limit resizing our window: maintain a minimum width/height of 100 boxels and a max width/height of 300 boxels
	XPLMSetWindowResizingLimits(g_window, 300, 150, 300,150);
	XPLMSetWindowTitle(g_window, "Sample Window");
	
	
	simSpeedRef = XPLMFindDataRef("sim/flightmodel/position/indicated_airspeed"); // Sim airspeed
	acWeightRef = XPLMFindDataRef("sim/flightmodel/weight/m_total"); // Aircraft weight
	simLiftRef = XPLMFindDataRef("sim/flightmodel/forces/fnrml_aero"); // Airframe lift
	simDragRef = XPLMFindDataRef("sim/flightmodel/forces/faxil_aero"); // Airframe drag
	acThrustRef = XPLMFindDataRef("sim/flightmodel/engine/POINT_thrust"); // Total engine thrust
	acFlapRef = XPLMFindDataRef("sim/flightmodel2/wing/flap1_deg"); // Aircraft flap setting
	rhoRef = XPLMFindDataRef("sim/weather/rho"); // Atmospheric density
	
	runwayPatchyCheck = XPLMFindDataRef("sim/weather/runway_is_patchy"); // Runway condition
	runwayFriction = XPLMFindDataRef("sim/weather/runway_friction"); // Runway friction
	distanceRef = XPLMFindDataRef("sim/flightmodel/controls/dist"); // Distance remaining
	float firstflapRef[8] = {}; // Create table to store different thrust values
	
	double firstCalculatedLift = 0; // Initialise theoretical lift value
	double firstCalculatedDrag = 0; // Initialise theoretical drag value
	double VcRef = 0; // Aircraft starting speed, used to find theoretical distance
	int firststep = 500; // Amount of steps to break the calculations into
	double firstFnet = 0; // Initialise airframe force
	double firstexpectedThrust = 1244100.9625; // Reference expected (maximum) aircraft thrust
	double firsta = 0; // Initialise expected acceleration
	double acWeight = 397000; // Set aircraft weight- depreciated
	double firstidealDistance = 0; // Set initial ideal distance- depreciated
	double groundMu = 0.06; // Ground rolling resistance coefficient

	/* Only return that we initialized correctly if we found the data ref. */
	return (rhoRef != NULL) ? 1 : 0;
	return g_window != NULL;
	
}

PLUGIN_API void	XPluginStop(void)
{
	// Remove the diagnostics window once the plugin is stopped
	XPLMDestroyWindow(g_window);
	g_window = NULL;
}

PLUGIN_API void XPluginDisable(void) { }
PLUGIN_API int  XPluginEnable(void)  { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, int inMsg, void * inParam) { }


//#define MESSAGE L"Test Message"

void	draw_hello_world(XPLMWindowID in_window_id, void* in_refcon)
{
	float flapRef[8] = {}; // Create table to store flap settings
	float refTable[8] = {}; // Create table to store thrust settings
	float simDrag = XPLMGetDataf(simDragRef); // Take live value of aircraft drag
	float simLift = XPLMGetDataf(simLiftRef); // Take live value of aircraft lift
	float acWeightHold = XPLMGetDataf(acWeightRef) / 10000; // Retrieve aircraft weight and downsize to prevent overflow
	float acThrust = XPLMGetDatavf(acThrustRef, refTable, 0, 8) / 10000; // Retrieve total aircraft thrust and downsize to prevent overflow
	float Vc = (XPLMGetDataf(simSpeedRef) * 0.514444);
	float liveSpeed = (XPLMGetDataf(simSpeedRef) * 0.514444);// Live airspeed and convert to m/s
	float flapSetting = XPLMGetDatavf(acFlapRef, flapRef, 0, 8); // Take degree setting of aircraft flaps
	float rho = XPLMGetDataf(rhoRef); // Atmospheric density
	float runwayLength = 9800; // Reference runway length
	int runwayCondition = XPLMGetDatai(runwayFriction); // Runway condition- good, fair, poor
	int runwayPatchy = XPLMGetDatai(runwayPatchyCheck); // Check if surface conditions are uniform
	float distance = XPLMGetDataf(distanceRef); // Retrieve distance travelled by aircraft so far
	double acWeight = (double)acWeightHold;

	double firstAdd = refTable[0]; // Engine 1 thrust
	double secondAdd = refTable[1]; // Engine 2 thrust
	double thirdAdd = refTable[2]; // Engine 3 thrust
	double fourthAdd = refTable[3]; // Engine 4 thrust
	double totalThrust = (firstAdd + secondAdd + thirdAdd + fourthAdd) / 100000; //  Add thrust from all 4 engines and downsize to prevent overflow

	// Initial creation of variables still used by the program
	double groundMu = 0.09;
	double Vr = 72.02; // Target rotation speed
	int aircraftPos = 0; // Initial aircraft position
	int step = 500; // Discretisation size
	double idealDistance = 0; // Initialise the ideal distance to target speed
	double predictedDistance = 0; // Initialise the actual predicted distance to target speed
	double Vcref = 0; // Set reference starting velocity for ideal calculate. This will not change.
	double dV = Vr - Vc; // Work out the change in velocity required for the predicted distance
	double dVhold = Vr - Vcref; // Work out the change in velocity required for the ideal distance.
	double stepChange = dV / step; // Work out the velocity increment for the predicted distance
	double stepChangeTheory = dVhold / step; // Work out the velocity increment for the ideal distance
	double calculatedLift = 0; // Initialise theoretical lift
	double liftDeviation = 0; // Comparison value 
	double calculatedDrag = 0; // Initialise theoretical drag
	double dragDeviation = 0; // Comparison value
	double liftPrediction = 0; //Comparison value
	double dragPrediction = 0; // Initialise predicted drag
	double resistance = 0; // Initialise theoretical rolling resistance
	double actualResistance = 0; // Initialise actual rolling resistance
	double expectedThrust = 0; // Initialise expected thrust
	double actualFnet = 0; // Initialise actual net force acting on the airframe
	double actualA = 0; // Initialise actual/predicted acceleration
	double Fnet = 0; // Initialise ideal net force acting on the airframe
	double a = 0; // Initialise ideal acceleration


	if (flapRef[0] == 5) { // Calculate appropriate drag and lift values depending on flap setting
		calculatedLift = (2595 + 376 * Vcref + 47.1 * pow(Vcref, 2)); // Calculate expected lift under ideal circumstances
		calculatedDrag = (109 + 32 * Vcref + 3.19 * pow(Vcref, 2)); // Calculate expected drag under ideal circumstances
		dragDeviation = simDrag / calculatedDrag; // Determine the deviation from instantaneous sim drag compared to expected drag
		liftDeviation = calculatedLift / simLift; // Determine the deviation from instantaneous sim lift compared to expected lift

	}

	if (flapRef[0] == 10) { // Calculate appropriate drag and lift values depending on flap setting
		calculatedLift = 5119 - (538 * Vcref) + (73 * pow(Vcref, 2)); // Calculate expected lift under ideal circumstances
		calculatedDrag = (194 - (1.21 * Vcref) + (4.85 * pow(Vcref, 2))); // Calculate expected drag under ideal circumstances
		dragDeviation = simDrag / calculatedDrag;  // Determine the deviation from instantaneous sim drag compared to expected dra
		liftDeviation = calculatedLift / simLift;// Determine the deviation from instantaneous sim lift compared to expected lift
	}

	if (flapRef[0] == 20) { // Calculate appropriate drag and lift values depending on flap setting
		calculatedLift = 33894 - (910 * Vcref) + (107 * pow(Vcref, 2));
		calculatedDrag = (1994 - 37.5 * Vcref + 8.62 * (Vcref, 2));
		dragDeviation = simDrag / calculatedDrag;
		liftDeviation = calculatedLift / simLift;
	}

	for (int i = 0; i < step; i++) {
		// Create repeatable function
		if (flapRef[0] == 5) { // Calculate appropriate drag and lift values depending on flap setting
			calculatedLift = (2595 + 376 * Vcref + 47.1 * pow(Vcref, 2)); // Calculate iterative lift values 
			calculatedDrag = (109 + 32 * Vcref + 3.19 * pow(Vcref, 2)); // Calculate iterative drag values 
			dragDeviation = simDrag / calculatedDrag;
			liftDeviation = calculatedLift / simLift;
		}

		if (flapRef[0] == 10) { // Calculate appropriate drag and lift values depending on flag setting
			calculatedLift = 5119 - 538 * Vcref + 73 * pow(Vcref, 2); // Calculate iterative lift values 
			calculatedDrag = (194 - 1.21 * Vcref + 4.85 * pow(Vcref, 2)); // Calculate iterative drag values 
			dragDeviation = simDrag / calculatedDrag;
			liftDeviation = calculatedLift / simLift;
		}

		if (flapRef[0] == 20) { // Calculate appropriate drag and lift values depending on flap setting
			calculatedLift = 33894 - 910 * Vcref + 107 * pow(Vcref, 2); // Calculate iterative lift values 
			calculatedDrag = (1994 - 37.5 * Vcref + 8.62 * pow(Vcref, 2)); // Calculate iterative drag values 
			dragDeviation = simDrag / calculatedDrag;
			liftDeviation = calculatedLift / simLift;
		}

		liftPrediction = calculatedLift * liftDeviation; // Based on instantaneous lift deviation, predict change in lift

		dragPrediction = calculatedDrag * dragDeviation; // Based on instantaneous drag deviation, predict change in drag

		actualResistance = groundMu * ((acWeight * 9.81 * 10000) - (liftDeviation * calculatedLift)); // Determine actual resistance based on deviations

		expectedThrust = 1244100.9625; // Expected thrust obtained from analytical method

		actualFnet = (totalThrust * 100000) - ((dragDeviation * calculatedDrag) + actualResistance); // Actual force acting on the airframe- deviation/sim based

		actualA = actualFnet / (acWeight * 10000); // Actual acceleration acting on the airframe- deviation based

		predictedDistance = predictedDistance + (pow((Vc + stepChange), 2) - pow(Vc, 2)) / (2 * actualA); // Actual distance- deviation based

		resistance = (groundMu * ((acWeight * 9.81 * 10000) - (calculatedLift))); // Ideal rolling resistance

		Fnet = (expectedThrust)-((calculatedDrag + resistance)); // Ideal force acting on the airframe- calculation based

		a = Fnet / (acWeight * 10000); // Ideal acceleration acting on the airframe- calculation based

		idealDistance = (idealDistance)+(pow((Vcref + stepChangeTheory), 2) - pow(Vcref, 2)) / (2 * a); // Calculate ideal distance based on theoretical acceleration values

		Vcref = Vcref + stepChangeTheory; // Step up reference velocity

		Vc = Vc + stepChange;

	}

	idealDistance = idealDistance * 3.28084; // Convert from m to ft

	predictedDistance = predictedDistance * 3.28084; // Convert from m to ft

	// Mandatory: We *must* set the OpenGL state before drawing
	// (we can't make any assumptions about it)
	XPLMSetGraphicsState(
		0 /* no fog */,
		0 /* 0 texture units */,
		0 /* no lighting */,
		0 /* no alpha testing */,
		1 /* do alpha blend */,
		1 /* do depth testing */,
		0 /* no depth writing */
	);

	int l, t, r, b;
	XPLMGetWindowGeometry(in_window_id, &l, &t, &r, &b); // Set window size
	//get font size to be able to include multiline strings
	int char_height;
	XPLMGetFontDimensions(xplmFont_Proportional, NULL, &char_height, NULL);


	float col_white[] = { 1.0, 1.0, 1.0 }; // Set button colours

	// Update live figures
	if (predictedDistance > (idealDistance - (distance * 3.281))
		)
	double liveDeviation = predictedDistance / (idealDistance - (distance * 3.281)); // New live deviation value
	double rwyRemaining = runwayLength - predictedDistance; // Live runway remaining
	int distanceChange = predictedDistance - (idealDistance - (distance * 3.281));
	double acceptableDeviation = ((2 * liveSpeed) / Vr) * 200 + 250;

	int rwyRemaining_toUnity = (idealDistance/3.281 + predictedDistance/3.281 - distance)/10 ; //converted to metres and downsized to prevent overflow
	//index to shift down each line
	int y = t;

	//Write runway distance remaining to Unity
	//Assign shared memory file names and buffer sizes
	#define BUF_SIZE 256
	TCHAR szName[] = TEXT("rwyRemaining_toUnity");
	//Create handle for file mapping object and string for address
	HANDLE	hMapFile;
	LPCTSTR pBuf;
	//Create a file mapping object
	//Which is a memory block that can be manipulated by MapViewOfFile
	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,		// use paging file
		NULL,						// default security
		PAGE_READWRITE,				// read/write access
		0,							// minimum object size  (high-order DWORD)
		BUF_SIZE,					// maximum object size	(low-order DWORD)
		szName						// name of mapping object
	);
	//Map view of the file to read/write to a location
	pBuf = (LPTSTR)MapViewOfFile(
		hMapFile,					// hand to map object
		FILE_MAP_ALL_ACCESS,			// read/write permission
		0,
		0,
		BUF_SIZE
	);
	//Continually update and copy to memory the value chosen
	CopyMemory((PVOID)pBuf, &rwyRemaining_toUnity, sizeof(rwyRemaining_toUnity));
	
	// Diagnostics window for debugging. Values displayed can change as necesary
	XPLMDrawString(col_white, l, y, "ROAAS Debug Window", NULL, xplmFont_Proportional);
	y -= 3 * char_height;
	XPLMDrawNumber(col_white, l, y, (idealDistance - (distance * 3.281)), 5, 5, NULL, xplmFont_Proportional);	XPLMDrawString(col_white, r - 200, y, "Dist. Remaining (Feet)", NULL, xplmFont_Proportional);
	y -= 1.5 * char_height;
	XPLMDrawNumber(col_white, l, y, distanceChange, 5, 5, NULL, xplmFont_Proportional);							XPLMDrawString(col_white, r - 200, y, "Predicted Change (Feet)", NULL, xplmFont_Proportional);
	y -= 1.5 * char_height;
	XPLMDrawNumber(col_white, l, y, acceptableDeviation, 5, 5, NULL, xplmFont_Proportional);					XPLMDrawString(col_white, r - 200, y, "Acceptable Deviation (m/s)", NULL, xplmFont_Proportional);
	y -= 1.5 * char_height;
	XPLMDrawNumber(col_white, l, y, rwyRemaining_toUnity, 5, 5, NULL, xplmFont_Proportional);					XPLMDrawString(col_white, r - 200, y, "Dist. Remaining > Unity (Metres)", NULL, xplmFont_Proportional);
	//XPLMDrawNumber(col_white, l, y, (totalThrust * 100000), 5, 5, NULL, xplmFont_Proportional);					XPLMDrawString(col_white, r - 200, y, "Total Thrust (N)", NULL, xplmFont_Proportional);
	y -= 3 * char_height;

	if (runwayCondition == 0) { // Conditional output for dry runway

		if (distanceChange > acceptableDeviation) {
			XPLMDrawString(col_white, l, y, "PERF DEG. ABORT T/O", NULL, xplmFont_Proportional); // Display takeoff command
		}
		//-------------------------------------------------------------------------------------------------------------------------------------

		if (distanceChange < 0) {
			XPLMDrawString(col_white, l, y, "Data out of range", NULL, xplmFont_Proportional); // Display takeoff command
		}
		//-------------------------------------------------------------------------------------------------------------------------------------

		if (distanceChange < acceptableDeviation && distanceChange > 0) {
			XPLMDrawString(col_white, l, y, "Improved Performance", NULL, xplmFont_Proportional); // Display takeoff command
		}
		//-------------------------------------------------------------------------------------------------------------------------------------
		if (runwayCondition == 1 && runwayPatchy == 0) { // Conditional output for damp runway

			if ((distanceChange) > (0.8 * acceptableDeviation)) {
				XPLMDrawString(col_white, l, y, "PERF DEG. ABORT T/O", NULL, xplmFont_Proportional); // Display takeoff command
			}
			//-------------------------------------------------------------------------------------------------------------------------------------
			if (distanceChange < 0) {
				XPLMDrawString(col_white, l, y, "Data out of range", NULL, xplmFont_Proportional); // Display takeoff command
			//-------------------------------------------------------------------------------------------------------------------------------------
			}
			if (distanceChange < acceptableDeviation && distanceChange > 0) {
				XPLMDrawString(col_white, l, y, "Improved Performance", NULL, xplmFont_Proportional); // Display takeoff command
			}
			//-------------------------------------------------------------------------------------------------------------------------------------
		}

		if (runwayCondition == 1 && runwayPatchy == 1) { // Conditional output for damp and patchy runway

			if (distanceChange > (0.7 * acceptableDeviation)) { // Modify tolerances
				XPLMDrawString(col_white, l, y, "PERF DEG. ABORT T/O", NULL, xplmFont_Proportional); // Display takeoff command
			//-------------------------------------------------------------------------------------------------------------------------------------
			}
			if (distanceChange < 0) { // Modify tolerances
				XPLMDrawString(col_white, l, y, "Data out of range", NULL, xplmFont_Proportional); // Display takeoff command
			//-------------------------------------------------------------------------------------------------------------------------------------
			}

			if (distanceChange < acceptableDeviation && distanceChange > 0) {
				XPLMDrawString(col_white, l, y, "Improved Performance", NULL, xplmFont_Proportional); // Display takeoff command
			}
			//-------------------------------------------------------------------------------------------------------------------------------------
		}

		if (runwayCondition == 2 && runwayPatchy == 0) { // Conditional output for wet runway

			if (distanceChange > (0.65 * acceptableDeviation)) { // Modify tolerances
				XPLMDrawString(col_white, l, y, "PERF DEG. ABORT T/O", NULL, xplmFont_Proportional); // Display takeoff command
			//-------------------------------------------------------------------------------------------------------------------------------------
			}

			if (distanceChange < 0) { // Modify tolerances
				XPLMDrawString(col_white, l, y, "Data out of range", NULL, xplmFont_Proportional); // Display takeoff command
			//-------------------------------------------------------------------------------------------------------------------------------------
			}

			if (distanceChange < acceptableDeviation && distanceChange > 0) { // Modify tolerances
				XPLMDrawString(col_white, l, y, "Improved Performance", NULL, xplmFont_Proportional); // Display takeoff command
			}
			//-------------------------------------------------------------------------------------------------------------------------------------
		}

		if (runwayCondition == 2 && runwayPatchy == 1) { // Conditional output for wet and patchy runway

			if (distanceChange > (0.65 * acceptableDeviation)) { // Modify tolerances
				XPLMDrawString(col_white, l, y, "PERF DEG. ABORT T/O", NULL, xplmFont_Proportional); // Display takeoff command
			//-------------------------------------------------------------------------------------------------------------------------------------
			}

			if (distanceChange < 0) { // Modify tolerances
				XPLMDrawString(col_white, l, y, "Data out of range", NULL, xplmFont_Proportional); // Display takeoff command
			//-------------------------------------------------------------------------------------------------------------------------------------
			}

			if (distanceChange < acceptableDeviation && distanceChange > 0) { // Modify tolerances
				XPLMDrawString(col_white, l, y, "Improved Performance", NULL, xplmFont_Proportional); // Display takeoff command
			}
			//-------------------------------------------------------------------------------------------------------------------------------------

		}
	}
}
		
void MyMenuHandlerCallback(
	void* inMenuRef,
	void* inItemRef) {
	// Handler for the menu item.
	if (gDataRef != NULL)
	{
	}
}