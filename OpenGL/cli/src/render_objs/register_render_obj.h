#pragma once
#include "common.h"
#include "aabb_obj.h"
#include "axis_obj.h"
#include "box_obj.h"
#include "ellipsoid_obj.h"
#include "sphere_obj.h"
#include "rectangle2d_obj.h"
#include "map_obj.h"
#include <functional>
#include <memory>

RENDERABLE_BEGIN

class RenderObjectFactory {
public:
	static std::shared_ptr<RenderObjectBase> CreateAxis();
	static std::shared_ptr<RenderObjectBase> CreateSphere();
	static std::shared_ptr<RenderObjectBase> CreateBox();
	static std::shared_ptr<RenderObjectBase> CreateRectangle2D();
	static std::shared_ptr<RenderObjectBase> CreateEllipsoid();
	static std::shared_ptr<RenderObjectBase> CreateMap();
};



extern std::unordered_map<std::string, std::function<std::shared_ptr<RenderObjectBase>()>> register_obj;
extern std::unordered_map<std::string, std::function<void()>> register_imguicallback;
std::unordered_map<std::string, std::function<std::shared_ptr<RenderObjectBase>()>>& GetRegisterRenderObj();
RENDERABLE_END