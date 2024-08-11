#include "register_obj.h"

namespace renderable {
#define DECLEAR_REGISTER_OBJECT(OBJNAME) \
	std::shared_ptr<RenderObjectBase> RenderObjectFactory::Create##OBJNAME##(){ \
	return std::make_shared<##OBJNAME##Obj>(); \
}

	DECLEAR_REGISTER_OBJECT(Axis);
	DECLEAR_REGISTER_OBJECT(Sphere);

	std::unordered_map<std::string, std::function<std::shared_ptr<RenderObjectBase>()>> register_obj{
		{"axis", RenderObjectFactory::CreateAxis},
		{"sphere", RenderObjectFactory::CreateSphere}
	};

	std::unordered_map<std::string, std::function<std::shared_ptr<RenderObjectBase>()>>& GetRegisterRenderObj()
	{
		return register_obj;
	}





};