#include <iostream>
#include <cmath> // 更现代的头文件包含方式

// 计算第 n 个斐波那契数的迭代方法
unsigned long long fibonacciIterative(int n) {
	if (n <= 1) {
		return n;
	}
	unsigned long long first = 0, second = 1;
	for (int i = 2; i <= n; ++i) {
		unsigned long long nextTerm = first + second;
		first = second;
		second = nextTerm;
	}
	return second;
}

// 使用 Binet 公式计算第 n 个斐波那契数
unsigned long long fibonacciBinet(int n) {
	const double sqrt5 = std::sqrt(5);
	const double goldenRatio = (1.0 + sqrt5) / 2.0;
	const double inverseGoldenRatio = (1.0 - sqrt5) / 2.0;
	return static_cast<unsigned long long>(std::round((std::pow(goldenRatio, n) - std::pow(inverseGoldenRatio, n)) / sqrt5));
}

int main() {
	const int sequenceLength = 71;  // 计算斐波那契数列的前 71 项

	// 使用迭代方法打印斐波那契数列
	std::cout << "Iterative Fibonacci sequence up to " << sequenceLength << ":" << std::endl;
	for (int i = 0; i <= sequenceLength; i++) {
		std::cout << fibonacciIterative(i) << " ";
	}
	std::cout << std::endl;

	// 使用 Binet 公式打印斐波那契数列
	std::cout << "Binet's Formula Fibonacci sequence up to " << sequenceLength << ":" << std::endl;
	for (int i = 0; i <= sequenceLength; i++) {
		std::cout << fibonacciBinet(i) << " ";
	}
	std::cout << std::endl;

	return 0;
}