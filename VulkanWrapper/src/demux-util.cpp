#include "demux-util.h"

namespace av {
DemuxUtil::DemuxUtil() {}

DemuxUtil::~DemuxUtil() {
	avcodec_free_context(&VideoCodecContext);
	avcodec_free_context(&AudioCodecContext);
	avformat_close_input(&FormatContext);
	av_packet_free(&Packet);
	av_frame_free(&Frame);
	avformat_free_context(FormatContext);
}

DemuxUtil &DemuxUtil::GetInstance() {
	static DemuxUtil instance;
	return instance;
}

int DemuxUtil::openFile(const char *url) {
	AVDictionary *options = nullptr;
	// 设置rtsp流以tcp协议打开
	av_dict_set(&options, "rtsp_transport", "tcp", 0);
	// 设置网络延时
	av_dict_set(&options, "max_delay", "500", 0);

	int ret = 0;
	// 打开视频文件流，读取文件头的一些信息
	ret = avformat_open_input(&FormatContext, url, nullptr, &options);
	if (ret < 0) {
		std::cout << "avformat_open_input failed: " << av_err2str(ret)
				  << std::endl;
		throw std::runtime_error("无法打开文件");
	}
	// 获取流信息
	ret = avformat_find_stream_info(FormatContext, nullptr);
	if (ret < 0) {
		std::cout << "avformat_find_stream_info failed: " << av_err2str(ret)
				  << std::endl;
		throw std::runtime_error("无法获取视频文件的流信息");
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

	if (openCodecContext(
			&VideoStreamIndex, &VideoCodecContext, AVMEDIA_TYPE_VIDEO) >= 0) {
		VideoStream = FormatContext->streams[VideoStreamIndex];
		std::cout << "------------------------------------------------------"
				  << std::endl;
		std::cout << "视频宽度" << VideoCodecContext->width << std::endl;
		std::cout << "视频高度" << VideoCodecContext->height << std::endl;
		std::cout << "视频像素格式"
				  << av_get_pix_fmt_name(
						 (AVPixelFormat)VideoCodecContext->pix_fmt)
				  << std::endl;

		Width = VideoCodecContext->width;
		Height = VideoCodecContext->height;
		PixFmt = VideoCodecContext->pix_fmt;

		// 存放解码后的一帧数据
		ret = av_image_alloc(
			VideoDstData, VideoDstLinesize, Width, Height, PixFmt, 1);
		if (ret < 0) {
			throw std::runtime_error("无法为视频帧数据分配缓存空间");
		}
		VideoDstBufsize = ret;
	}

	if (openCodecContext(
			&AudioStreamIndex, &AudioCodecContext, AVMEDIA_TYPE_AUDIO) >= 0) {
		AudioStream = FormatContext->streams[AudioStreamIndex];
		std::cout << "------------------------------------------------------"
				  << std::endl;
		std::cout << "样本率" << AudioCodecContext->sample_rate << std::endl;
		AVChannelLayout avChannelLayout;
		av_channel_layout_default(
			&avChannelLayout, AudioCodecContext->ch_layout.nb_channels);
		std::cout << "声道数" << avChannelLayout.nb_channels << std::endl;
		std::cout << "------------------------------------------------------"
				  << std::endl;
	}

	return ret;
}

int DemuxUtil::startDecode(int type) {
	int ret = 0;
	if (type & static_cast<int>(Type::VIDEO)) {
		std::cout << "开启视频解码" << std::endl;
		if (!VideoHandler) {
			std::cout << "请提供视频AVFrame的处理函数" << std::endl;
			VideoHandler = [](AVFrame *frame) {};
		}
		EnableVideoDecode = true;
	}
	if (type & Type::AUDIO) {
		std::cout << "开启音频解码" << std::endl;
		if (!AudioHandler) {
			std::cout << "请提供音频AVFrame的处理函数" << std::endl;
			AudioHandler = [](AVFrame *frame) {};
		}
		EnableAudioDecode = true;
	}
	if ((!EnableVideoDecode) && (!EnableAudioDecode)) {
		std::cout << "不进行解码" << std::endl;
		return -1;
	}

	if (EnableVideoDecode && !VideoStream) {
		throw std::runtime_error("无法获取视频文件的视频流");
	}

	if (EnableAudioDecode && !AudioStream) {
		throw std::runtime_error("无法获取视频文件的音频流");
	}

	Frame = av_frame_alloc();
	if (!Frame) {
		ret = AVERROR(ENOMEM);
		throw std::runtime_error("无法创建frame!");
	}

	Packet = av_packet_alloc();
	if (!Packet) {
		ret = AVERROR(ENOMEM);
		throw std::runtime_error("无法创建packet!");
	}

	while (av_read_frame(FormatContext, Packet) >= 0) {
		if (Packet->stream_index == VideoStreamIndex) {
			ret = decodePacket(VideoCodecContext, Packet, Frame);
		} else if (Packet->stream_index == AudioStreamIndex) {
			ret = decodePacket(AudioCodecContext, Packet, Frame);
		}
		av_packet_unref(Packet);
		if (ret < 0) {
			break;
		}
	}

	if (VideoCodecContext) {
		decodePacket(VideoCodecContext, nullptr, Frame);
	}
	if (AudioCodecContext) {
		decodePacket(AudioCodecContext, nullptr, Frame);
	}
	return ret;
}

int DemuxUtil::openCodecContext(int *streamIndex, AVCodecContext **codecContext,
	enum AVMediaType mediaType) {
	int ret, index;
	AVStream *stream;
	const AVCodec *codec = nullptr;

	ret = av_find_best_stream(FormatContext, mediaType, -1, -1, nullptr, 0);
	if (ret < 0) {
		std::cout << "无法获取流: " << av_get_media_type_string(mediaType)
				  << std::endl;
		std::cout << "av_find_best_stream failed: " << av_err2str(ret)
				  << std::endl;
		return ret;
	}
	index = ret;
	stream = FormatContext->streams[index];

	// 找到解码器
	codec = avcodec_find_decoder(stream->codecpar->codec_id);
	if (!codec) {
		std::cout << "找不到解码器: " << av_get_media_type_string(mediaType)
				  << std::endl;
		return AVERROR(EINVAL);
	}

	// allocate codec context
	*codecContext = avcodec_alloc_context3(codec);
	if (!*codecContext) {
		std::cout << "无法分配codec context: "
				  << av_get_media_type_string(mediaType) << std::endl;
		return AVERROR(ENOMEM);
	}

	// copy codec parameters
	if ((ret = avcodec_parameters_to_context(*codecContext, stream->codecpar)) <
		0) {
		std::cout << "无法复制codec parameters: "
				  << av_get_media_type_string(mediaType) << std::endl;
		return ret;
	}

	// 初始化解码器
	if ((ret = avcodec_open2(*codecContext, codec, nullptr)) < 0) {
		std::cout << "无法打开解码器: " << av_get_media_type_string(mediaType)
				  << std::endl;
		return ret;
	}
	*streamIndex = index;

	return 0;
}

int DemuxUtil::decodePacket(
	AVCodecContext *codecContext, const AVPacket *packet, AVFrame *frame) {
	int ret = 0;
	ret = avcodec_send_packet(codecContext, packet);
	if (ret < 0) {
		std::cout << "发送packet去解码失败: " << av_err2str(ret) << std::endl;
		return ret;
	}

	while (ret >= 0) {
		ret = avcodec_receive_frame(codecContext, frame);
		if (ret < 0) {
			if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
				return 0;
			}
			std::cout << "获取解码后得到的frame失败: " << av_err2str(ret)
					  << std::endl;
			return ret;
		}

		if (codecContext->codec->type == AVMEDIA_TYPE_VIDEO) {
			ret = outputVideoFrame(frame);
		} else {
			ret = outputAudioFrame(frame);
		}
		av_frame_unref(frame);
		if (ret < 0) {
			return ret;
		}
	}
	return ret;
}

int DemuxUtil::outputVideoFrame(AVFrame *frame) {
	if (frame->width != Width || frame->height != Height ||
		frame->format != PixFmt) {
		std::cout << "视频的宽度或高度或像素格式发生了变化: \n"
				  << "旧: " << Width << Height << av_get_pix_fmt_name(PixFmt)
				  << "\n"
				  << "新: " << frame->width << frame->height
				  << av_get_pix_fmt_name((AVPixelFormat)frame->format)
				  << std::endl;
		return -1;
	}

	VideoFrameCount++;
	// std::cout << "当前视频帧序号: " << VideoFrameCount << std::endl;
	if (VideoFrameCount == 100 && frame->width > 0) {
		std::cout << "(pts: " << frame->pts << ")->[" << frame->width << ","
				  << frame->height << "]"
				  << " linesize1: " << frame->linesize[0]
				  << " linesize2: " << frame->linesize[1]
				  << " linesize3: " << frame->linesize[2] << std::endl;
		// yuv2file(frame);
		// return -1;
	}
	VideoHandler(frame);
	return 0;
}

int DemuxUtil::outputAudioFrame(AVFrame *frame) {
	AudioHandler(frame);
	return 0;
}

void DemuxUtil::yuv2file(AVFrame *frame) {
	av_image_copy(VideoDstData, VideoDstLinesize,
		(const uint8_t **)(frame->data), frame->linesize, PixFmt, Width,
		Height);

	// ffplay -f rawvideo -video_size 3840x2160 ~/Downloads/video_output_file
	std::ofstream dstFile(
		"/Users/icuxika/Downloads/video_output_file", std::ios::binary);
	if (dstFile.is_open()) {
		for (int i = 0; i < frame->height; i++) {
			dstFile.write(reinterpret_cast<const char *>(
							  frame->data[0] + i * frame->linesize[0]),
				frame->width);
		}
		for (int i = 0; i < frame->height / 2; i++) {
			dstFile.write(reinterpret_cast<const char *>(
							  frame->data[1] + i * frame->linesize[1]),
				frame->width / 2);
		}
		for (int i = 0; i < frame->height / 2; i++) {
			dstFile.write(reinterpret_cast<const char *>(
							  frame->data[2] + i * frame->linesize[2]),
				frame->width / 2);
		}
		dstFile.close();
	}
}

void DemuxUtil::yuv2rgba(AVFrame *frame) {
	struct SwsContext *swsCtx = sws_getContext(frame->width, frame->height,
		(AVPixelFormat)frame->format, frame->width, frame->height,
		AV_PIX_FMT_RGBA, SWS_BILINEAR, NULL, NULL, NULL);

	sws_scale(swsCtx, (const uint8_t *const *)(frame->data), frame->linesize, 0,
		frame->height, VideoDstData, VideoDstLinesize);
}
} // namespace av
