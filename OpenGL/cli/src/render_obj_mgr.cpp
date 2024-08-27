#include "render_obj_mgr.h"


std::shared_ptr<RenderObjectManager> RenderObjectManager::instance = nullptr;
void RenderObjectManager::ParseCameraConfig(const std::string& path)
{
	std::ifstream ifs(path);
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	rapidjson::Document document;
	document.Parse(buffer.str().c_str());
	if (document.HasParseError()) {
		std::cerr << "JSON parse error: " << document.GetParseError() << std::endl;
		return;
	}

	if (!document.IsArray()) {
		std::cerr << "JSON is not an array." << std::endl;
		return;
	}

	for (const auto& elem : document.GetArray()) {
		RenderObjConfig config;
		if (!elem.IsObject()) {
			std::cerr << "Element is not an object." << std::endl;
			continue;
		}

		const rapidjson::Value& obj_info = elem;

		config.type = obj_info["object"].GetString();
		config.vertex_shader = obj_info["vertex_shader"].GetString();
		config.fragment_shader = obj_info["fragment_shader"].GetString();

		const rapidjson::Value& uniform_info = obj_info["uniform"];
		if (uniform_info.IsArray()) {
			for (const auto& uniform : uniform_info.GetArray()) {
				config.uniform.emplace_back(uniform.GetString());
			}
		}
		else {
			std::cerr << "Uniform is not an array." << std::endl;
		}

		obj_configs.emplace_back(config);
	}

}
