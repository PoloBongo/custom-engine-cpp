#include "first_app.h"

namespace lve {
	void FirstApp::Run() {
		while (!lveWindow.ShouldClose()) {
			glfwPollEvents();
		}
	}
}