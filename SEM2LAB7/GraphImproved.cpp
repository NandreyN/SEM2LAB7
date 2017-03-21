#undef UNICODE

#define EXP(x) exp(x)
#define LN(x) log(x)
#define SQRT(x) sqrt(x)
#define XSINX(x) (double)x*sin(x)
#define CUSTOM(x) (double)x*x

#define EXP_ID 0
#define LN_ID 1
#define SQRT_ID 2
#define XSINX_ID 3
#define CUSTOM_ID 4

#define IDR_MENU 101
#define ID_SETTINGS_GRAPHSETTINGS 40001
#define IDD_GRAPHDIALOG 102
#define IDB_GRAPHAPPLY 1003
#define IDB_GRAPHCANCEL 1004
#define IDE_GRAPHEA 1001
#define IDE_GRAPHEB 1002

#include <windows.h>
#include  <math.h>
#include <cmath>
#include <string>
#include <limits>
using namespace std;

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

struct DrawAreaInfo
{
	double xPoints, yPoints, divValueX, divValueY, newX, newY, a, b;
};
static DrawAreaInfo dAInfo;

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
DrawAreaInfo GetAreaInfo(int x, int y, int a, int b, int ID);
void Draw(HDC& hdc, int x, int y, int funcID);
POINT ConvertCoordinates(int x, int y, int widthOld, int heightOld);
double GetDistance(int x1, int y1, int x2, int y2);
double CALLFUNC(int id, double x);
double GetFuncMax(int ID);
double GetFuncMin(int ID);

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

	while (GetMessage(&msg, NULL, 0, 0))
	{
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
	wndclass.hCursor = LoadCursor(NULL, IDC_CROSS);
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

BOOL CALLBACK GraphBoxHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	string text;
	static pair<int, int> tempFrontiers;

	switch (message)
	{
	case WM_INITDIALOG:
		tempFrontiers.first = tempFrontiers.second = MAXINT32;
		text = "a = " + to_string(dAInfo.a);
		SetDlgItemText(hwnd, IDE_GRAPHEA, text.data());
		text = "b = " + to_string(dAInfo.b);
		SetDlgItemText(hwnd, IDE_GRAPHEB, text.data());
		break;
	case WM_COMMAND:
	{
		switch (LOWORD(wparam))
		{
		case IDB_GRAPHAPPLY:
			if (tempFrontiers.first >= tempFrontiers.second || tempFrontiers.first >= dAInfo.b) EndDialog(hwnd , 0);
			if (tempFrontiers.first != MAXINT32)
				dAInfo.a = tempFrontiers.first;
			if (tempFrontiers.second != MAXINT32)
				dAInfo.b = tempFrontiers.second;

			InvalidateRect(GetParent(hwnd), NULL, true);
			EndDialog(hwnd, 0);
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
		}
		break;
	}
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	static int x, y, a, b;
	static HDC hdc, buffer;
	RECT client;

	PAINTSTRUCT ps;
	static int funcID;

	RECT clientRect;
	static Graph gr;
	switch (message)
	{
	case WM_CREATE:
		gr.a = 1; gr.b = 1; gr.c = 1;
		funcID = XSINX_ID;
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
		switch (wparam)
		{
		case ID_SETTINGS_GRAPHSETTINGS:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_GRAPHDIALOG), hwnd, GraphBoxHandler);
			break;
		}
		break;
	}
	case WM_PAINT:
	{
		GetAreaInfo(x, y, dAInfo.a, dAInfo.b, funcID);
		RECT Rect; GetClientRect(hwnd, &Rect);

		hdc = BeginPaint(hwnd, &ps);
		SetMapMode(hdc, MM_ANISOTROPIC);
		SetWindowExtEx(hdc, x, y, NULL);
		SetViewportExtEx(hdc, x, -y, NULL);
		SetViewportOrgEx(hdc, x / 2, y / 2, NULL);

		Draw(hdc, x, y, funcID);
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
	HWND hwnd;
	hwnd = CreateWindow(
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


	if (!hwnd)
		return FALSE;
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
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

DrawAreaInfo GetAreaInfo(int x, int y, int aa, int bb, int ID)
{
	DrawAreaInfo di;

	dAInfo.a = aa; dAInfo.b = bb;
	int xPoints, yPoints, divValueX, divValueY, a, b; // Кол-во делений

	if (sgn(aa) != sgn(bb))
		xPoints = abs(bb - aa);
	else
		xPoints = (abs(bb) > abs(aa)) ? bb + 3 : aa + 3;

	divValueX = x / (xPoints * 2);
	dAInfo.divValueX = divValueX;
	double min = ceil(abs(GetFuncMin(ID))), max = ceil(abs(GetFuncMax(ID)));
	yPoints = (max > min) ? max : min;//max;

	divValueY = y / (yPoints * 2);
	int newX, newY; newX = x / 2; newY = y / 2;

	dAInfo.divValueY = divValueY; dAInfo.newX = newX; dAInfo.newY = newY; dAInfo.xPoints = xPoints; dAInfo.yPoints = yPoints;
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

	for (int xx = 0; xx <= dAInfo.newX; xx += dAInfo.divValueX)
	{
		MoveToEx(hdc, xx, 5, NULL);
		LineTo(hdc, xx, -5);
		MoveToEx(hdc, -xx, 5, NULL);
		LineTo(hdc, -xx, -5);
	}

	for (int yy = 0; yy <= dAInfo.newY; yy += dAInfo.divValueY)
	{
		MoveToEx(hdc, 5, yy, NULL);
		LineTo(hdc, -5, yy);
		MoveToEx(hdc, 5, -yy, NULL);
		LineTo(hdc, -5, -yy);
	}
	MoveToEx(hdc, dAInfo.a, 0, NULL);

	double fx = CALLFUNC(ID, dAInfo.a * dAInfo.divValueX);

	MoveToEx(hdc, dAInfo.a * dAInfo.divValueX, fx * dAInfo.divValueY, NULL);

	for (double xArg = dAInfo.a * dAInfo.divValueX; xArg <= dAInfo.b * dAInfo.divValueX; xArg += 0.05)
	{
		fx = CALLFUNC(ID, xArg / dAInfo.divValueX);
		if (fx == fx)
			SetPixel(hdc, xArg, fx * dAInfo.divValueY, RGB(0, 0, 0));
	}

	newPen = CreatePen(PS_DOT, 1, BLACK_PEN);
	oldPen = (HPEN)SelectObject(hdc, newPen);
	MoveToEx(hdc, dAInfo.a * dAInfo.divValueX, dAInfo.newY, NULL);
	LineTo(hdc, dAInfo.a * dAInfo.divValueX, -dAInfo.newY);

	MoveToEx(hdc, dAInfo.b * dAInfo.divValueX, dAInfo.newY, NULL);
	LineTo(hdc, dAInfo.b * dAInfo.divValueX, -dAInfo.newY);
	SelectObject(hdc, oldPen);
	DeleteObject(newPen);
}
double GetDistance(int x1, int y1, int x2, int y2)
{
	return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
}

double CALLFUNC(int id, double x)
{
	double val;
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

double GetFuncMax(int ID)
{
	double max = MININT32;
	double fx = 0;
	for (double xArg = dAInfo.a * dAInfo.divValueX; xArg <= dAInfo.b * dAInfo.divValueX; xArg += 0.05)
	{
		fx = CALLFUNC(ID, xArg / dAInfo.divValueX);
		if (fx > max) max = fx;
	}
	return max;
}

double GetFuncMin(int ID)
{
	double min = MAXINT32;
	double fx = 0;
	for (double xArg = dAInfo.a * dAInfo.divValueX; xArg <= dAInfo.b * dAInfo.divValueX; xArg += 0.05)
	{
		fx = CALLFUNC(ID, xArg / dAInfo.divValueX);
		if (fx < min) min = fx;
	}
	return min;
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