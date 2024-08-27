//#include <httplib/httplib.h>
//#include <iostream>
//
//int main(void)
//{
//	httplib::Client cli("localhost", 1234);
//
//	if (auto res = cli.Get("/hi")) {
//		if (res->status == httplib::StatusCode::OK_200) {
//			std::cout << res->body << std::endl;
//		}
//	}
//	else {
//		auto err = res.error();
//		std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
//	}
//}

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib/httplib.h>
#include <iostream>
#include <string>

std::string urlEncode(const std::string& value) {
	std::string encoded = "";
	for (size_t i = 0; i < value.length(); i++) {
		const char c = value[i];
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
			// 允许的字符直接添加
			encoded += c;
		}
		else if (c == ' ') {
			// 空格转换为 +
			encoded += '+';
		}
		else {
			// 其他字符转换为 %XX 格式
			encoded += '%';
			char hex[4];
			sprintf(hex, "%02X", static_cast<unsigned char>(c));
			encoded += hex;
		}
	}
	return encoded;
}

int main() {
	// 初始化 httplib，设置超时时间
	httplib::Client cli("https://mapapi.cloud.huawei.com");
	const std::string key = "DQEDAO6WrIiyzoBfJLfUfFXRJ7Gvj4v3PRIAr6wIBCDbXQu8FwpMhh4qXQsb1e3Yr6JYuqVDsVSI+hxfXcupo3ZzmqbgFR9DdWulJg==";
	std::string encodedKey = urlEncode(key);
	std::cout << encodedKey << std::endl;
	// 构建请求的URL
	std::string url = std::string("/mapApi/v1/mapService/getStaticMap?") +
		// 设置返回图片宽度为1024
		"width=1024&" +
		// 设置返回图片高度为512
		"height=512&" +
		// 地址信息，纬度：41.43206，经度：-81.38992
		"location=41.43206%2C-81.38992&" +
		// 设置清晰度比较尺为1
		"scale=1&" +
		// 设置缩放级别为5
		"zoom=5&" +
		// 设置图片格式：png
		"pattern=PNG&" +
		// 设置图片显示的语言为英语
		"language=en&" +
		// logo样式描述
		"logo=logoAnchor%3Abottomleft&" +
		// maker描述信息，纬度：41.43206，经度：-81.38992
		"markers=%7B41.43206%2C-81.38992%7D&" +
		// maker描述信息，纬度：43.43206，经度：-85.38992
		"markers=%7B43.43206%2C-85.38992%7D&" +
		// maker样式描述信息
		"markerStyles=size%3Atiny%7Ccolor%3Ablue%7Clabel%3Amjy&" +
		// path描述信息，{纬度，经度}：{41.43206，-81.38992}|{43.43206，-85.38992}
		"path=%7B41.43206%2C-81.38992%7D%7C%7B43.43206%2C-85.38992%7D&" +
		// path描述信息，样式为红色，{纬度，经度}：{41.43206，-81.38992}|{43.43206，-85.38992}|{39.43206，-83.38992}
		"path=fillcolor%3Ared%7C%7B41.43206%2C-81.38992%7D%7C%7B43.43206%2C-85.38992%7D%7C%7B39.43206%2C-83.38992%7D&" +
		// path样式描述信息
		"pathStyles=weight%3A1%7Ccolor%3A0x0000ff80%7Cfillcolor%3A0x0000ff80&" +
		"key=" + encodedKey;


	// 发送GET请求
	auto res = cli.Get(url.c_str());

	// 检查响应状态码
	if (res && res->status == 200) {
		// 打印出响应体（图片数据）
		std::cout << "Response status: " << res->status << std::endl;
		std::cout << "Response body of length " << res->body.size() << std::endl;
		// 这里可以将res->body写入到文件中，例如：
		std::ofstream out("static_map.jpg", std::ios::binary);
		out.write(res->body.c_str(), res->body.size());
		out.close();
	}
	else {
		std::cerr << "Failed to get static map: " << res->status << std::endl;
	}

	return 0;
}