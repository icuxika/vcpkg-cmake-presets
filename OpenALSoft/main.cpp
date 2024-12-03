#include "alpha-av-core.h"
#include <al.h>
#include <alc.h>

ALCdevice *device = nullptr;
ALCcontext *context = nullptr;
ALuint source;

int initALContext() {
	device = alcOpenDevice(nullptr);
	if (device == nullptr) {
		return -1;
	}
	context = alcCreateContext(device, nullptr);
	if (context == nullptr) {
		alcCloseDevice(device);
		return -1;
	}
	alcMakeContextCurrent(context);
	if (alGetError() != AL_NO_ERROR) {
		return -1;
	}

	ALfloat position[] = {0.0f, 0.0f, 0.0f};
	ALfloat velocity[] = {0.0f, 0.0f, 0.0f};

	alGenSources(1, &source);
	// 高音倍数
	alSourcef(source, AL_PITCH, 1.0f);
	// 声音增益
	alSourcef(source, AL_GAIN, 1.0f);
	// 声音位置
	alSourcefv(source, AL_POSITION, position);
	// 声音移速
	alSourcefv(source, AL_VELOCITY, velocity);
	// 是否循环播放
	alSourcei(source, AL_LOOPING, AL_FALSE);
	return 0;
}

int playAVFrame(uint8_t *data, int size) {
	ALuint buffer;
	alGenBuffers(1, &buffer);
	// AL_FORMAT_STEREO16与AVFrame在swr_convert重采样前swr_alloc_set_opts2设置的out_sample_fmt有关
	alBufferData(buffer, AL_FORMAT_STEREO16, data, size, 48000);
	alSourceQueueBuffers(source, 1, &buffer);
	ALint state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	if (state != AL_PLAYING) {
		alSourcePlay(source);
		if (alGetError() != AL_NO_ERROR) {
			PRINTLN("播放音频时出错");
			return -1;
		}
	}
	return 0;
}

int clearALContext() {
	ALint processed;
	alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
	while (processed > 0) {
		ALuint finishedBuffer;
		alSourceUnqueueBuffers(source, 1, &finishedBuffer);
		alDeleteBuffers(1, &finishedBuffer);
		processed--;
	}
	alcMakeContextCurrent(nullptr);
	alDeleteSources(1, &source);
	alcDestroyContext(context);
	alcCloseDevice(device);
	return 0;
}

// https://github.com/kcat/openal-soft/blob/master/examples/alffplay.cpp
// https://zhuanlan.zhihu.com/p/406575059
int main(int argc, char **argv) {
	initALContext();

	const char *url = "C:\\Users\\icuxika\\Desktop\\test.mp4";
	av::AlphaAVCore::GetInstance().openFile(url);
	av::AlphaAVCore::GetInstance().AlphaAVDecodeContext->VideoDataHandler =
		[](std::vector<uint8_t> buffer) {};
	av::AlphaAVCore::GetInstance().AlphaAVDecodeContext->AudioDataHandler =
		[](std::vector<uint8_t> buffer) {
			playAVFrame(buffer.data(), buffer.size());
		};
	av::AlphaAVCore::GetInstance().play();

	clearALContext();
	return 0;
}