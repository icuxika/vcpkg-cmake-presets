#pragma once

#include <fstream>
#include <functional>
#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/channel_layout.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
}

namespace av {
class AlphaAVDemux {

  public:
	AlphaAVDemux();
	~AlphaAVDemux();

	int openFile(const char *url);

	AVFormatContext *FormatContext = nullptr;
	int TotalMilliseconds = 0;
};
} // namespace av