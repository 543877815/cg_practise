#include <iostream>
#include <fstream>
#include <string>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <vector>
#include <windows.h>
#include <commdlg.h>
#include "render_object.h"

bool GetOFNPath(TCHAR* szFile, bool isSave) {
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = TEXT("Text Files\0*.TXT\0All Files\0*.*\0");
	ofn.lpstrFile = szFile;
	ofn.nFilterIndex = 1;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = TEXT("txt\0json");
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	if (isSave) {
		return GetSaveFileName(&ofn);
	}
	else {
		return GetOpenFileName(&ofn);
	}
}

void LoadCameraConfig() {
	TCHAR szFile[MAX_PATH] = { 0 };
	if (GetOFNPath(szFile, false)) {
		std::stringstream ss;
		ss << szFile;
		std::ifstream file;
		file.open(ss.str(), std::ifstream::in);
		if (file.fail()) {
			std::cerr << "Unable to open file: " << ss.str() << std::endl;
			return;
		}

		file.close();
	}
}

struct JsonInfo {
	std::string obj_type;
	std::string vertex_shader;
	std::string fragment_shader;
	std::vector<std::string> uniform;
};

void ParseCameraConfig(const std::string& path, std::vector<JsonInfo>& json_infos) {
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
		JsonInfo json_info;
		if (!elem.IsObject()) {
			std::cerr << "Element is not an object." << std::endl;
			continue;
		}

		const rapidjson::Value& obj_info = elem;

		json_info.obj_type = obj_info["object"].GetString();
		json_info.vertex_shader = obj_info["vertex_shader"].GetString();
		json_info.fragment_shader = obj_info["fragment_shader"].GetString();

		const rapidjson::Value& uniform_info = obj_info["uniform"];
		if (uniform_info.IsArray()) {
			for (const auto& uniform : uniform_info.GetArray()) {
				json_info.uniform.emplace_back(uniform.GetString());
			}
		}
		else {
			std::cerr << "Uniform is not an array." << std::endl;
		}

		json_infos.emplace_back(json_info);
	}

}