#include "alpha-av-core.h"

int main(int argc, char *argv[]) {
	const char *url = "C:\\Users\\icuxika\\Desktop\\test.mp4";
	av::AlphaAVCore::GetInstance().openFile(url);
	av::AlphaAVCore::GetInstance().AlphaAVDecodeContext->VideoDataHandler =
		[](std::vector<uint8_t> buffer) {};
	av::AlphaAVCore::GetInstance().AlphaAVDecodeContext->AudioDataHandler =
		[](std::vector<uint8_t> buffer) {};
	av::AlphaAVCore::GetInstance().play();
	return 0;
}