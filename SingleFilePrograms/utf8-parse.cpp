#include <fmt/core.h>
#include <iostream>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/view.hpp>
#include <range/v3/view/chunk_by.hpp>
#include <xstring>

namespace view = ranges::views;

int main(int argc, char **argv) {
	std::u8string str = u8"music: 渺啊渺 往无语 来无杳";
	std::cout << "字节数: " << str.length() << std::endl;
	for (auto c : str) {
		std::cout << fmt::format("{0:08b}", char(c)) << " ";
	}
	std::cout << "\n\n";

	// 切分成字符流
	auto fun = [](auto c1, auto c2) {
		return (0b11000000 & char(c2)) == 0b10000000;
	};
	auto v = str | view::chunk_by(fun);

	std::cout << "字符数: " << ranges::distance(v) << std::endl;
	for (auto vv : v) {
		for (auto c : vv) {
			std::cout << fmt::format("{0:08b} ", char(c));
		}
		std::cout << "\n";
	}
	std::cout << "\n\n";

	std::cout << "第三个字符的字符串: ";
	{
		auto c3 = v | view::drop(2) | view::take(1);
		for (auto c : c3) {
			for (auto cc : c) {
				std::cout << fmt::format("{0:08b} ", char(cc));
			}
		}
	}
	std::cout << "\n";
	std::cout << "第三个字符的字符串: ";
	{
		auto c3 = v | view::drop(2) | view::take(1);
		for (auto c : c3) {
			std::string s;
			for (auto cc : c) {
				s.push_back(cc);
			}
			std::cout << s;
		}
	}
	std::cout << "\n";

	std::cout << "第九个字符的字符串: ";
	{
		auto c3 = v | view::drop(8) | view::take(1);
		for (auto c : c3) {
			for (auto cc : c) {
				std::cout << fmt::format("{0:08b} ", char(cc));
			}
		}
	}
	std::cout << "\n";
	std::cout << "第九个字符的字符串: ";
	{
		auto c3 = v | view::drop(8) | view::take(1);
		for (auto c : c3) {
			std::string s;
			for (auto cc : c) {
				s.push_back(cc);
			}
			std::cout << s;
		}
	}
	std::cout << "\n";
	return 0;
}
