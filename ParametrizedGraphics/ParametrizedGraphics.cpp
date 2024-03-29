// ParametrizedGraphics.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include <stdio.h>

#include "ParametrizedGraphics.h"
#include "GraphicsLayer.h"
#include "SurfacePlane.h"

#define MAX_LOADSTRING 100
#define TIMER_ID 1
#define TIMER_MS (1000 / 60)
// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

CSurfacePlane				*g_Surface=NULL;
bool						g_pause=false;
HWND						g_hWnd; //R

char final_formula[7000];

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Formulas(HWND, UINT, WPARAM, LPARAM);
BOOL                InitDevice(HWND hWnd);
void                Render();

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PARAMETRIZEDGRAPHICS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PARAMETRIZEDGRAPHICS));
	WndProc(g_hWnd, WM_COMMAND, 98307, NULL);
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}else{
			Render();
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PARAMETRIZEDGRAPHICS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PARAMETRIZEDGRAPHICS);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   g_hWnd = hWnd;
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}



//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_FORMVIEW), hWnd, Formulas);
		break;
	case WM_KEYUP:
		switch(wParam)
		{		
			case 67:
				//ImageCapture(hWnd);	//using "c"
			break;
			case 80:
				g_pause=!g_pause;	//using "p"
			break;
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_FILE_INITD3D:
				
			if (Graphics())
			{
				MessageBox(hWnd, TEXT("D3D11 Device already initialized"), 
				TEXT("Init Skipped"), MB_OK | MB_ICONWARNING);
			}
			else
			{
				// = InitDevice(hWnd);
				
				if (InitDevice(hWnd) == FALSE)
				{
				
					MessageBox(hWnd, TEXT("Failed to Initialize D3D11"), 
                           TEXT("Init Failure"), MB_OK | MB_ICONERROR);
					//sprintf((char*)pText, "HR %d\n",g_bDeviceActive);
					//outputDebugString (pText);
				}
			}
            break;
		case ID_FILE_CLOSED3D:
            KillTimer(hWnd, TIMER_ID);
            
            //g_bDeviceActive = FALSE;
            InvalidateRect(hWnd, NULL, TRUE);
            break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_FORMVIEW), hWnd, Formulas);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
void wtoc(char* Dest, const WCHAR* Source)
{
int i = 0;

while(Source[i] != '\0')
{
Dest[i] = Source[i];
++i;
}
}

INT_PTR CALLBACK Formulas(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_BUTTON1)
		{
			char x_formula[100]={};
			char y_formula[100]={};
			char z_formula[100]={};
			char s_rangeBegin[10]={};
			char s_rangeEnd[10]={};
			char t_rangeBegin[10]={};
			char t_rangeEnd[10]={};


			{
				int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT4));
				
				if(len > 0)
						{
							// Now we allocate, and get the string into our buffer

							
							LPWSTR temp =  TEXT("1");
							temp = (LPWSTR)GlobalAlloc(GPTR, len + 1);

							GetDlgItemText(hDlg, IDC_EDIT4, temp, len + 1);

							wtoc(s_rangeBegin, temp);
							//MessageBox(hDlg,x_formula, (LPCWSTR)"Warning", MB_OK );
						}
				 
			}
			{
				int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT5));
				
				if(len > 0)
						{
							// Now we allocate, and get the string into our buffer

							
							LPWSTR temp =  TEXT("1");
							temp = (LPWSTR)GlobalAlloc(GPTR, len + 1);

							GetDlgItemText(hDlg, IDC_EDIT5, temp, len + 1);

							wtoc(s_rangeEnd, temp);
							//MessageBox(hDlg,x_formula, (LPCWSTR)"Warning", MB_OK );
						}
				 
			}
			{
				int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT6));
				
				if(len > 0)
						{
							// Now we allocate, and get the string into our buffer

							
							LPWSTR temp =  TEXT("1");
							temp = (LPWSTR)GlobalAlloc(GPTR, len + 1);

							GetDlgItemText(hDlg, IDC_EDIT6, temp, len + 1);

							wtoc(t_rangeBegin, temp);
							//MessageBox(hDlg,x_formula, (LPCWSTR)"Warning", MB_OK );
						}
				 
			}
			{
				int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT7));
				
				if(len > 0)
						{
							// Now we allocate, and get the string into our buffer

							
							LPWSTR temp =  TEXT("1");
							temp = (LPWSTR)GlobalAlloc(GPTR, len + 1);

							GetDlgItemText(hDlg, IDC_EDIT7, temp, len + 1);

							wtoc(t_rangeEnd, temp);
							//MessageBox(hDlg,x_formula, (LPCWSTR)"Warning", MB_OK );
						}
				 
			}


			//if(IDC_CHECK1)
			//TODO:check how to read checkbox value
			{
				int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT1));
				
				if(len > 0)
						{
							// Now we allocate, and get the string into our buffer

							
							LPWSTR temp =  TEXT("1");
							temp = (LPWSTR)GlobalAlloc(GPTR, len*2);

							GetDlgItemText(hDlg, IDC_EDIT1, temp, len*2);

							wtoc(x_formula, temp);
							//MessageBox(hDlg,x_formula, (LPCWSTR)"Warning", MB_OK );
						}
				 
			}
			//if(IDC_CHECK2)
			{
				int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT2));
				
				if(len > 0)
						{
							// Now we allocate, and get the string into our buffer


							LPWSTR temp =  TEXT("1");
							temp = (LPWSTR)GlobalAlloc(GPTR, len*2);

							GetDlgItemText(hDlg, IDC_EDIT2, temp, len*2);

							wtoc(y_formula, temp);

							//MessageBox(hDlg,y_formula, (LPCWSTR)"Warning", MB_OK );
						}
			}
			//if(IDC_CHECK3)
			{
				int len = GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT3));
				if(len > 0)
						{
							// Now we allocate, and get the string into our buffer


							LPWSTR temp =  TEXT("1");
							temp = (LPWSTR)GlobalAlloc(GPTR, len*2);

							GetDlgItemText(hDlg, IDC_EDIT3, temp, len*2);

							wtoc(z_formula, temp);


							//MessageBox(hDlg,z_formula, (LPCWSTR)"Warning", MB_OK );
						}
			}
			//cos(v) * cos(u) ;	pnt.y = cos(v) *sin(u) ;	pnt.z = sin(v);
			strcat(final_formula, "float3 calculateFormula( float s, float t){float3 pnt = float3(0,0,0);");
			strcat(final_formula, "s=s*(");
			strcat(final_formula, s_rangeEnd);
			strcat(final_formula, "-(");
			strcat(final_formula, s_rangeBegin);
			strcat(final_formula, "))+(");
			strcat(final_formula, s_rangeBegin);
			strcat(final_formula, ");");

			strcat(final_formula, "\n t=t*(");
			strcat(final_formula, t_rangeEnd);
			strcat(final_formula, "-(");
			strcat(final_formula, t_rangeBegin);
			strcat(final_formula, "))+(");
			strcat(final_formula, t_rangeBegin);
			strcat(final_formula, ");");

			strcat(final_formula,"\n pnt.x=");
			strcat(final_formula,x_formula);
			strcat(final_formula,";\n pnt.y=");
			strcat(final_formula,y_formula);
			strcat(final_formula,";\n pnt.z=");
			strcat(final_formula,z_formula);
			strcat(final_formula,";return pnt;} \n");
			
			strcat(final_formula, 		"cbuffer cbPerFrame : register( b0 )");
			strcat(final_formula, "{\n");
			strcat(final_formula, 	"matrix g_mWorld;                   // World matrix for object\n");
			strcat(final_formula, 	"matrix g_mView;					 // View matrix for object\n");
			strcat(final_formula, 	"matrix g_mProj;					 // View matrix for object\n");
			strcat(final_formula, 	"float4 g_vMouse;");
	
			strcat(final_formula, "};\n");
			strcat(final_formula, "struct HS_OUTPUT");
			strcat(final_formula,"{");
			strcat(final_formula, "float3 vPosition	: BEZIERPOS;\n");
			strcat(final_formula, "//float2 vTexCoord	:TEXCOORD;\n");
			strcat(final_formula,"};\n");
			strcat(final_formula, "struct HS_CONSTANT_DATA_OUTPUT");
			strcat(final_formula, 	"{");
			strcat(final_formula,	"float Edges[4]			: SV_TessFactor;\n");
			strcat(final_formula, 		"float Inside[2]			: SV_InsideTessFactor;\n");
			strcat(final_formula, 	"};\n");
			strcat(final_formula, "struct DS_OUTPUT");
			strcat(final_formula, "{");
			strcat(final_formula, "	float4 vPosition	: SV_POSITION;\n");
			strcat(final_formula, "	float3 v3DPos		: WORLDPOS;\n");
			strcat(final_formula, " float2 vTexCoord	:TEXCOORD;\n");
			strcat(final_formula, "};\n");
			strcat(final_formula, "[domain(\"quad\")]\n");
			strcat(final_formula, "DS_OUTPUT SmoothDS( HS_CONSTANT_DATA_OUTPUT input,  float2 UV : SV_DomainLocation,const OutputPatch<HS_OUTPUT, 16> bezpatch )\n");
			strcat(final_formula, "						{   DS_OUTPUT Output;\n");
			strcat(final_formula, "float3 WorldPos = calculateFormula(UV.x, UV.y);\n");
			strcat(final_formula, "Output.v3DPos =mul(float4(WorldPos,1.0f), g_mWorld);\n");
			strcat(final_formula, "Output.vPosition =mul(float4(Output.v3DPos,1), g_mView);	\n");
			strcat(final_formula, "Output.vTexCoord =UV;	\n");
			strcat(final_formula, "return Output;    \n");
			strcat(final_formula, "}");
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	case IDM_EXIT:
			DestroyWindow(hDlg);
			break;
	}

	return (INT_PTR)FALSE;
}


BOOL InitDevice(HWND hWnd)
{
	RECT rect;
    GetClientRect(hWnd, &rect);
    BOOL result = TRUE;
	CGraphicsLayer::Create(hWnd,rect.right - rect.left,rect.bottom - rect.top, final_formula);
	if(Graphics()==NULL){return FALSE;}
	
	g_Surface=new CSurfacePlane();
	
    return (result);
}

void Render()
{

	if(!Graphics()){return;}
	Graphics()->ClearBackBuffer();
	Graphics()->ClearStencilBuffer();
	if(!g_pause)g_Surface->Update(0.005);
	g_Surface->Draw();
	Graphics()->Present();	

}