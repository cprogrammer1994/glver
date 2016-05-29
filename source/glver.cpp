#include "Python.h"

#include <Windows.h>
#include <cstdio>

#define GL_VENDOR		0x1F00
#define GL_RENDERER		0x1F01
#define GL_VERSION		0x1F02
#define GL_EXTENSIONS	0x1F03

PIXELFORMATDESCRIPTOR pfd = {
	sizeof(PIXELFORMATDESCRIPTOR),	// nSize
	1,								// nVersion
	PFD_SUPPORT_OPENGL,				// dwFlags
	0,								// iPixelType
	24,								// cColorBits
	0,								// cRedBits
	0,								// cRedShift
	0,								// cGreenBits
	0,								// cGreenShift
	0,								// cBlueBits
	0,								// cBlueShift
	0,								// cAlphaBits
	0,								// cAlphaShift
	0,								// cAccumBits
	0,								// cAccumRedBits
	0,								// cAccumGreenBits
	0,								// cAccumBlueBits
	0,								// cAccumAlphaBits
	0,								// cDepthBits
	0,								// cStencilBits
	0,								// cAuxBuffers
	0,								// iLayerType
	0,								// bReserved
	0,								// dwLayerMask
	0,								// dwVisibleMask
	0								// dwDamageMask
};

const char * error_message = "No error";

const char * VERSION = 0;
const char * VENDOR = 0;
const char * RENDERER = 0;
const char * EXTENSIONS = 0;

typedef const char * (WINAPI * My_glGetString) (unsigned name);
typedef int (WINAPI * My_ChoosePixelFormat) (HDC hdc, const PIXELFORMATDESCRIPTOR * ppfd);
typedef BOOL (WINAPI * My_SetPixelFormat) (HDC hdc, int iPixelFormat, const PIXELFORMATDESCRIPTOR * ppfd);

typedef HGLRC (WINAPI * My_wglCreateContext) (HDC hdc);
typedef BOOL (WINAPI * My_wglMakeCurrent) (HDC hdc, HGLRC hglrc);

My_ChoosePixelFormat my_ChoosePixelFormat;
My_SetPixelFormat my_SetPixelFormat;
My_glGetString my_glGetString;

My_wglCreateContext my_wglCreateContext;
My_wglMakeCurrent my_wglMakeCurrent;

void initialize() {
	HMODULE gdi32 = LoadLibrary("gdi32.dll");

	if (gdi32) {
		my_ChoosePixelFormat = (My_ChoosePixelFormat)GetProcAddress(gdi32, "ChoosePixelFormat");
		if (!my_ChoosePixelFormat) {
			error_message = "ChoosePixelFormat is not available in gdi32.dll";
			return;
		}

		my_SetPixelFormat = (My_SetPixelFormat)GetProcAddress(gdi32, "SetPixelFormat");
		if (!my_SetPixelFormat) {
			error_message = "SetPixelFormat is not available in gdi32.dll";
			return;
		}
	} else {
		error_message = "gdi32.dll is missing!";
		return;
	}

	HMODULE opengl32 = LoadLibrary("opengl32.dll");

	if (opengl32) {
		my_wglCreateContext = (My_wglCreateContext)GetProcAddress(opengl32, "wglCreateContext");
		if (!my_wglCreateContext) {
			error_message = "wglCreateContext is not available in opengl32.dll";
			return;
		}

		my_wglMakeCurrent = (My_wglMakeCurrent)GetProcAddress(opengl32, "wglMakeCurrent");
		if (!my_wglMakeCurrent) {
			error_message = "wglMakeCurrent is not available in opengl32.dll";
			return;
		}

		my_glGetString = (My_glGetString)GetProcAddress(opengl32, "glGetString");
		if (!my_glGetString) {
			error_message = "glGetString is not available in opengl32.dll";
			return;
		}
	} else {
		error_message = "opengl32.dll is missing!";
	}

	HMODULE hinst = GetModuleHandle(0);

	if (!hinst) {
		error_message = "GetModuleHandle";
		return;
	}

	WNDCLASS wndClass = {
		CS_OWNDC,				// style
		DefWindowProc,			// lpfnWndProc
		0,						// cbClsExtra
		0,						// cbWndExtra
		hinst,					// hInstance
		0,						// hIcon
		0,						// hCursor
		0,						// hbrBackground
		0,						// lpszMenuName
		"TestOpenGL",			// lpszClassName
	};

	if (!RegisterClass(&wndClass)) {
		error_message = "RegisterClass";
		return;
	}

	HWND hwnd = CreateWindow(
		"TestOpenGL",			// lpClassName
		0,						// lpWindowName
		0,						// dwStyle
		0,						// x
		0,						// y
		0,						// nWidth
		0,						// nHeight
		0,						// hWndParent
		0,						// hMenu
		hinst,					// hInstance
		0						// lpParam
	);

	if (!hwnd) {
		error_message = "CreateWindow";
		return;
	}

	HDC hdc = GetDC(hwnd);

	if (!hdc) {
		error_message = "GetDC";
		return;
	}

	int pixelformat = my_ChoosePixelFormat(hdc, &pfd);

	if (!pixelformat) {
		error_message = "ChoosePixelFormat";
		return;
	}

	if (!my_SetPixelFormat(hdc, pixelformat, &pfd)) {
		error_message = "SetPixelFormat";
		return;
	}

	HGLRC hglrc = my_wglCreateContext(hdc);

	if (!hglrc) {
		error_message = "wglCreateContext";
		return;
	}

	if (!my_wglMakeCurrent(hdc, hglrc)) {
		error_message = "wglMakeCurrent";
		return;
	}

	VERSION = my_glGetString(GL_VERSION);
	VENDOR = my_glGetString(GL_VENDOR);
	RENDERER = my_glGetString(GL_RENDERER);
	EXTENSIONS = my_glGetString(GL_EXTENSIONS);

	return;
}

PyObject * vendor(PyObject * self, PyObject * args) {
	if (!PyArg_ParseTuple(args, ":vendor")) {
		return 0;
	}
	return PyUnicode_FromString(VENDOR ? VENDOR : "UNKNOWN");
}

PyObject * renderer(PyObject * self, PyObject * args) {
	if (!PyArg_ParseTuple(args, ":renderer")) {
		return 0;
	}
	return PyUnicode_FromString(RENDERER ? RENDERER : "UNKNOWN");
}

PyObject * version(PyObject * self, PyObject * args) {
	if (!PyArg_ParseTuple(args, ":version")) {
		return 0;
	}
	return PyUnicode_FromString(VERSION ? VERSION : "UNKNOWN");
}

PyObject * extensions(PyObject * self, PyObject * args) {
	if (!PyArg_ParseTuple(args, ":extensions")) {
		return 0;
	}
	return PyUnicode_FromString(EXTENSIONS ? EXTENSIONS : "UNKNOWN");
}

PyObject * error(PyObject * self, PyObject * args) {
	if (!PyArg_ParseTuple(args, ":error")) {
		return 0;
	}
	return PyUnicode_FromString(error_message);
}

PyObject * info(PyObject * self, PyObject * args) {
	if (!PyArg_ParseTuple(args, ":info")) {
		return 0;
	}
	PyObject * tuple = PyTuple_New(4);
	PyTuple_SetItem(tuple, 0, PyUnicode_FromString(VENDOR ? VENDOR : "UNKNOWN"));
	PyTuple_SetItem(tuple, 1, PyUnicode_FromString(RENDERER ? RENDERER : "UNKNOWN"));
	PyTuple_SetItem(tuple, 2, PyUnicode_FromString(VERSION ? VERSION : "UNKNOWN"));
	PyTuple_SetItem(tuple, 3, PyUnicode_FromString(EXTENSIONS ? EXTENSIONS : "UNKNOWN"));
	return tuple;
}

static PyMethodDef methods[] = {
	{"vendor", vendor, METH_VARARGS, 0},
	{"renderer", renderer, METH_VARARGS, 0},
	{"version", version, METH_VARARGS, 0},
	{"extensions", extensions, METH_VARARGS, 0},
	{"error", error, METH_VARARGS, 0},
	{"info", info, METH_VARARGS, 0},
	{0, 0},
};

static struct PyModuleDef moduledef = {PyModuleDef_HEAD_INIT, "glver", 0, -1, methods, 0, 0, 0, 0};

extern "C" {
	PyObject * PyInit_glver();
}

PyObject * PyInit_glver() {
	initialize();

	PyObject * module = PyModule_Create(&moduledef);
	PyModule_AddStringConstant(module, "AUTHOR_NAME", "Szabolcs Dombi");
	PyModule_AddStringConstant(module, "AUTHOR_EMAIL", "cprogrammer1994@gmail.com");
	return module;
}
