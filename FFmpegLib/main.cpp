#include "alpha-av-core.h"

int main(int argc, char *argv[]) {
	const char *url = "C:\\Users\\icuxika\\Desktop\\test.mp4";
	av::AlphaAVCore::GetInstance().openFile(url);
	av::AlphaAVCore::GetInstance().AlphaAVDecodeContext->VideoHandler =
		[](AVFrame *frame) {};
	av::AlphaAVCore::GetInstance().AlphaAVDecodeContext->AudioHandler =
		[](AVFrame *frame) {};
	av::AlphaAVCore::GetInstance().play();
	return 0;
}