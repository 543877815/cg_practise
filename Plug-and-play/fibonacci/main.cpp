#include <iostream>
#include <cmath> // ���ִ���ͷ�ļ�������ʽ

// ����� n ��쳲��������ĵ�������
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

// ʹ�� Binet ��ʽ����� n ��쳲�������
unsigned long long fibonacciBinet(int n) {
	const double sqrt5 = std::sqrt(5);
	const double goldenRatio = (1.0 + sqrt5) / 2.0;
	const double inverseGoldenRatio = (1.0 - sqrt5) / 2.0;
	return static_cast<unsigned long long>(std::round((std::pow(goldenRatio, n) - std::pow(inverseGoldenRatio, n)) / sqrt5));
}

int main() {
	const int sequenceLength = 71;  // ����쳲��������е�ǰ 71 ��

	// ʹ�õ���������ӡ쳲���������
	std::cout << "Iterative Fibonacci sequence up to " << sequenceLength << ":" << std::endl;
	for (int i = 0; i <= sequenceLength; i++) {
		std::cout << fibonacciIterative(i) << " ";
	}
	std::cout << std::endl;

	// ʹ�� Binet ��ʽ��ӡ쳲���������
	std::cout << "Binet's Formula Fibonacci sequence up to " << sequenceLength << ":" << std::endl;
	for (int i = 0; i <= sequenceLength; i++) {
		std::cout << fibonacciBinet(i) << " ";
	}
	std::cout << std::endl;

	return 0;
}