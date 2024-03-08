#include "alpha-av-demux.h"

namespace av {
AlphaAVDemux::AlphaAVDemux() {}
AlphaAVDemux::~AlphaAVDemux() {
	avformat_close_input(&FormatContext);
	avformat_free_context(FormatContext);
}

int AlphaAVDemux::openFile(const char *url) {
	AVDictionary *options = nullptr;
	// 设置rtsp流以tcp协议打开
	av_dict_set(&options, "rtsp_transport", "tcp", 0);
	// 设置网络延时
	av_dict_set(&options, "max_delay", "500", 0);

	int ret = 0;
	// 打开视频文件流，读取文件头的一些信息
	ret = avformat_open_input(&FormatContext, url, nullptr, &options);
	if (ret < 0) {
		std::cout << "无法打开文件" << std::endl;
		return ret;
	}
	// 获取流信息
	ret = avformat_find_stream_info(FormatContext, nullptr);
	if (ret < 0) {
		std::cout << "无法获取流信息" << std::endl;
		return ret;
	}
	// 打印视频流信息
	av_dump_format(FormatContext, 0, url, 0);

	std::cout << "------------------------------------------------------"
			  << std::endl;
	// 获取总时长（毫秒）
	TotalMilliseconds = FormatContext->duration / (AV_TIME_BASE / 1000);
	std::cout << "视频总时长（毫秒）" << TotalMilliseconds << std::endl;
	// 获取总时长（秒）
	int totalSeconds = FormatContext->duration / AV_TIME_BASE;
	std::cout << "视频总时长（秒）" << totalSeconds << std::endl;
	return ret;
}
} // namespace av