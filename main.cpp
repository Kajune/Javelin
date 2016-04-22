#include "Javelin.h"

using namespace Javelin;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPInst, LPSTR lpC, int nC) {
	if (Application::Initialize("Javelin", 640, 480, true, 1)){
		Application::Cleanup();
		return -1;
	}
	while (Application::MainLoop() == 0) {
		Application::ClearScreen(COLOR(0.0f, 0.0f, 1.0f, 1.0f));
		Application::SetWindowTitle(std::to_string(Javelin::Application::GetFPS()));
		Application::Present();
	}
	Application::Cleanup();
	return 0;
}
