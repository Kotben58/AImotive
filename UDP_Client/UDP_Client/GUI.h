#ifndef GUI_H_
#define GUI_H_

#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "implot_internal.h"
#include <stdio.h>

#include <GL/gl3w.h>

#include <GLFW/glfw3.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

struct ScrollingBuffer {
	int MaxSize;
	int Offset;
	ImVector<ImVec2> Data;
	ScrollingBuffer(int max_size = 2000) {
		MaxSize = max_size;
		Offset = 0;
		Data.reserve(MaxSize);
	}
	void AddPoint(float x, float y) {
		if (Data.size() < MaxSize)
			Data.push_back(ImVec2(x, y));
		else {
			Data[Offset] = ImVec2(x, y);
			Offset = (Offset + 1) % MaxSize;
		}
	}
	void Erase() {
		if (Data.size() > 0) {
			Data.shrink(0);
			Offset = 0;
		}
	}
};


class GUI
{
//	const float scale[20] = { 0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,11.0,12.0,13.0,14.0,15.0,16.0,17.0,18.0,19.0 };

	const int16_t scale[20] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
	bool collect_data_from_GUI;
	int16_t targetSpeed[20];
	int16_t actualSpeed[20];
	int16_t targetSWA[20];
	int16_t actualSWA[20];
public:
	float readGuiSpeed;
	float readGuiSWA;
	GLFWwindow* window = NULL;

	GUI() : collect_data_from_GUI(false) {}

	void setData(int16_t aSpeed, int16_t aSWA, int16_t tSpeed, int16_t tSWA);

	void initOpenGL();

	void destroyOpenGL();

	void work(Control& ctrl);
};

#endif