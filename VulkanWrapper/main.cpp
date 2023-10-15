#include <string>
#include <thread>
#include <utility>
#define GLFW_INCLUDE_VULKAN
#include "context.h"
#include "demux-util.h"
#include <GLFW/glfw3.h>
#include <chrono>
#include <cxxopts.hpp>
#include <fstream>
#include <iostream>
#include <list>
#include <mutex>
#include <vector>

std::list<std::vector<uint8_t>> yuvDataList;
std::mutex yuvMutex;

int main(int argc, char **argv) {
	cxxopts::Options options("VulkanWrapper", "Yuv420P Player");
	options.add_options()("f,file", "File name", cxxopts::value<std::string>())(
		"m,mode", "Framerate limitation",
		cxxopts::value<bool>()->default_value("false"))(
		"h,help", "Print usage");
	auto result = options.parse(argc, argv);
	if (result.count("help") || !result.count("file")) {
		std::cout << options.help() << std::endl;
		exit(0);
	}
	auto filename = result["file"].as<std::string>();
	std::cout << "文件路径 " << filename << std::endl;
	const char *url = filename.c_str();

	// 是否控制帧率与视频帧率一致
	bool limitFramerate = result["mode"].as<bool>();

	// FFmpeg 功能初始化
	auto &demuxUtil = av::DemuxUtil::GetInstance();
	demuxUtil.VideoHandler = [](AVFrame *frame) {
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
		std::lock_guard<std::mutex> lock(yuvMutex);
		yuvDataList.push_back(buffer);
	};
	demuxUtil.AudioHandler = [](AVFrame *frame) {};
	demuxUtil.openFile(url);

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow *window =
		glfwCreateWindow(1280, 720, "Vulkan Wrapper", nullptr, nullptr);

	// Vulkan 初始化
	vw::Context::GetInstance().initVkContext(
		window, demuxUtil.Width, demuxUtil.Height);

	// 键盘事件监听
	glfwSetKeyCallback(window,
		[](GLFWwindow *window, int key, int scancode, int action, int mods) {
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			}
			if (key == GLFW_KEY_U && action == GLFW_PRESS) {
			}
		});

	// 窗口尺寸变化监听
	glfwSetFramebufferSizeCallback(
		window, [](GLFWwindow *window, int width, int height) {
			vw::Context::GetInstance().FramebufferResized = true;
		});

	// 视频解码线程
	std::thread t(&av::DemuxUtil::startDecode, &demuxUtil,
		av::Type::VIDEO | av::Type::AUDIO);

	long long targetFrameInterval = 1000000.0 / demuxUtil.VideoFrameRate;
	std::cout << "帧间隔时间μs" << targetFrameInterval << std::endl;
	while (!glfwWindowShouldClose(window)) {
		// 帧开始时间
		auto startFrameTime = std::chrono::high_resolution_clock::now();

		glfwPollEvents();
		std::lock_guard<std::mutex> lock(yuvMutex);
		if (!yuvDataList.empty()) {
			std::vector<uint8_t> data = yuvDataList.front();
			vw::Context::GetInstance().BufferContext->loadYUVData(data);
			yuvDataList.pop_front();
			vw::Context::GetInstance().RenderContext->drawFrame();

			// 帧结束时间
			auto endFrameTime = std::chrono::high_resolution_clock::now();
			auto frameTime =
				std::chrono::duration_cast<std::chrono::microseconds>(
					endFrameTime - startFrameTime)
					.count();
			// 帧率控制，并不准确
			if (limitFramerate && frameTime < targetFrameInterval) {
				while (true) {
					endFrameTime = std::chrono::high_resolution_clock::now();
					frameTime =
						std::chrono::duration_cast<std::chrono::microseconds>(
							endFrameTime - startFrameTime)
							.count();
					if (frameTime >= targetFrameInterval) {
						break;
					}
					continue;
				}
			}
		}
	}
	vkDeviceWaitIdle(vw::Context::GetInstance().LogicalDevice);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
