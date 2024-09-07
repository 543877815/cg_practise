#pragma once
#include <memory>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "render_objs/register_render_obj.h"
#include <format>

class RenderObjectManager {
public:
	struct RenderObjConfig {
		std::string type;
		std::string vertex_shader;
		std::string fragment_shader;
		std::vector<std::string> uniform;
	};

	RenderObjectManager();
	RenderObjectManager(const RenderObjectManager&) = delete;
	RenderObjectManager& operator=(const RenderObjectManager&) = delete;
	std::vector<RenderObjConfig>& GetObjConfigs();
	std::vector<std::shared_ptr<renderable::RenderObjectBase>>& GetRenderObjs();
	static std::shared_ptr<RenderObjectManager> GetInstance();
	void InitRenderObj(const std::string& config);

private:
	void ParseCameraConfig(const std::string& path);

private:
	static std::shared_ptr<RenderObjectManager> instance;
	std::vector<RenderObjConfig> obj_configs;
	std::unordered_map<std::string, std::function<std::shared_ptr<renderable::RenderObjectBase>()>> register_render_obj;
	std::vector<std::shared_ptr<renderable::RenderObjectBase>> render_objs;

};

