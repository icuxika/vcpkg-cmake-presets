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
enum Type { VIDEO = 1, AUDIO = 2 };

using VideoHandlerFunction = std::function<void(AVFrame *)>;
using AudioHandlerFunction = std::function<void(AVFrame *)>;

class DemuxUtil {
  public:
	static DemuxUtil &GetInstance();

	int openFile(const char *url);
	int startDecode(int type);

	VideoHandlerFunction VideoHandler;
	AudioHandlerFunction AudioHandler;
	bool EnableVideoDecode;
	bool EnableAudioDecode;
	int Width, Height;

  private:
	DemuxUtil();
	~DemuxUtil();
	DemuxUtil(const DemuxUtil &) = delete;
	DemuxUtil &operator=(const DemuxUtil &) = delete;

	int openCodecContext(int *streamIndex, AVCodecContext **codecContext,
		enum AVMediaType mediaType);
	int decodePacket(
		AVCodecContext *codecContext, const AVPacket *packet, AVFrame *frame);
	int outputVideoFrame(AVFrame *frame);
	int outputAudioFrame(AVFrame *frame);
	void yuv2file(AVFrame *frame);
	void yuv2rgba(AVFrame *frame);

	AVFormatContext *FormatContext = nullptr;
	int TotalMilliseconds = 0;
	enum AVPixelFormat PixFmt;

	int VideoStreamIndex = -1;
	AVCodecContext *VideoCodecContext = nullptr;
	AVStream *VideoStream;

	int AudioStreamIndex = -1;
	AVCodecContext *AudioCodecContext = nullptr;
	AVStream *AudioStream;

	AVFrame *Frame;
	AVPacket *Packet;

	int VideoFrameCount = 0;
	uint8_t *VideoDstData[4] = {nullptr};
	int VideoDstLinesize[4];
	int VideoDstBufsize;
};
} // namespace av
