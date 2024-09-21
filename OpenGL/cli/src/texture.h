#pragma once
#include<iostream>
#include<vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Texture {
public:
	std::vector<uint32_t> m_textures;
	Texture(size_t tex_num);
	size_t GenerateTexture(const char* path);
	uint32_t GetTexture(size_t idx);
	void BindTexture(size_t idx);
};

