#pragma once
#include "common.h"

RENDERABLE_BEGIN

class MapObj : public RenderObject<float, uint32_t> {
public:
	struct Point {
		float x = 0.0, y = 0.0, z = 0.0;
	};

	MapObj();
	void DrawObj(const std::unordered_map<std::string, std::any>& uniform);

private:
	void SetUpGLStatus() {
		glEnable(GL_DEPTH_TEST);
	}

	void SetUpTexture(int num = 0) override;
	void SetUpShader() override;
	void SetUpData() override;
};

RENDERABLE_END