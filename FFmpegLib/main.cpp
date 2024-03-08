#include "alpha-av-core.h"
#include <fstream>
#include <iostream>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avassert.h>
#include <libavutil/hwcontext.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
}

/**
 * https://github.com/FFmpeg/FFmpeg/blob/master/doc/examples/hw_decode.c
 * https://zhuanlan.zhihu.com/p/609946275?utm_id=0
 * https://blog.csdn.net/yaningli/article/details/128032093?spm=1001.2100.3001.7377&utm_medium=distribute.pc_feed_blog_category.none-task-blog-classify_tag-3-128032093-null-null.nonecase&depth_1-utm_source=distribute.pc_feed_blog_category.none-task-blog-classify_tag-3-128032093-null-null.nonecase
 */

static AVBufferRef *hwDeviceCtx = nullptr;
static enum AVPixelFormat hwPixFmt;
static int videoFrameCount = 0;

static int hwDecoderInit(AVCodecContext *ctx, const enum AVHWDeviceType type) {
	int err = 0;

	if ((err = av_hwdevice_ctx_create(
			 &hwDeviceCtx, type, nullptr, nullptr, 0)) < 0) {
		std::cout << "无法创建指定的HW设备" << std::endl;
		return err;
	}
	ctx->hw_device_ctx = av_buffer_ref(hwDeviceCtx);

	return err;
}

static enum AVPixelFormat getHwFormat(
	AVCodecContext *ctx, const enum AVPixelFormat *pixFmts) {
	const enum AVPixelFormat *p;

	for (p = pixFmts; *p != -1; p++) {
		if (*p == hwPixFmt)
			return *p;
	}

	std::cout << "无法获取 HW surface format" << std::endl;
	return AV_PIX_FMT_NONE;
}

static void processNV12Frame(AVFrame *frame) {
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
			buffer[frame->width * frame->height + i * frame->width / 2 + j] =
				uData[i * uvLineSize + j * 2];
			buffer[frame->width * frame->height +
				frame->width * frame->height / 4 + i * frame->width / 2 + j] =
				vData[i * uvLineSize + j * 2];
		}
	}

	// ffplay.exe -f rawvideo -video_size 2560x1440
	// C:\Users\icuxika\Desktop\test.yuv
	std::ofstream dstFile(
		"C:\\Users\\icuxika\\Desktop\\test.yuv", std::ios::binary);
	if (dstFile.is_open()) {
		std::cout << "写入文件" << std::endl;
		dstFile.write(
			reinterpret_cast<const char *>(buffer.data()), bufferSize);
		dstFile.close();
	}
}

static int decodeWrite(AVCodecContext *codecContext, AVPacket *packet) {
	AVFrame *frame = nullptr, *swFrame = nullptr;
	AVFrame *tmpFrame = nullptr;
	int ret = 0;

	ret = avcodec_send_packet(codecContext, packet);
	if (ret < 0) {
		std::cout << "解码时发生错误" << std::endl;
		return ret;
	}

	while (1) {
		if (!(frame = av_frame_alloc()) || !(swFrame = av_frame_alloc())) {
			std::cout << "无法分配Frame" << std::endl;
			ret = AVERROR(ENOMEM);
			break;
		}

		ret = avcodec_receive_frame(codecContext, frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
			av_frame_free(&frame);
			av_frame_free(&swFrame);
			return 0;
		}
		if (ret < 0) {
			std::cout << "解码时发生错误" << std::endl;
			return ret;
		}

		if (frame->format == hwPixFmt) {
			/* retrieve data from GPU to CPU */
			if ((ret = av_hwframe_transfer_data(swFrame, frame, 0)) < 0) {
				std::cout << "无法传输数据到系统内存" << std::endl;
				return ret;
			}
			tmpFrame = swFrame;
		} else {
			tmpFrame = frame;
		}

		videoFrameCount++;
		if (videoFrameCount == 100 & tmpFrame->width > 0) {
			std::cout << "(pts: " << tmpFrame->pts << ")->("
					  << av_get_pix_fmt_name((AVPixelFormat)tmpFrame->format)
					  << ")->[" << tmpFrame->width << "," << tmpFrame->height
					  << "]"
					  << " linesize1: " << tmpFrame->linesize[0]
					  << " linesize2: " << tmpFrame->linesize[1]
					  << " linesize3: " << tmpFrame->linesize[2] << std::endl;
			processNV12Frame(tmpFrame);
			break;
		}
	}
	return ret;
}

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

int main1(int argc, char *argv[]) {
	std::cout << "Hello, world!" << std::endl;
	const char *queryType = "cuda";
	enum AVHWDeviceType type;
	type = av_hwdevice_find_type_by_name(queryType);
	if (type == AV_HWDEVICE_TYPE_NONE) {
		std::cout << "不支持硬件设备 " << queryType << std::endl;
		std::cout << "下面是支持的硬件设备" << std::endl;
		while (
			(type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE) {
			std::cout << "->" << av_hwdevice_get_type_name(type) << std::endl;
		}
	}

	const char *url = "C:\\Users\\icuxika\\Desktop\\test.mp4";
	AVFormatContext *formatContext = nullptr;
	const AVCodec *decoder = nullptr;
	int videoStream = 0;
	AVCodecContext *decoderContext = nullptr;
	AVStream *video = nullptr;
	AVPacket *packet = nullptr;

	AVDictionary *options = nullptr;
	// 设置rtsp流以tcp协议打开
	av_dict_set(&options, "rtsp_transport", "tcp", 0);
	// 设置网络延时
	av_dict_set(&options, "max_delay", "500", 0);

	int ret = 0;
	// 打开视频文件流，读取文件头的一些信息
	ret = avformat_open_input(&formatContext, url, nullptr, &options);
	if (ret < 0) {
		return ret;
	}
	// 获取流信息
	ret = avformat_find_stream_info(formatContext, nullptr);
	if (ret < 0) {
		return ret;
	}
	// 打印视频流信息
	av_dump_format(formatContext, 0, url, 0);

	ret = av_find_best_stream(
		formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0);
	if (ret < 0) {
		return ret;
	}
	videoStream = ret;

	for (int i = 0;; i++) {
		const AVCodecHWConfig *config = avcodec_get_hw_config(decoder, i);
		if (!config) {
			std::cout << "解码器 " << decoder->name << " 不支持设备类型 "
					  << av_hwdevice_get_type_name(type) << std::endl;
			return -1;
		}
		if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
			config->device_type == type) {
			hwPixFmt = config->pix_fmt;
			break;
		}
	}

	decoderContext = avcodec_alloc_context3(decoder);
	if (!decoderContext) {
		return -1;
	}

	video = formatContext->streams[videoStream];
	ret = avcodec_parameters_to_context(decoderContext, video->codecpar);
	if (ret < 0) {
		return ret;
	}

	decoderContext->get_format = getHwFormat;

	ret = hwDecoderInit(decoderContext, type);
	if (ret < 0) {
		return ret;
	}

	ret = avcodec_open2(decoderContext, decoder, nullptr);
	if (ret < 0) {
		return ret;
	}

	packet = av_packet_alloc();
	if (!packet) {
		return -1;
	}

	std::cout << "-------------------------------------" << std::endl;

	while (ret >= 0) {
		if ((ret = av_read_frame(formatContext, packet)) < 0) {
			break;
		}
		if (videoStream == packet->stream_index) {
			ret = decodeWrite(decoderContext, packet);
		}
		av_packet_unref(packet);
	}

	// flush the decoder
	ret = decodeWrite(decoderContext, nullptr);

	av_packet_free(&packet);
	avcodec_free_context(&decoderContext);
	avformat_close_input(&formatContext);
	av_buffer_unref(&hwDeviceCtx);
	return ret;
}