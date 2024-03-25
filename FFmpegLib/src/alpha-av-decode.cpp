#include "alpha-av-decode.h"

namespace av {
AlphaAVDecode::AlphaAVDecode() {}
AlphaAVDecode::~AlphaAVDecode() {
	av_packet_free(&Packet);
	av_frame_free(&Frame);
	av_frame_free(&HwFrame);
	avcodec_free_context(&VideoCodecContext);
	avcodec_free_context(&AudioCodecContext);
	av_buffer_unref(&HwDeviceCtx);
}

int AlphaAVDecode::enableHwDeviceSupport(const char *type) {
	HWDeviceType = av_hwdevice_find_type_by_name(type);
	if (HWDeviceType == AV_HWDEVICE_TYPE_NONE) {
		PRINTLN("不支持硬件设备->" << type);
		PRINTLN("下面是支持的硬件设备");
		while ((HWDeviceType = av_hwdevice_iterate_types(HWDeviceType)) !=
			AV_HWDEVICE_TYPE_NONE) {
			PRINTLN("->" << av_hwdevice_get_type_name(HWDeviceType));
		}
		EnableHwDecode = false;
		return -1;
	}
	PRINTLN("硬件解码使用 " << av_hwdevice_get_type_name(HWDeviceType));
	EnableHwDecode = true;
	return 0;
}
int AlphaAVDecode::openVideoCodecContext(AVFormatContext *formatContext) {
	if (openCodecContext(formatContext, &VideoStreamIndex, &VideoCodecContext,
			AVMEDIA_TYPE_VIDEO) >= 0) {
		PRINTLN("------------------------------------------------------");
		PRINTLN("视频宽度" << VideoCodecContext->width);
		PRINTLN("视频高度" << VideoCodecContext->height);
		PRINTLN("视频像素格式"
			<< av_get_pix_fmt_name((AVPixelFormat)VideoCodecContext->pix_fmt));
		AVRational framerate = VideoCodecContext->framerate;
		double fps =
			framerate.den == 0 ? 0 : (double)framerate.num / framerate.den;
		PRINTLN("视频帧速率" << fps);

		Width = VideoCodecContext->width;
		Height = VideoCodecContext->height;
		PixFmt = VideoCodecContext->pix_fmt;
		VideoFrameRate = fps;
	}
	return -1;
}
int AlphaAVDecode::openAudioCodecContext(AVFormatContext *formatContext) {
	return openCodecContext(formatContext, &AudioStreamIndex,
		&AudioCodecContext, AVMEDIA_TYPE_AUDIO);
}
int AlphaAVDecode::openCodecContext(AVFormatContext *formatContext,
	int *streamIndex, AVCodecContext **codecContext,
	enum AVMediaType mediaType) {

	int ret, index;
	AVStream *stream;
	const AVCodec *codec = nullptr;

	ret = av_find_best_stream(formatContext, mediaType, -1, -1, &codec, 0);
	if (ret < 0) {
		PRINTLN("无法获取流->" << av_get_media_type_string(mediaType));
		return ret;
	}
	index = ret;
	stream = formatContext->streams[index];

	// 寻找解码器
	if (EnableHwDecode && mediaType == AVMEDIA_TYPE_VIDEO) {
		// 硬件解码
		for (int i = 0;; i++) {
			const AVCodecHWConfig *config = avcodec_get_hw_config(codec, i);
			if (!config) {
				PRINTLN(codec->name << "不支持设备 "
									<< av_hwdevice_get_type_name(HWDeviceType));
				return -1;
			}
			if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
				config->device_type == HWDeviceType) {
				HwPixFmt = config->pix_fmt;
				break;
			}
		}
	} else {
		// 软件解码
		codec = avcodec_find_decoder(stream->codecpar->codec_id);
		if (!codec) {
			PRINTLN("找不到解码器 " << av_get_media_type_string(mediaType));
			return -1;
		}
	}

	*codecContext = avcodec_alloc_context3(codec);
	if (!*codecContext) {
		PRINTLN(
			"无法分配AVCodecContext " << av_get_media_type_string(mediaType));
		return -1;
	}

	if ((ret = avcodec_parameters_to_context(*codecContext, stream->codecpar)) <
		0) {
		PRINTLN(
			"无法复制AVCodecContext " << av_get_media_type_string(mediaType));
		return ret;
	}

	if (EnableHwDecode && mediaType == AVMEDIA_TYPE_VIDEO) {
		(*codecContext)->opaque = this;
		(*codecContext)->get_format = [](AVCodecContext * ctx,
			const enum AVPixelFormat *pixFmts) -> enum AVPixelFormat {
			auto *instance = static_cast<AlphaAVDecode *>(ctx->opaque);
			return instance->getHwFormat(ctx, pixFmts);
		};
		ret = hwDecoderInit(*codecContext, HWDeviceType);
		if (ret < 0) {
			return ret;
		}
	}

	if ((ret = avcodec_open2(*codecContext, codec, nullptr)) < 0) {
		PRINTLN("无法打开解码器 " << av_get_media_type_string(mediaType));
		return ret;
	}
	*streamIndex = index;

	return ret;
} // namespace av

enum AVPixelFormat AlphaAVDecode::getHwFormat(
	AVCodecContext *ctx, const enum AVPixelFormat *pixFmts) {
	const enum AVPixelFormat *p;

	for (p = pixFmts; *p != -1; p++) {
		if (*p == HwPixFmt)
			return *p;
	}
	PRINTLN("无法找到AVPixelFormat");
	return AV_PIX_FMT_NONE;
}

int AlphaAVDecode::hwDecoderInit(
	AVCodecContext *ctx, const enum AVHWDeviceType type) {
	int err = 0;

	if ((err = av_hwdevice_ctx_create(
			 &HwDeviceCtx, type, nullptr, nullptr, 0)) < 0) {
		PRINTLN("无法创建指定的硬件设备");
		return err;
	}
	ctx->hw_device_ctx = av_buffer_ref(HwDeviceCtx);

	return err;
}

int AlphaAVDecode::startDecode(AVFormatContext *formatContext) {
	int ret;
	Packet = av_packet_alloc();
	if (!Packet) {
		PRINTLN("无法创建packet");
		return -1;
	}

	Frame = av_frame_alloc();
	if (!Frame) {
		PRINTLN("无法创建frame");
		return -1;
	}

	HwFrame = av_frame_alloc();
	if (!HwFrame) {
		PRINTLN("无法创建frame");
		return -1;
	}

	while (av_read_frame(formatContext, Packet) >= 0) {
		if (Packet->stream_index == VideoStreamIndex) {
			ret = decodePacket(VideoCodecContext, Packet, Frame, HwFrame);
		} else if (Packet->stream_index == AudioStreamIndex) {
			ret = decodePacket(AudioCodecContext, Packet, Frame, HwFrame);
		}
		av_packet_unref(Packet);
		if (ret < 0) {
			break;
		}
	}

	if (VideoCodecContext) {
		decodePacket(VideoCodecContext, nullptr, Frame, HwFrame);
	}
	if (AudioCodecContext) {
		decodePacket(AudioCodecContext, nullptr, Frame, HwFrame);
	}
	return ret;
}

int AlphaAVDecode::decodePacket(AVCodecContext *codecContext,
	const AVPacket *packet, AVFrame *frame, AVFrame *hwFrame) {
	int ret;
	AVFrame *tmpFrame;
	ret = avcodec_send_packet(codecContext, packet);
	if (ret < 0) {
		PRINTLN("发送packet去解码失败");
		return ret;
	}
	while (true) {
		ret = avcodec_receive_frame(codecContext, frame);
		if (ret < 0) {
			if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
				return 0;
			}
			PRINTLN("获取解码后的frame失败");
			return ret;
		}

		if (codecContext->codec->type == AVMEDIA_TYPE_VIDEO) {
			if (EnableHwDecode) {
				if (frame->format == HwPixFmt) {
					if ((ret = av_hwframe_transfer_data(hwFrame, frame, 0)) <
						0) {
						PRINTLN("无法将数据从GPU传输到CPU");
						return ret;
					}
					tmpFrame = hwFrame;
				} else {
					tmpFrame = frame;
				}
				ret = outputVideoFrame(tmpFrame);
			} else {
				ret = outputVideoFrame(frame);
			}
		} else {
			ret = outputAudioFrame(frame);
		}

		av_frame_unref(frame);
		av_frame_unref(hwFrame);
		if (ret < 0) {
			return ret;
		}
	}
}
int AlphaAVDecode::outputVideoFrame(AVFrame *frame) {
	VideoFrameCount++;
	if (VideoFrameCount == 100 & frame->width > 0) {
		PRINTLN("(pts: " << frame->pts << ")->("
						 << av_get_pix_fmt_name((AVPixelFormat)frame->format)
						 << ")->[" << frame->width << "," << frame->height
						 << "]"
						 << " linesize1: " << frame->linesize[0]
						 << " linesize2: " << frame->linesize[1]
						 << " linesize3: " << frame->linesize[2]);
	}

	if ((AVPixelFormat)frame->format == AV_PIX_FMT_YUV420P) {
		std::vector<uint8_t> buffer;
		size_t bufferSize = frame->width * frame->height * 3 / 2;
		buffer.resize(bufferSize);
		for (int i = 0; i < frame->height; i++) {
			std::copy(frame->data[0] + i * frame->linesize[0],
				frame->data[0] + i * frame->linesize[0] + frame->width,
				buffer.begin() + i * frame->width);
		}
		for (int i = 0; i < frame->height / 2; i++) {
			std::copy(frame->data[1] + i * frame->linesize[1],
				frame->data[1] + i * frame->linesize[1] + frame->width / 2,
				buffer.begin() + frame->width * frame->height +
					i * frame->width / 2);
		}
		for (int i = 0; i < frame->height / 2; i++) {
			std::copy(frame->data[2] + i * frame->linesize[2],
				frame->data[2] + i * frame->linesize[2] + frame->width / 2,
				buffer.begin() + frame->width * frame->height * 5 / 4 +
					i * frame->width / 2);
		}
		outputVideoData(buffer);
	}
	if ((AVPixelFormat)frame->format == AV_PIX_FMT_NV12) {
		std::vector<uint8_t> buffer;
		size_t bufferSize = frame->width * frame->height * 3 / 2;
		buffer.resize(bufferSize);

		for (int i = 0; i < frame->height; i++) {
			std::copy(frame->data[0] + i * frame->linesize[0],
				frame->data[0] + i * frame->linesize[0] + frame->width,
				buffer.begin() + i * frame->width);
		}

		uint8_t *uData = frame->data[1];
		uint8_t *vData = frame->data[1] + 1;
		int uvLineSize = frame->linesize[1];

		for (int i = 0; i < frame->height / 2; i++) {
			for (int j = 0; j < frame->width / 2; j++) {
				buffer[frame->width * frame->height + i * frame->width / 2 +
					j] = uData[i * uvLineSize + j * 2];
				buffer[frame->width * frame->height +
					frame->width * frame->height / 4 + i * frame->width / 2 +
					j] = vData[i * uvLineSize + j * 2];
			}
		}
		outputVideoData(buffer);
	}
	return 0;
}
int AlphaAVDecode::outputAudioFrame(AVFrame *frame) { return 0; }
int AlphaAVDecode::outputVideoData(std::vector<uint8_t> buffer) {
	VideoDataHandler(buffer);
	return 0;
}
int AlphaAVDecode::outputAudioData(std::vector<uint8_t> buffer) {
	AudioDataHandler(buffer);
	return 0;
}
} // namespace av