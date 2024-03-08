#include "alpha-av-core.h"

namespace av {
AlphaAVCore::AlphaAVCore() {
	AlphaAVDemuxContext.reset(new AlphaAVDemux());
	AlphaAVDecodeContext.reset(new AlphaAVDecode());
}
AlphaAVCore::~AlphaAVCore() {
	AlphaAVDecodeContext.reset();
	AlphaAVDemuxContext.reset();
}
AlphaAVCore &AlphaAVCore::GetInstance() {
	static AlphaAVCore instance;
	return instance;
}
int AlphaAVCore::openFile(const char *url) {
	PRINTLN("打开文件");
	AlphaAVDemuxContext->openFile(url);
	PRINTLN("检查硬件解码支持");
	if (AlphaAVDecodeContext->EnableHwDecode) {
		if (AlphaAVDecodeContext->enableHwDeviceSupport("cuda") < 0) {
			PRINTLN("不支持硬件解码，将进行软件解码");
		}
	}
	PRINTLN("打开视频解码器");
	AlphaAVDecodeContext->openVideoCodecContext(
		AlphaAVDemuxContext->FormatContext);
	PRINTLN("打开音频解码器");
	AlphaAVDecodeContext->openAudioCodecContext(
		AlphaAVDemuxContext->FormatContext);
	return 0;
}
int AlphaAVCore::play() {
	PRINTLN("开始解码");
	AlphaAVDecodeContext->startDecode(AlphaAVDemuxContext->FormatContext);
	return 0;
}
} // namespace av