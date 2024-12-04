#include "lodepng.h"
#include <iostream>

// Example 1
// Encode from raw pixels to disk with a single function call
// The image argument has width * height RGBA pixels or width * height * 4 bytes
void encodeOneStep(const char *filename, std::vector<unsigned char> &image,
	unsigned width, unsigned height) {
	// Encode the image
	unsigned error = lodepng::encode(filename, image, width, height);

	// if there's an error, display it
	if (error)
		std::cout << "encoder error " << error << ": "
				  << lodepng_error_text(error) << std::endl;
}

// Example 2
// Encode from raw pixels to an in-memory PNG file first, then write it to disk
// The image argument has width * height RGBA pixels or width * height * 4 bytes
void encodeTwoSteps(const char *filename, std::vector<unsigned char> &image,
	unsigned width, unsigned height) {
	std::vector<unsigned char> png;

	unsigned error = lodepng::encode(png, image, width, height);
	if (!error)
		lodepng::save_file(png, filename);

	// if there's an error, display it
	if (error)
		std::cout << "encoder error " << error << ": "
				  << lodepng_error_text(error) << std::endl;
}

// Example 3
// Save a PNG file to disk using a State, normally needed for more advanced
// usage. The image argument has width * height RGBA pixels or width * height *
// 4 bytes
void encodeWithState(const char *filename, std::vector<unsigned char> &image,
	unsigned width, unsigned height) {
	std::vector<unsigned char> png;
	lodepng::State state; // optionally customize this one

	unsigned error = lodepng::encode(png, image, width, height, state);
	if (!error)
		lodepng::save_file(png, filename);

	// if there's an error, display it
	if (error)
		std::cout << "encoder error " << error << ": "
				  << lodepng_error_text(error) << std::endl;
}

int main(int argc, char **argv) {
	// NOTE: this sample will overwrite the file or test.png without warning!
	const char *filename = argc > 1 ? argv[1] : "test.png";

	unsigned width = 512, height = 512;
	std::vector<unsigned char> image;
	image.resize(width * height * 4);

	for (unsigned blockY = 0; blockY < height / 32; blockY++) {
		for (unsigned blockX = 0; blockX < width / 32; blockX++) {
			unsigned char r = static_cast<unsigned char>(rand() % 256);
			unsigned char g = static_cast<unsigned char>(rand() % 256);
			unsigned char b = static_cast<unsigned char>(rand() % 256);
			unsigned char a = 255;
			for (unsigned y = 0; y < 32; y++) {
				for (unsigned x = 0; x < 32; x++) {
					unsigned int index =
						4 * ((blockY * 32 + y) * width + blockX * 32 + x);
					image[index + 0] = r;
					image[index + 1] = g;
					image[index + 2] = b;
					image[index + 3] = a;
				}
			}
		}
	}

	encodeOneStep(filename, image, width, height);
	return 0;
}
