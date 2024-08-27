#pragma once
#include <memory>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "render_objs/register_obj.h"

class RenderObjectManager {
public:
	struct RenderObjConfig {
		std::string type;
		std::string vertex_shader;
		std::string fragment_shader;
		std::vector<std::string> uniform;
	};

	RenderObjectManager() {
		register_render_obj = renderable::GetRegisterRenderObj();
	}

	RenderObjectManager(const RenderObjectManager&) = delete;

	RenderObjectManager& operator=(const RenderObjectManager&) = delete;

	std::vector<RenderObjConfig>& GetObjConfigs() {
		return obj_configs;
	}

	std::vector<std::shared_ptr<RenderObjectBase>> &GetRenderObjs() {
		return render_objs;
	}

	static std::shared_ptr<RenderObjectManager> GetInstance() {
		if (instance == nullptr) {
			instance = std::make_shared<RenderObjectManager>();
		}
		return instance;
	}

	void InitRenderObj(const std::string& config) {
		ParseCameraConfig(config);
		for (size_t i = 0; i < obj_configs.size(); i++) {
			auto& obj = obj_configs[i];
			render_objs.emplace_back(register_render_obj[obj.type]());
		}
	}


private:
	void ParseCameraConfig(const std::string& path);

private:
	static std::shared_ptr<RenderObjectManager> instance;
	std::vector<RenderObjConfig> obj_configs;
	std::unordered_map<std::string, std::function<std::shared_ptr<RenderObjectBase>()>> register_render_obj;
	std::vector<std::shared_ptr<RenderObjectBase>> render_objs;

};

