#include "render_obj_mgr.h"


std::shared_ptr<RenderObjectManager> RenderObjectManager::instance = nullptr;
RenderObjectManager::RenderObjectManager()
{
	register_render_obj = renderable::GetRegisterRenderObj();
}

std::vector<RenderObjectManager::RenderObjConfig>& RenderObjectManager::GetObjConfigs()
{
	return obj_configs;
}

RenderObjectManager::RenderObjConfig& RenderObjectManager::GetObjConfig(size_t idx)
{
	return obj_configs[idx];
}

std::vector<std::shared_ptr<renderable::RenderObjectBase>>& RenderObjectManager::GetRenderObjs()
{
	return render_objs;
}

std::shared_ptr<RenderObjectManager> RenderObjectManager::GetInstance()
{
	if (instance == nullptr) {
		instance = std::make_shared<RenderObjectManager>();
	}
	return instance;
}

void RenderObjectManager::InitRenderObjs(std::vector<std::string>& configs) {
	for (auto config : configs) {
		InitRenderObj(config);
	}
}

void RenderObjectManager::InitRenderObj(const std::string& config)
{
	ParseCameraConfig(config);
	for (size_t i = 0; i < obj_configs.size(); i++) {
		auto& obj = obj_configs[i];
		if (register_render_obj.count(obj.type)) {
			render_objs.emplace_back(register_render_obj[obj.type]());
		}
		else {
			std::cerr << std::format("Error occur: the object: {} is not regisered yet.", obj.type) << std::endl;
		}
	}
}

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
				config.uniform.insert(uniform.GetString());
			}
		}
		else {
			std::cerr << "Uniform is not an array." << std::endl;
		}

		obj_configs.emplace_back(config);
	}

}
