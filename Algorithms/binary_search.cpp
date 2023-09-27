#include <functional>
#include <iostream>
#include <vector>

size_t binarySearchArray(const std::vector<int> &vec, int target) {
	size_t low = 0;
	size_t high = vec.size();
	while (low != high) {
		size_t mid = (low + high) / 2;
		if (vec[mid] < target) {
			low = mid + 1;
		} else {
			high = mid;
		}
	}
	if (vec[low] == target) {
		return low;
	}
	std::cout << "low = " << low << std::endl;
	return vec.size();
}

uint32_t binarySearchSq(uint32_t sq) {
	uint32_t low = 0;
	uint32_t high = sq + 1;
	while (low != high) {
		uint32_t mid = (low + high) / 2;
		if (mid * mid > sq) {
			high = mid;
		} else {
			low = mid + 1;
		}
	}
	return low - 1;
}

// 统一的二分法
//
// 整个区间分为两半：
//     左边全部满足某个条件
//     右边全部不满足
// 找到右边子区间的起始位置
//     首个不满足条件的位置
// [low, high)
size_t binarySearch(size_t low, size_t high, std::function<bool(size_t)> func) {
	while (low != high) {
		size_t mid = (low + high) / 2;
		if (func(mid)) {
			low = mid + 1;
		} else {
			high = mid;
		}
	}
	return low;
}

int main(int argc, char **argv) {
	std::cout << "========================================" << std::endl;
	std::vector<int> vec = {1, 12, 23, 34, 45, 56, 67, 78, 89, 100};
	int target = 25;
	size_t index = binarySearchArray(vec, target);
	std::cout << "index = " << index << std::endl;
	std::cout << "========================================" << std::endl;
	std::cout << binarySearchSq(25) << std::endl;
	std::cout << binarySearchSq(37) << std::endl;
	std::cout << "========================================" << std::endl;

	std::cout << binarySearch(0, vec.size(), [&](size_t index) {
		return vec[index] < target;
	}) << std::endl;

	std::cout << binarySearch(0, 38, [](size_t x) { return x * x <= 38; }) - 1
			  << std::endl;
	return 0;
}
