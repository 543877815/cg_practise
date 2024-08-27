#include <torch/torch.h>

int main() {
	// 检查是否有CUDA环境，并且LibTorch是使用CUDA构建的
	if (torch::cuda::is_available()) {
		std::cout << "CUDA is available. Training on GPU." << std::endl;

		// 创建一个随机张量并移动到GPU
		torch::Tensor tensor = torch::randn({ 3, 3 }, torch::dtype(torch::kFloat32).device(torch::kCUDA));

		// 在GPU上执行操作
		torch::Tensor tensor_clone = tensor.clone(); // 克隆张量以避免原地操作
		tensor_clone = tensor_clone.to(torch::kFloat64); // 转换数据类型
		tensor_clone = tensor_clone * 2; // 简单的标量乘法

		// 将结果移回CPU并打印
		tensor_clone = tensor_clone.to(torch::kCPU);
		std::cout << "张量在GPU上操作后的结果:\n" << tensor_clone << std::endl;
	}
	else {
		std::cout << "CUDA is not available. Training on CPU." << std::endl;
	}

	return 0;
}