//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//
//	T3: Tutorial Three - Further 3D-Audio processes.
//
//	Illustrates:	Creating an A3D render object.				(T1)
//						Creating an A3D primary buffer.				(T1)
//						Creating an A3D listener object.				(T1)
//						Creating an A3D 3d-source buffer object.	(T1)
//
//						Positioning an A3D 3d-source object.		(T2)
//						Orienting an A3D 3d-source object.			(T2)
//						Positioning an A3D listener object.			(T2)
//						Orienting an A3D listener object.			(T2)
//						Setting a velocity for an A3D 3d-source.	(T2)
//						Setting a velocity for an A3D listener.	(T2)
//						Exaggerating Doppler and Rolloff effects.	(T2)
//
//						Forcing a hardware-source buffer.
//						Creating a directional source with cones.
//						Updating audio using deferred settings.
//						Minimum and maximum distances.
//						Head-relative positional settings.
//						
//
//	Created:			3rd December, 1996, by Suneil Mishra.
//						Copyright (C) 1996 Aureal Semiconductor. All Rights Reserved. 	
//
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//
// FILE: t3.cpp	Main application code for tutorial three.
//
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

/*************************************************************************************/
/*																												 */
/*	NB: CAPITALIZED COMMENTS REFER TO NECESSARY CALLS FOR 3D-AUDIO TO BE ENABLED.		 */
/*		 SEARCH FOR THE KEYWORD "TUTORIAL:" WITHIN THIS FILE FOR AUDIO EXAMPLE CODE.	 */
/*																												 */
/*************************************************************************************/


// Include headers to 3d-audio definitions and resources.
#include "..\Common\a3d.h"
#include <math.h>

extern "C"	// Cover the windowing calls for C++.
{

//////////////////////////////////////////////////////////////////////////////////////
// Application routines to create a window and display information/error messages.
//////////////////////////////////////////////////////////////////////////////////////

int  PASCAL			WinMain(HANDLE hInstance, HANDLE hPrevInstance,
								  LPSTR lpszCmdParam, int nCmdShow);
long FAR PASCAL	WndProc(HWND hwnd, UINT message, UINT wParam, LONG lParam);


//////////////////////////////////////////////////////////////////////////////////////
// WinProc: Windows-message handling.
//////////////////////////////////////////////////////////////////////////////////////
long FAR PASCAL WndProc(HWND hWnd, UINT message, UINT wParam, LONG lParam)
{
	HDC hdc;
	HPEN hpen, hpenOld;
	PAINTSTRUCT ps;
	RECT rect;
 
	switch (message)
	{
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rect);
			hpen = CreatePen(PS_SOLID, 6, RGB(0,0,255));
			hpenOld = SelectObject(hdc, hpen);
			Rectangle(hdc, rect.left+10, rect.top+10,
							  rect.right-10, rect.bottom-10);

			DrawText(hdc, "A3D: Tutorial Three", -1, &rect,
					  DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			SelectObject(hdc, hpenOld);
			DeleteObject(hpen);
			EndPaint(hWnd, &ps);
			return (0);
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return (0);
		break;

		default:
		break;
	}

	return (DefWindowProc(hWnd, message, wParam, lParam));
}

//////////////////////////////////////////////////////////////////////////////////////
// WinMain: Main application-loop for windows.
//////////////////////////////////////////////////////////////////////////////////////
int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance,
						 LPSTR lpszCmdParam, int nCmdShow)
{
	static char szAppName[] = "Tutorial-Three";
	HWND hWnd;
	MSG msg;
	WNDCLASS wndclass;
  
	if (!hPrevInstance)
	{
		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = WndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInstance;
		wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground = GetStockObject(WHITE_BRUSH);
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = szAppName;

		RegisterClass(&wndclass);
	}

	hWnd = CreateWindow(szAppName, "A3D Tutorial", WS_OVERLAPPEDWINDOW,
							  CW_USEDEFAULT, CW_USEDEFAULT, 200, 100, NULL,
							  NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// TUTORIAL: INSTANCE THE AUDIO RENDERER.
	CA3dRenderer *pA3d = new CA3dRenderer;

	// TUTORIAL: INITIALIZE THE AUDIO-RENDERER.
	HRESULT hr = pA3d->CreateA3dRenderer(hWnd,					// Use application's window-handle for scope.
										 RENDER_A3D);			// Use A3D as renderer
													 
	//
	// IMPORTANT NOTE!!!
	// You should ALWAYS check the return-codes for any calls.
	//
	if (hr < DS_OK)	// Check if we were successful
	{
		// Something went wrong in the initialization.
		pA3d->DestroyA3dRenderer();	// Cleanup before exiting.
		delete pA3d;
		pA3d = NULL;
		return(-1);
	}

	// Successfully created a 3D-audio renderer.
	// TUTORIAL: INSTANCE A LISTENER.
	CA3dListener *pA3dListener = new CA3dListener;
	
	// TUTORIAL: INSTANCE SOURCES
	CA3dSource	 *pA3dSourceA	= new CA3dSource;
	CA3dSource	 *pA3dSourceB	= new CA3dSource;

	// TUTORIAL: CREATE AND INITIALIZE A 3D-LISTENER.
	if ((hr = pA3dListener->CreateListener(pA3d->m_p3DAudioRenderer)) != DS_OK)
	{
		pA3dListener->DestroyListener();	// Cleanup before exiting.
		pA3d->DestroyA3dRenderer();		

		delete pA3dListener;
		delete pA3d;
		pA3dListener = NULL;
		pA3d			 = NULL;
		return(-1);
	}
	
	// NB: When setting values to their defaults, this is simply to illustrate the specific call.
	//		 Default values, of course, are set "by default" implicitly.

	// TUTORIAL: SET LISTENER POSITION TO AN ARBITRARY POSITION OF (0, 0, -5).
	if ((pA3dListener) && (pA3dListener->m_pDS3DListener))
		pA3dListener->m_pDS3DListener->SetPosition(0.0f, 0.0f, -5.0f, DS3D_IMMEDIATE);
	
	// TUTORIAL: SET LISTENER ORIENTATION TO DEFAULT OF (0, 0, 1)-FRONT, (0, 1, 0)-TOP.
	if ((pA3dListener) && (pA3dListener->m_pDS3DListener))
		pA3dListener->m_pDS3DListener->SetOrientation(0.0f, 0.0f, 1.0f,	// Normalized front-vector. 
																	 0.0f, 1.0f, 0.0f,	// Normalized top-vector.
																	 DS3D_IMMEDIATE);
	
	// TUTORIAL: SET LISTENER VELOCITY FOR STATIONARY LISTENER.
	// Note that the velocity setting is independent of position.
	// The listener may have a velocity and not actually be changing position.
	// This allows for special effects to be applied. The same applies for 3d-sources.
	if ((pA3dListener) && (pA3dListener->m_pDS3DListener))
		pA3dListener->m_pDS3DListener->SetVelocity(0.0f, 0.0f, 0.0f, DS3D_IMMEDIATE);
	
	// TUTORIAL: SET ROLLOFF FACTOR TO ONE-TENTH NORMAL, TO SHOW SPECIAL EFFECT. (DISTANCE CUES.)
	// Adjust this value to see the effect of varying distance cues; 1.0 is the default value.
	if ((pA3dListener) && (pA3dListener->m_pDS3DListener))
		pA3dListener->m_pDS3DListener->SetRolloffFactor(0.1f, DS3D_IMMEDIATE);

	// TUTORIAL: SET DOPPLER FACTOR TO TWICE NORMAL, TO SHOW SPECIAL EFFECT. (VELOCITY CUES.)
	// Adjust this value to see the effect of varying velocity cues; 1.0 is the default value.
	if ((pA3dListener) && (pA3dListener->m_pDS3DListener))
		pA3dListener->m_pDS3DListener->SetDopplerFactor(2.0f, DS3D_IMMEDIATE);

	// TUTORIAL: CREATE AND INITIALIZE A 3D-SOURCE.
	// Make this source optimally-processed, looping, and play it immediately.
	if ((hr = pA3dSourceA->CreateSource(pA3d->m_p3DAudioRenderer, "Heli22.wav",
													BUFFER_OPTIMAL, PLAY_IMMEDIATE, PLAY_LOOPING)) != DS_OK)
	{
		pA3dSourceA->DestroySource();		// Cleanup before exiting.
		pA3dListener->DestroyListener();
		pA3d->DestroyA3dRenderer();
		
		delete pA3dSourceA;
		delete pA3dListener;
		delete pA3d;
		pA3dSourceA	 = NULL;
		pA3dListener = NULL;
		pA3d			 = NULL;
		return(-1);
	}

	// TUTORIAL: SET THIS SOURCE TO BE HEAD-RELATIVE OR NOT IN ITS COORDINATE-SYSTEM.
	// In head-relative mode, all locations are based on the head position.
	// This means each coordinate value is an offset from where the listener is positioned.
	// Compare the difference between these two modes by selecting from the two lines below.
	// The difference will be most noticeable in the Doppler-effect which is based on
	// relative positions and velocities of a source and the listener.
	if ((pA3dSourceA) && (pA3dSourceA->m_pDS3DBuffer3D))
	{
		//pA3dSourceA->m_pDS3DBuffer3D->SetMode(DS3DMODE_HEADRELATIVE, DS3D_IMMEDIATE);
		pA3dSourceA->m_pDS3DBuffer3D->SetMode(DS3DMODE_NORMAL, DS3D_IMMEDIATE);		// Default mode.
	}

	// TUTORIAL: CREATE AND INITIALIZE ANOTHER 3D-SOURCE.
	// Make this source hardware-processed, looping, but don't play it immediately.
	if ((hr = pA3dSourceB->CreateSource(pA3d->m_p3DAudioRenderer, "Bee22.wav",
													BUFFER_HARDWARE, PLAY_DEFERRED, PLAY_LOOPING)) != DS_OK)
	{
		pA3dSourceB->DestroySource();		// Cleanup before exiting.

		// Kill the other source before exiting.
		if(pA3dSourceA)
		{
			pA3dSourceA->DestroySource();
			delete pA3dSourceA;
			pA3dSourceA = NULL;
		}

		pA3dListener->DestroyListener();
		pA3d->DestroyA3dRenderer();
		
		delete pA3dSourceB;
		delete pA3dListener;
		delete pA3d;
		pA3dSourceB	 = NULL;
		pA3dListener = NULL;
		pA3d			 = NULL;
		return(-1);
	}
	
	// Keep track of when this source was loaded so we can play it (arbitrarily) 5 seconds later. 
	static DWORD dwSourceBLoadTime = GetCurrentTime();
	
	// TUTORIAL: SET SOURCE-B MINIMUM AND MAXIMUM DISTANCES.
	// Try different values for these to see their effect.
	if ((pA3dSourceB) && (pA3dSourceB->m_pDS3DBuffer3D))
	{
		// TUTORIAL: THE MINIMUM DISTANCE IS THE DISTANCE AT WHICH SOURCES WILL GET NO LOUDER.
		pA3dSourceB->m_pDS3DBuffer3D->SetMinDistance(0.1f, DS3D_IMMEDIATE);	// Default is 1.0.

		// TUTORIAL: THE MAXIMUM DISTANCE IS THE DISTANCE AT WHICH SOURCES WILL GET NO QUIETER.
		pA3dSourceB->m_pDS3DBuffer3D->SetMaxDistance(5.0f, DS3D_IMMEDIATE);	// Default is 'infinity'.
	}

	// Main application-loop.
	// This basic example code is simply to demonstrate audio functionality.

	// Debug output string.
	char szTmpDebug[50];

	// Setup some variables and values for having a source rotate around the listener.
	// Note: this is pretty inefficient, but is meant only to illustrate a simple motion.
	//			also, this does not give a constant frame-rate. Use timers to do this if you require
	//			a constant framerate, or have frame-based motion increments you want to be consistent.
	const float fRadius = 6.0f;
	const float fHeight = 5.0f;
	const float fRotate = 0.3f;
	const float fPI	  = 3.1415927f;

	// TUTORIAL: DEFER SETTINGS OR USE IMMEDIATE MODE.
	const DWORD	 dwApplyType = DS3D_IMMEDIATE;
	//const DWORD	 dwApplyType = DS3D_DEFERRED;

	static DWORD	 dwLastFrameTime = GetCurrentTime();
	static float	 fSourceAPosition = 0.0f;
	static D3DVALUE xPos = 0.0f;
	static D3DVALUE yPos = fHeight;
	static D3DVALUE zPos = fRadius;
	D3DVALUE			 xVel, yVel, zVel;
	float				 fRandX, fRandY, fRandZ;

	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// If exiting...
			if (msg.message == WM_QUIT)
			{
				// TUTORIAL: DESTROY SOURCES.
				if (pA3dSourceA)
				{
					pA3dSourceA->DestroySource();
					delete pA3dSourceA;
					pA3dSourceA = NULL;
				}

				if (pA3dSourceB)
				{
					pA3dSourceB->DestroySource();
					delete pA3dSourceB;
					pA3dSourceB = NULL;
				}

				// TUTORIAL: DESTROY LISTENER.
				if (pA3dListener)
				{
					pA3dListener->DestroyListener();
					delete pA3dListener;
					pA3dListener = NULL;
				}

				// TUTORIAL: DESTROY RENDERER.
				if (pA3d)
				{
					pA3d->DestroyA3dRenderer();
					delete pA3d;
					pA3d = NULL;
				}
				
				break;
			}
  
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			/* 
			 *		FRAME-UPDATED AUDIO FUNCTIONALITY.
			 */

			// TUTORIAL: ALL FRAME-UPDATED SOURCE-A FUNCTIONS USE DEFERRED-SETTINGS IN THIS TUTORIAL.
			//	Deferred settings allow a number of values to be changed but does not call
			// the update for the renderer, until some later time. This way, repeated calls to
			// the renderer can be avoided, aiding performance for frame-updated functions.

			// increment angular position.
			if (fSourceAPosition > 360)
				fSourceAPosition = 0.0f;
			else	
				fSourceAPosition += fRotate;

			// Calculate the velocity and new position for each coordinate.
			xVel = xPos; // Temporary storage of previous position.
			yVel = yPos;
			zVel = zPos;
			
			// Calculate frame-time in seconds.
			float fTimeElapsed = (GetCurrentTime() - dwLastFrameTime) / 1000.0f;

			// Rotate sound source around the head; calculate new position of the source.
			xPos = fRadius * (float) cos((fSourceAPosition / 180.0f * fPI));
			yPos = fHeight;
			zPos = fRadius * (float) sin((fSourceAPosition / 180.0f * fPI));
			
			// Update velocity.
			if(fTimeElapsed != 0)	// Make sure there's no divide-by-zero.
			{
				xVel = (xPos - xVel) / fTimeElapsed;
				yVel = (yPos - yVel) / fTimeElapsed;
				zVel = (zPos - zVel) / fTimeElapsed;
			}

			// Update this frame-time as last frame-time.
			dwLastFrameTime = GetCurrentTime();

			// TUTORIAL: ALTERNATE STARTING AND STOPPING THE LOADED SOURCE-B.
			if ((dwLastFrameTime - dwSourceBLoadTime) > 5000)
			{
				// Use the load time as an update counter to test for 5 second intervals.
				dwSourceBLoadTime = dwLastFrameTime;

				// Check validity of source.
				if ((pA3dSourceB) && (pA3dSourceB->m_pDS3DBuffer2D))
				{
					// Toggle between playing and stopping dependent on the source's current status.
					if (pA3dSourceB->m_3DSourceInfo.ePlayStatus == PLAY_INACTIVE)
					{
						pA3dSourceB->PlaySource();	// Start the source playing.

						// To make things more interesting, move the sound randomly each time we play.
						// TUTORIAL: SET POSITION OF SOURCE-B WITH BOUNDED RANDOM POSITION.
						srand((unsigned) dwLastFrameTime);	// Seed with time to get different randoms.
						fRandX = (float) (rand() % 10) - 5; // Random offset between -5 and 5
						fRandY = (float) (rand() %  8) - 4; // Random offset between -4 and 4.
						fRandZ = (float) (rand() % 16) - 8; // Random offset between -8 and 8;
						pA3dSourceB->m_pDS3DBuffer3D->SetPosition(fRandX, fRandY, fRandZ,	DS3D_IMMEDIATE);
					}
					else
						pA3dSourceB->StopSource();	// Stop the source playing.
				}
			}

			// TUTORIAL: POSITION SOURCE-A IN 3-SPACE.
			if ((pA3dSourceA) && (pA3dSourceA->m_pDS3DBuffer3D))
				pA3dSourceA->m_pDS3DBuffer3D->SetPosition(xPos, yPos, zPos, dwApplyType);
			
			// TUTORIAL: SET SOURCE-A ORIENTATION TO POINT TOWARDS ORIGIN (0, 0, 0).
			// Setting the source-orientation has no effect unless the source is directional.
			if ((pA3dSourceA) && (pA3dSourceA->m_pDS3DBuffer3D))
			{
				// Pass the radius-normalized vector facing towards the origin from SourceA.
				pA3dSourceA->m_pDS3DBuffer3D->SetConeOrientation((-xPos/fRadius), (-yPos/fRadius),
																				 (-zPos/fRadius),  dwApplyType);
			}
	
			// TUTORIAL: SET SOURCE-A CONES TO PRODUCE A DIRECTIONAL SOURCE.
			// A directional source is specified by two cones: an inner cone and outer cone.
			// These cones are defined by angles through which a cone is swept in 3-space.
			// The gain for the source is controlled by these cones. Within the inner cone, the source
			// does not increase its gain; outwith the outer cone, the source does not decrease its gain
			// below a set outside-cone-volume; between the inner cone and outer cone,
			// the gain transitions from the maximal gain for the source, to the outer-cone volume.
			// Note: The cone-orientation was already set above.

			// TUTORIAL: SET OUTSIDE-CONE-VOLUME.
			// This value is in units of hundredths-of-decibels.
			if ((pA3dSourceA) && (pA3dSourceA->m_pDS3DBuffer3D))
				pA3dSourceA->m_pDS3DBuffer3D->SetConeOutsideVolume(-1000, dwApplyType);

			// TUTORIAL: SET CONE-ANGLES.
			// These values are in degrees.
			if ((pA3dSourceA) && (pA3dSourceA->m_pDS3DBuffer3D))
				pA3dSourceA->m_pDS3DBuffer3D->SetConeAngles((DWORD) 30, (DWORD) 60, dwApplyType);


			// TUTORIAL: SET SOURCE-A VELOCITY IN 3-SPACE.
			// Note that the velocity setting is independent of position.
			// The 3d-source may have a velocity and not actually be changing position.
			// This allows for special effects to be applied. The same applies for the listener.
			if ((pA3dSourceA) && (pA3dSourceA->m_pDS3DBuffer3D) && (fTimeElapsed != 0))
				pA3dSourceA->m_pDS3DBuffer3D->SetVelocity(xVel, yVel, zVel, dwApplyType);
			
			// TUTORIAL: IF DEFERRING, APPLY DEFERRED SETTINGS AT END OF EACH FRAME.
			// Note: The actual commiting of deferred settings is attached to the listener object.
			if(dwApplyType == DS3D_DEFERRED)
			{
				if ((pA3dListener) && (pA3dListener->m_pDS3DListener))
				{
					if ((hr = pA3dListener->m_pDS3DListener->CommitDeferredSettings()) != DS_OK)
						pA3d->DisplayErrorBox(hr);			// Display what went wrong.
				}
			}

			// Enable debug of Source-A's positions and velocities, and
			// Source-B's positions if interested in seeing them change.
			BOOL bDebug = FALSE;
			if(bDebug)
			{
				sprintf(szTmpDebug, "SourceA: XPos: %3.2f YPos: %3.2f ZPos: %3.2f\n", xPos, yPos, zPos);
				OutputDebugString(szTmpDebug);
				sprintf(szTmpDebug, "SourceA: XVel: %3.2f YVel: %3.2f ZVel: %3.2f\n\n", xVel, yVel, zVel);
				OutputDebugString(szTmpDebug);
				sprintf(szTmpDebug, "SourceB: XPos: %3.2f YPos: %3.2f ZPos: %3.2f\n", fRandX, fRandY, fRandZ);
				OutputDebugString(szTmpDebug);
			}
		}
	}

	return (msg.wParam);
}


}	// extern "C".

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
