#include "Control.h"

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}



void GUI::setData(int16_t aSpeed, int16_t aSWA, int16_t tSpeed, int16_t tSWA){

	for (int i = 0; i < 20; i++)
	{
		targetSpeed[i] = targetSpeed[i+1];
		actualSpeed[i] = actualSpeed[i + 1];
		targetSWA[i] = targetSWA[i + 1];
		actualSWA[i] = actualSWA[i + 1];
	}
	targetSpeed[19] = tSpeed;
	actualSpeed[19] = aSpeed;
	targetSWA[19] = tSWA;
	actualSWA[19] = aSWA;
}

void GUI::initOpenGL()
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		printf("if (!glfwInit()) Error! \n");

	// Decide GL+GLSL versions
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// Create window with graphics context
	window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
	if (window == NULL)
		printf("if (window == NULL) error \n");
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Initialize OpenGL loader
	bool err = gl3wInit() != 0;

	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		printf("if (err) Error \n");
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();


	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void GUI::destroyOpenGL()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void GUI::work(Control& ctrl)
{
	glfwPollEvents(); 	// Poll and handle events (inputs, window resize, etc.)

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Client side GUI");                          // Create a window called "Hello, world!" and append into it.

	ImGui::Checkbox("selfDrive mode", &collect_data_from_GUI);      // Edit bools storing our window open/close state
	ImGui::SameLine();
	if (true == collect_data_from_GUI)                            // Buttons return true when clicked (most widgets return true when edited/activated)
	{
		ctrl.engageSelfDrive(true);

		ImGui::Text("engaged");
	}
	else
	{
		ctrl.engageSelfDrive(false);

		ImGui::Text("disengaged");
	}

	ImGui::InputFloat("Target speed", &readGuiSpeed);
	ImGui::InputFloat("Target SWA", &readGuiSWA);
	ImGui::Text("Target speed is %f km/h and target SWA is %f mrad", readGuiSpeed, readGuiSWA);
	ctrl.TargetDatasFromGUI(readGuiSpeed, readGuiSWA);

	ImPlot::SetNextPlotLimits(0, 20, -20, 200, ImGuiCond_Always);
	if (ImPlot::BeginPlot("Speed", "time", "speed")){
		ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
		ImPlot::PlotLine("Actual Speed", scale, actualSpeed, 20);
		ImPlot::SetNextMarkerStyle(ImPlotMarker_Diamond);
		ImPlot::PlotLine("Target Speed", scale, targetSpeed, 20);
		ImPlot::EndPlot();
	}

	ImPlot::SetNextPlotLimits(0, 20, -1500, 1500, ImGuiCond_Always);
	if (ImPlot::BeginPlot("SWA", "time", "mrad")) {
		ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
		ImPlot::PlotLine("Actual SWA", scale, actualSWA, 20);
		ImPlot::SetNextMarkerStyle(ImPlotMarker_Diamond);
		ImPlot::PlotLine("Target SWA", scale, targetSWA, 20);
		ImPlot::EndPlot();
	}
	ImGui::End();
	


	// Rendering
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
}