// Markus Buchholz, 2023

#include <stdafx.hpp>
#include "imgui_helper.hpp"
#include <tuple>
#include <thread>
#include <chrono>
#include <vector>
#include <math.h>
#include <list>
#include <numeric>
#include <memory>

//----------- system dynamic parameters --------------------

float m = 2.5f;
float R = 200.0f; // spring stretch (displacement from rest length)
float b = 0.13f;
float k = 1.0f;
float g = 9.81f;
float tx = 300.0f; // anchor point
float ty = 100.0f; // anchor point
float dt = 0.02f;

//-----------------------------------------------------------
// dx
float function1(float x, float y, float dx, float dy)
{

	return dx;
}

//-----------------------------------------------------------
// dy
float function2(float x, float y, float dx, float dy)
{

	return dy;
}

//-----------------------------------------------------------
// dvx
float function3(float x, float y, float dx, float dy)
{

	auto L = std::sqrt(std::pow(x - tx, 2) + std::pow(y - ty, 2));
	auto s = (x - tx) / L;
	auto S = L - R;

	return -(float)k * S * s / m - (float)dx * b / m;
}

//-----------------------------------------------------------
// dvy
float function4(float x, float y, float dx, float dy)
{

	auto L = std::sqrt(std::pow(x - tx, 2) + std::pow(y - ty, 2));
	auto c = (y - ty) / L;
	auto S = L - R;
	return g - (float)k * S * c / m - (float)dy * (float)b / m;
}

//-----------------------------------------------------------

int main(int argc, char const *argv[])
{
	ImVec4 clear_color = ImVec4(0.0f / 255.0, 0.0f / 255.0, 0.0f / 255.0, 1.00f);
	ImVec4 white_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	ImVec4 pink_color = ImVec4(245.0f / 255.0, 5.0f / 255.0, 150.0f / 255.0, 1.00f);
	ImVec4 blue_color = ImVec4(0.0f / 255.0, 0.0f / 255.0, 2550.0f / 255.0, 1.00f);
	int flag = true;

	int w = 800;
	int h = 800;
	std::string title = "Spring Forces";
	initImgui(w, h, title);

	float k11{0.0f}, k12{0.0f}, k13{0.0f}, k14{0.0f};
	float k21{0.0f}, k22{0.0f}, k23{0.0f}, k24{0.0f};
	float k31{0.0f}, k32{0.0f}, k33{0.0f}, k34{0.0f};
	float k41{0.0f}, k42{0.0f}, k43{0.0f}, k44{0.0f};

	// init values
	float x1 = 500.0f; // init position of mass in x
	float x2 = 400.0f; // init position of masss in y
	float x3 = 0.0f;   // init velocity
	float x4 = 0.0f;   // init velocity
	float t = 0.0f;	   // init time
	int ij = 0;
	std::list<ImVec2> tail;

	while (!glfwWindowShouldClose(window) && flag == true)
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		ImGuiWindowFlags window_flags = 0;
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoBackground;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::Begin("Simulation", nullptr, window_flags);
		ImDrawList *draw_list = ImGui::GetWindowDrawList();

		if (ij % 10 == 0)
		{
			t = t + dt;
		}
		if (tail.size() > 50)
		{
			tail.pop_front();
		}
		tail.push_back({x1, x2});

		k11 = function1(x1, x2, x3, x4);
		k12 = function2(x1, x2, x3, x4);
		k13 = function3(x1, x2, x3, x4);
		k14 = function4(x1, x2, x3, x4);

		k21 = function1(x1 + dt / 2 * k11, x2 + dt / 2 * k12, x3 + dt / 2 * k13, x4 + dt / 2 * k14);
		k22 = function2(x1 + dt / 2 * k11, x2 + dt / 2 * k12, x3 + dt / 2 * k13, x4 + dt / 2 * k14);
		k23 = function3(x1 + dt / 2 * k11, x2 + dt / 2 * k12, x3 + dt / 2 * k13, x4 + dt / 2 * k14);
		k24 = function4(x1 + dt / 2 * k11, x2 + dt / 2 * k12, x3 + dt / 2 * k13, x4 + dt / 2 * k14);

		k31 = function1(x1 + dt / 2 * k21, x2 + dt / 2 * k22, x3 + dt / 2 * k23, x4 + dt / 2 * k24);
		k32 = function2(x1 + dt / 2 * k21, x2 + dt / 2 * k22, x3 + dt / 2 * k23, x4 + dt / 2 * k24);
		k33 = function3(x1 + dt / 2 * k21, x2 + dt / 2 * k22, x3 + dt / 2 * k23, x4 + dt / 2 * k24);
		k34 = function4(x1 + dt / 2 * k21, x2 + dt / 2 * k22, x3 + dt / 2 * k23, x4 + dt / 2 * k24);

		k41 = function1(x1 + dt * k31, x2 + dt * k32, x3 + dt * k33, x4 + dt * k34);
		k42 = function2(x1 + dt * k31, x2 + dt * k32, x3 + dt * k33, x4 + dt * k34);
		k43 = function3(x1 + dt * k31, x2 + dt * k32, x3 + dt * k33, x4 + dt * k34);
		k44 = function4(x1 + dt * k31, x2 + dt * k32, x3 + dt * k33, x4 + dt * k34);

		x1 = x1 + dt / 6.0 * (k11 + 2 * k21 + 2 * k31 + k41);
		x2 = x2 + dt / 6.0 * (k12 + 2 * k22 + 2 * k32 + k42);
		x3 = x3 + dt / 6.0 * (k13 + 2 * k23 + 2 * k33 + k43);
		x4 = x4 + dt / 6.0 * (k14 + 2 * k24 + 2 * k34 + k44);

		ij++;

		draw_list->AddCircleFilled({tx, ty}, 3.0f, ImColor(white_color));
		draw_list->AddCircleFilled({x1, x2}, 20.0f, ImColor(pink_color));
		draw_list->AddLine({tx, ty}, {x1, x2}, ImColor(pink_color), 1.0f);

		for (auto &ii : tail)
		{
			draw_list->AddCircleFilled({ii.x, ii.y}, 2.0f, ImColor(blue_color));
		}

		// std::this_thread::sleep_for(std::chrono::milliseconds(50));

		ImGui::End();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	termImgui();
	return 0;
}