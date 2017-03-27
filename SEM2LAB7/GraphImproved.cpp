#undef UNICODE

#define EXP(x) exp(x)
#define LN(x) log(x)
#define SQRT(x) sqrt(x)
#define XSINX(x) (double)x*sin(x)
#define CUSTOM(x) (double)x*x

#define EXP_ID 1005
#define LN_ID 1006
#define SQRT_ID 1007
#define XSINX_ID 1008
#define CUSTOM_ID 1009
#define ID_COLOR_ACCEL 40005
#define IDM_ABOUT 40007
#define IDR_MENU 101
#define IDR_CONTEXTMENU 106
#define IDD_WIDTHDIALOG 108
#define IDD_ABOUT 112
#define ID_SETTINGS_GRAPHSETTINGS 40001
#define ID_HACCEL 111
#define ID_SETTINGS_SETCOLOR 40002
#define IDD_GRAPHDIALOG 102
#define IDB_GRAPHAPPLY 1003
#define IDB_GRAPHCANCEL 1004
#define IDE_GRAPHEA 1001
#define IDE_GRAPHEB 1002
#define IDE_CUSTOMEDIT 1010
#define ID_WIDTHSLIDER 1011
#define ID_EXIT 1020
#define ID_WIDTH 1021

#pragma comment(lib,"RPN.lib") 
#include <windows.h>
#include  <math.h>
#include <cmath>
#include <string>
#include <Commctrl.h>
using namespace std;

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

struct DrawAreaInfo
{
	double xPoints, yPoints, divValueX, divValueY, newX, newY, a, b, factorX, factorY;
};

static int globalGraphWidth = 1;
static COLORREF globalGraphColor;
static DrawAreaInfo dAInfo;
static int funcID;
static HWND hwndGlobal;

struct Graph
{
	int a;
	int b;
	int c;
};
bool isNumber(char* str, int len);
BOOL InitApplication(HINSTANCE hinstance);
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
DrawAreaInfo GetAreaInfo(double x, double y, int a, int b, int ID);
void Draw(HDC& hdc, int x, int y, int funcID);
POINT ConvertCoordinates(int x, int y, int widthOld, int heightOld);
double GetDistance(int x1, int y1, int x2, int y2);
double CALLFUNC(int id, double x);
pair<double, double> GetFuncMinMax(int ID);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevHinstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;


	if (!InitApplication(hinstance))
	{
		MessageBox(NULL, "Unable to Init App", "Error", MB_OK);
		return FALSE;
	}

	if (!InitInstance(hinstance, nCmdShow))
	{
		MessageBox(NULL, "Unable to Init Instance", "Error", MB_OK);
		return FALSE;
	}

	HACCEL haccel = LoadAccelerators(hinstance, MAKEINTRESOURCE(ID_HACCEL));
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateAccelerator(hwndGlobal, haccel, &msg);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

BOOL InitApplication(HINSTANCE hinstance)
{
	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hinstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
	wndclass.lpszClassName = "Graph";
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wndclass))
	{
		MessageBox(NULL, "Cannot register class", "Error", MB_OK);
		return FALSE;
	}
	return TRUE;
}

BOOL CALLBACK WidthBoxHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	static HWND slider;
	switch (message)
	{
	case WM_INITDIALOG:
		slider = GetDlgItem(hwnd, ID_WIDTHSLIDER);
		SendMessage(slider, TBM_SETRANGE, (WPARAM)FALSE, (LPARAM)MAKELONG(1, 3));
		break;
	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
		case IDOK:
			int val = SendMessage(slider, TBM_GETPOS, NULL, NULL);
			globalGraphWidth = val;
			InvalidateRect(GetParent(hwnd), NULL, true);
			EndDialog(hwnd, 0);
			break;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK AboutHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
		case IDCANCEL:
			EndDialog(hwnd, 0);
			break;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK GraphBoxHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	string text;
	static pair<int, int> tempFrontiers;

	switch (message)
	{
	case WM_INITDIALOG:
		CheckRadioButton(hwnd, EXP_ID, CUSTOM_ID, funcID);
		tempFrontiers.first = tempFrontiers.second = MAXINT32;
		text = "a = " + to_string(dAInfo.a);
		SetDlgItemText(hwnd, IDE_GRAPHEA, text.data());
		text = "b = " + to_string(dAInfo.b);
		SetDlgItemText(hwnd, IDE_GRAPHEB, text.data());
		SetDlgItemText(hwnd, IDE_CUSTOMEDIT, "f(x)=");
		break;
	case WM_COMMAND:
	{
		switch (LOWORD(wparam))
		{
		case IDB_GRAPHAPPLY:
		{
			//if (tempFrontiers.first == tempFrontiers.second) { EndDialog(hwnd, 0); return FALSE; };
			if (tempFrontiers.first != MAXINT32)
				dAInfo.a = tempFrontiers.first;
			if (tempFrontiers.second != MAXINT32)
				dAInfo.b = tempFrontiers.second;

			InvalidateRect(GetParent(hwnd), NULL, true);
			EndDialog(hwnd, 0);
		}
		break;
		case IDB_GRAPHCANCEL:
			EndDialog(hwnd, 0);
			break;
		case IDE_GRAPHEA:
			switch (HIWORD(wparam))
			{
			case EN_SETFOCUS:
			{
				SetDlgItemText(hwnd, IDE_GRAPHEA, "");
				break;
			}
			case EN_KILLFOCUS:
			{
				char buffer[5];
				GetDlgItemText(hwnd, IDE_GRAPHEA, buffer, 4);
				if (isNumber(buffer, strlen(buffer)))
				{
					tempFrontiers.first = atoi(buffer);
					text = "a = " + to_string(tempFrontiers.first);
					SetDlgItemText(hwnd, IDE_GRAPHEA, text.data());
				}
				else
				{
					MessageBox(hwnd, "Error", "Not a number", MB_OK);
					text = "a = " + to_string(dAInfo.a);
					SetDlgItemText(hwnd, IDE_GRAPHEA, text.data());
				}
			}
			}
			break;
		case IDE_GRAPHEB:
			switch (HIWORD(wparam))
			{
			case EN_SETFOCUS:
			{
				SetDlgItemText(hwnd, IDE_GRAPHEB, "");
				break;
			}
			case EN_KILLFOCUS:
			{
				char buffer[5];
				GetDlgItemText(hwnd, IDE_GRAPHEB, buffer, 4);
				if (isNumber(buffer, strlen(buffer)))
				{
					tempFrontiers.second = atoi(buffer);
					text = "b = " + to_string(tempFrontiers.second);
					SetDlgItemText(hwnd, IDE_GRAPHEB, text.data());
				}
				else
				{
					MessageBox(hwnd, "Error", "Not a number", MB_OK);
					text = "b = " + to_string(dAInfo.b);
					SetDlgItemText(hwnd, IDE_GRAPHEB, text.data());
				}
			}
			break;
			}
			break;

		case EXP_ID:
			funcID = EXP_ID;
			break;
		case LN_ID:
			funcID = LN_ID;
			break;
		case SQRT_ID:
			funcID = SQRT_ID;
			break;
		case XSINX_ID:
			funcID = XSINX_ID;
			break;
		case CUSTOM_ID:
			funcID = CUSTOM_ID;
			// to get text from text edit 
			break;
			// handle custom
		}
	}
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	static int x, y, a, b;

	static HDC hdc;
	RECT client;

	PAINTSTRUCT ps;
	static Graph gr;

	switch (message)
	{
	case WM_CREATE:
		// p = GetNotation("(x+8*x)");
		gr.a = 1; gr.b = 1; gr.c = 1;
		funcID = EXP_ID;
		a = -5; b = 5;
		dAInfo.a = a;
		dAInfo.b = b;

	case WM_SIZE:
		GetClientRect(hwnd, &client);
		x = client.right;
		y = client.bottom;
		GetAreaInfo(x, y, dAInfo.a, dAInfo.b, funcID);
		break;

	case WM_COMMAND:
	{
		switch (LOWORD(wparam))
		{
		case IDM_ABOUT:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutHandler);
			break;
		case ID_WIDTH:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_WIDTHDIALOG), hwnd, WidthBoxHandler);
			break;
		case ID_EXIT:
			int res; res = MessageBox(NULL, "Do you really want to exit?", "Message", MB_OKCANCEL);
			switch (res)
			{
			case IDOK:
				SendMessage(hwnd, WM_CLOSE, NULL, NULL);
			default:
				break;
			}
			break;
		case ID_SETTINGS_GRAPHSETTINGS:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_GRAPHDIALOG), hwnd, GraphBoxHandler);
			break;
		case ID_SETTINGS_SETCOLOR:
			CHOOSECOLOR ccs;
			COLORREF acrCustClr[16];
			ccs.lStructSize = sizeof(CHOOSECOLOR);
			ccs.hwndOwner = hwnd;
			ccs.rgbResult = RGB(255, 255, 255);
			ccs.Flags = CC_RGBINIT | CC_FULLOPEN;
			ccs.lpCustColors = (LPDWORD)acrCustClr;

			if (ChooseColor(&ccs))
			{
				globalGraphColor = ccs.rgbResult;
				InvalidateRect(hwnd, NULL, true);
			}

			break;
		}
		break;
	}
	case WM_PAINT:
	{
		GetAreaInfo(x, y, dAInfo.a, dAInfo.b, funcID);
		RECT Rect;

		string text = "Scale X : " + to_string(dAInfo.factorX) + " , Y : " + to_string(dAInfo.factorY);

		hdc = BeginPaint(hwnd, &ps);
		SetMapMode(hdc, MM_ANISOTROPIC);
		SetWindowExtEx(hdc, x, y, NULL);
		SetViewportExtEx(hdc, x, -y, NULL);
		SetViewportOrgEx(hdc, x / 2, y / 2, NULL);

		SetRect(&Rect, -x / 2, y / 2, -x / 2 + 0.3*x, y / 2 - 0.1*y);
		Draw(hdc, x, y, funcID);
		DrawText(hdc, text.data(), text.size(), &Rect, DT_SINGLELINE | DT_LEFT);
		EndPaint(hwnd, &ps);
		break;
	}

	case WM_LBUTTONDOWN:
	{
		DrawAreaInfo di = GetAreaInfo(x, y, a, b, funcID);
		int cx = LOWORD(lparam);
		int cy = HIWORD(lparam);
		POINT clickedCoord = ConvertCoordinates(cx, cy, x, y);

		double fx = CALLFUNC(funcID, (double)clickedCoord.x / di.divValueX);

		double distance = abs(fx - ((double)clickedCoord.y) / di.divValueY);
		if (distance <= 0.3)
		{
			string text;
			double t = (double)clickedCoord.x / di.divValueX;
			text += "X: " + to_string(t); text += ", Y : " + to_string(fx);
			hdc = GetDC(hwnd);
			TextOut(hdc, 0, 0, text.c_str(), text.size());
			ReleaseDC(hwnd, hdc);
		}
		break;
	}
	case WM_RBUTTONUP:
		POINT pt;
		pt.x = LOWORD(lparam);
		pt.y = HIWORD(lparam);
		ClientToScreen(hwnd, &pt);

		HMENU hMenu; hMenu = CreatePopupMenu();

		AppendMenu(hMenu, MFT_STRING, ID_WIDTH, "&Width");
		AppendMenu(hMenu, MFT_SEPARATOR, 0, NULL);
		AppendMenu(hMenu, MFT_STRING, ID_EXIT, "&Exit");

		TrackPopupMenu(hMenu, TPM_RIGHTBUTTON |
			TPM_TOPALIGN |
			TPM_LEFTALIGN,
			pt.x,
			pt.y, 0, hwnd, NULL);
		DestroyMenu(hMenu);

		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
	return FALSE;
}
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow)
{
	hwndGlobal = CreateWindow(
		"Graph",
		"Graph",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		NULL,
		NULL,
		hinstance,
		NULL);


	if (!hwndGlobal)
		return FALSE;
	ShowWindow(hwndGlobal, nCmdShow);
	UpdateWindow(hwndGlobal);
	return TRUE;
}
POINT ConvertCoordinates(int x, int y, int widthOld, int heightOld)
{
	// center is (x/2, y/2)
	POINT pt;
	pt.x = x - widthOld / 2;
	pt.y = heightOld / 2 - y;
	return pt;
}

DrawAreaInfo GetAreaInfo(double x, double y, int aa, int bb, int ID)
{
	if (!x || !y) return DrawAreaInfo();

	dAInfo.a = aa; dAInfo.b = bb;
	int xPoints, yPoints, divValueX, divValueY, factorX = 1, factorY = 1; // Кол-во делений

	if (sgn(aa) != sgn(bb))
		xPoints = abs(bb - aa) + 2;
	else
		xPoints = (abs(bb) > abs(aa)) ? bb + 3 : aa + 3;

	if (dAInfo.a > dAInfo.b) swap(dAInfo.a, dAInfo.b);
	divValueX = x / (xPoints * 2);
	while (divValueX <= globalGraphWidth * 3)
	{
		xPoints /= 2;
		factorX++;
		divValueX = x / (xPoints * 2);
	}

	dAInfo.divValueX = divValueX;

	pair <double, double> minMax = GetFuncMinMax(ID);
	double min = ceil(abs(minMax.first)), max = ceil(abs(minMax.second));

	if (max != MAXINT32 && abs(min) <= MAXINT32 -2)
		yPoints = (max > min) ? max : min;//max;
	else
		yPoints = xPoints * 2;
	if (max == min)
		yPoints = xPoints * 2;

	divValueY = y / (yPoints * 2);
	while (divValueY <= globalGraphWidth * 3)
	{
		yPoints /= 2;
		factorY++;
		divValueY = y / (yPoints * 2);
	}


	int newX, newY; newX = x / 2; newY = y / 2;

	dAInfo.divValueY = divValueY; dAInfo.newX = newX; dAInfo.newY = newY; dAInfo.xPoints = xPoints; dAInfo.yPoints = yPoints;
	dAInfo.factorX = factorX; dAInfo.factorY = factorY;
	return dAInfo;
}
void Draw(HDC& hdc, int x, int y, int ID)
{
	HPEN newPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN oldPen = (HPEN)SelectObject(hdc, newPen);

	MoveToEx(hdc, -dAInfo.newX, 0, NULL);
	LineTo(hdc, dAInfo.newX, 0);
	MoveToEx(hdc, 0, dAInfo.newY, NULL);
	LineTo(hdc, 0, -dAInfo.newY);


	DeleteObject(newPen);
	SelectObject(hdc, oldPen);

	for (int xx = 0; xx <= dAInfo.newX; xx += dAInfo.divValueX * dAInfo.factorX / dAInfo.factorX)
	{
		MoveToEx(hdc, xx, 5, NULL);
		LineTo(hdc, xx, -5);
		MoveToEx(hdc, -xx, 5, NULL);
		LineTo(hdc, -xx, -5);
	}

	for (int yy = 0; yy <= dAInfo.newY; yy += dAInfo.divValueY * dAInfo.factorY / dAInfo.factorY)
	{
		MoveToEx(hdc, 5, yy, NULL);
		LineTo(hdc, -5, yy);
		MoveToEx(hdc, 5, -yy, NULL);
		LineTo(hdc, -5, -yy);
	}
	//MoveToEx(hdc, dAInfo.a, 0, NULL);

	double fx = CALLFUNC(ID, dAInfo.a * dAInfo.divValueX);

	if (fx == fx)
		MoveToEx(hdc, dAInfo.a * dAInfo.divValueX, fx * dAInfo.divValueY, NULL);

	newPen = CreatePen(PS_SOLID, globalGraphWidth, globalGraphColor);
	oldPen = (HPEN)SelectObject(hdc, newPen);
	for (double xArg = dAInfo.a * dAInfo.divValueX / dAInfo.factorX; xArg <= dAInfo.b * dAInfo.divValueX / dAInfo.factorX; xArg += 0.05)
	{
		fx = CALLFUNC(ID, xArg / dAInfo.divValueX);
		if (fx == fx)
			//LineTo(hdc, xArg, fx * dAInfo.divValueY);
			Ellipse(hdc, xArg - globalGraphWidth, fx * dAInfo.divValueY / dAInfo.factorY - globalGraphWidth, xArg + globalGraphWidth, fx * dAInfo.divValueY / dAInfo.factorY + globalGraphWidth);
		//SetPixel(hdc, xArg, fx * dAInfo.divValueY, RGB(0, 0, 0));
	}
	SelectObject(hdc, oldPen);
	DeleteObject(newPen);

	newPen = CreatePen(PS_DOT, 1, BLACK_PEN);
	oldPen = (HPEN)SelectObject(hdc, newPen);
	MoveToEx(hdc, dAInfo.a * dAInfo.divValueX / dAInfo.factorX, dAInfo.newY, NULL);
	LineTo(hdc, dAInfo.a * dAInfo.divValueX / dAInfo.factorX, -dAInfo.newY);

	MoveToEx(hdc, dAInfo.b * dAInfo.divValueX / dAInfo.factorX, dAInfo.newY, NULL);
	LineTo(hdc, dAInfo.b * dAInfo.divValueX / dAInfo.factorX, -dAInfo.newY);
	SelectObject(hdc, oldPen);
	DeleteObject(newPen);
}
double GetDistance(int x1, int y1, int x2, int y2)
{
	return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
}

double CALLFUNC(int id, double x)
{
	switch (id)
	{
	case EXP_ID:
		return EXP(x);
	case LN_ID:
		return LN(x);
	case SQRT_ID:
		return SQRT(x);
	case XSINX_ID:
		return XSINX(x);
	case CUSTOM_ID:
		return CUSTOM(x);
	default: return 0.0;
	}
}

pair<double, double> GetFuncMinMax(int ID)
{
	pair<double, double> toReturn;
	toReturn.second = CALLFUNC(ID, dAInfo.b);
	toReturn.first = CALLFUNC(ID, dAInfo.a);
	double fx;

	for (double xArg = dAInfo.a * dAInfo.divValueX; xArg <= dAInfo.b * dAInfo.divValueX; xArg += 0.05)
	{
		fx = CALLFUNC(ID, xArg / dAInfo.divValueX);
		if (fx > toReturn.second) toReturn.second = fx;
		if (fx > CALLFUNC(ID, dAInfo.b)) { toReturn.second = MAXINT32; return toReturn; }

		if (fx < toReturn.first) toReturn.first = fx;
		if (fx < CALLFUNC(ID, dAInfo.a)) { toReturn.first = MININT32; return toReturn; }
	}
	if (toReturn.first != toReturn.first || isinf(toReturn.first)) toReturn.first = 0.0;
	if (toReturn.second != toReturn.second || isinf(toReturn.second)) toReturn.second = 0.0;
	return toReturn;
}

bool isNumber(char* str, int len)
{
	if (len > 0)
		if (str[0] == '-' || str[0] == '+' || isdigit(str[0])); else return false;
	if (len > 1)
	{
		for (int i = 1; i < len; i++)
			if (!isdigit(str[i])) return false;
		return true;
	}
	return true;
}