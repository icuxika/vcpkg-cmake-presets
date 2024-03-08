#pragma once

#include "macros.h"
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

class AlphaAVDecode {
  public:
	AlphaAVDecode();
	~AlphaAVDecode();

	// 检查该硬件类型是否支持硬件解码
	int enableHwDeviceSupport(const char *type);
	enum AVHWDeviceType HWDeviceType;

	int openVideoCodecContext(AVFormatContext *formatContext);
	int openAudioCodecContext(AVFormatContext *formatContext);

	int Width, Height;
	double VideoFrameRate;
	enum AVPixelFormat PixFmt;

	int VideoStreamIndex = -1;
	AVCodecContext *VideoCodecContext = nullptr;
	AVStream *VideoStream;
	bool EnableHwDecode = false;
	enum AVPixelFormat HwPixFmt;
	AVBufferRef *HwDeviceCtx = nullptr;

	int AudioStreamIndex = -1;
	AVCodecContext *AudioCodecContext = nullptr;
	AVStream *AudioStream;

	int startDecode(AVFormatContext *formatContext);
	int decodePacket(AVCodecContext *codecContext, const AVPacket *packet,
		AVFrame *frame, AVFrame *hwFrame);
	int outputVideoFrame(AVFrame *frame);
	int outputAudioFrame(AVFrame *frame);
	AVPacket *Packet;
	AVFrame *Frame;
	AVFrame *HwFrame;
	int VideoFrameCount = 0;

	VideoHandlerFunction VideoHandler;
	AudioHandlerFunction AudioHandler;

  private:
	int openCodecContext(AVFormatContext *formatContext, int *streamIndex,
		AVCodecContext **codecContext, enum AVMediaType mediaType);

	enum AVPixelFormat getHwFormat(
		AVCodecContext *ctx, const enum AVPixelFormat *pixFmts);
	int hwDecoderInit(AVCodecContext *ctx, const enum AVHWDeviceType type);
};
} // namespace av