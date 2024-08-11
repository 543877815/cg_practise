#include <torch/torch.h>

int main() {
	// ����Ƿ���CUDA����������LibTorch��ʹ��CUDA������
	if (torch::cuda::is_available()) {
		std::cout << "CUDA is available. Training on GPU." << std::endl;

		// ����һ������������ƶ���GPU
		torch::Tensor tensor = torch::randn({ 3, 3 }, torch::dtype(torch::kFloat32).device(torch::kCUDA));

		// ��GPU��ִ�в���
		torch::Tensor tensor_clone = tensor.clone(); // ��¡�����Ա���ԭ�ز���
		tensor_clone = tensor_clone.to(torch::kFloat64); // ת����������
		tensor_clone = tensor_clone * 2; // �򵥵ı����˷�

		// ������ƻ�CPU����ӡ
		tensor_clone = tensor_clone.to(torch::kCPU);
		std::cout << "������GPU�ϲ�����Ľ��:\n" << tensor_clone << std::endl;
	}
	else {
		std::cout << "CUDA is not available. Training on CPU." << std::endl;
	}

	return 0;
}