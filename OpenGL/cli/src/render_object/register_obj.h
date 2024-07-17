#include "common.h"
#include "aabb_obj.h"
#include "axis_obj.h"
#include "box3d_obj.h"
#include "ellipsoid_obj.h"
#include "sphere_obj.h"
#include "rectangle2d_obj.h"

static std::unordered_map<std::string, RenderObjectBase> register_obj;
void InitRegisterObj() {
	register_obj["axis"] = AxisObj();
	register_obj["aabb"] = AABBObj();
	register_obj["ellipsoid"] = EllipsoidObj();
	register_obj["sphere"] = SphereObj();
	register_obj["box3d"] = BoxObj();
	register_obj["rectangle2d"] = RectangleObj();
}

std::unordered_map<std::string, RenderObjectBase>& GetRegisterObj()
{
	return register_obj;
}

