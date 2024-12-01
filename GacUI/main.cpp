#define GAC_HEADER_USE_NAMESPACE
#include "HelloWorld.h"
#include <GacUI.h>

using namespace vl::collections;
using namespace vl::stream;

void GuiMain() {
	{
		FileStream fileStream(L"HelloWorld.bin", FileStream::ReadOnly);
		GetResourceManager()->LoadResourceOrPending(fileStream);
	}
	helloworld::MainWindow window;

	auto label = new GuiLabel(theme::ThemeName::Label);
	label->SetText(L"你好，世界！");
	{
		FontProperties font;
		font.fontFamily = L"STKaiti";
		font.size = 32;
		font.antialias = true;
		label->SetFont(font);
	}
	window.AddChild(label);

	window.MoveToScreenCenter();
	GetApplication()->Run(&window);
}
