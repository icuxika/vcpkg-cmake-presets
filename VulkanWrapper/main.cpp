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
		"h,help", "Print usage");
	auto result = options.parse(argc, argv);
	if (result.count("help") || !result.count("file")) {
		std::cout << options.help() << std::endl;
		exit(0);
	}
	auto filename = result["file"].as<std::string>();
	std::cout << filename << std::endl;

	using namespace std::chrono_literals;
	const char *url = filename.c_str();

	auto &demuxUtil = av::DemuxUtil::GetInstance();
	demuxUtil.VideoHandler = [](AVFrame *frame) {
		std::chrono::high_resolution_clock::time_point start =
			std::chrono::high_resolution_clock::now();
		// std::this_thread::sleep_for(70ms);
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
		std::chrono::high_resolution_clock::time_point end =
			std::chrono::high_resolution_clock::now();
		auto duration =
			std::chrono::duration_cast<std::chrono::microseconds>(end - start)
				.count();
		// std::cout << "[vedio] " << duration << std::endl;
		std::lock_guard<std::mutex> lock(yuvMutex);
		yuvDataList.push_back(buffer);
	};
	demuxUtil.AudioHandler = [](AVFrame *frame) {};
	demuxUtil.openFile(url);
	std::cout << "video width: " << demuxUtil.Width << std::endl;
	std::cout << "video height: " << demuxUtil.Height << std::endl;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow *window =
		glfwCreateWindow(1280, 720, "Vulkan Wrapper", nullptr, nullptr);

	vw::Context::GetInstance().initVkContext(
		window, demuxUtil.Width, demuxUtil.Height);

	glfwSetKeyCallback(window,
		[](GLFWwindow *window, int key, int scancode, int action, int mods) {
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			}
			if (key == GLFW_KEY_U && action == GLFW_PRESS) {
				// vw::Context::GetInstance().BufferContext->loadYUVData();
			}
		});
	glfwSetFramebufferSizeCallback(
		window, [](GLFWwindow *window, int width, int height) {
			vw::Context::GetInstance().FramebufferResized = true;
		});

	std::thread t(&av::DemuxUtil::startDecode, &demuxUtil,
		av::Type::VIDEO | av::Type::AUDIO);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		std::lock_guard<std::mutex> lock(yuvMutex);
		if (!yuvDataList.empty()) {
      std::vector<uint8_t> data = yuvDataList.front();
			vw::Context::GetInstance().BufferContext->loadYUVData(data);
			yuvDataList.pop_front();
		}
		vw::Context::GetInstance().RenderContext->drawFrame();
	}
	vkDeviceWaitIdle(vw::Context::GetInstance().LogicalDevice);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
