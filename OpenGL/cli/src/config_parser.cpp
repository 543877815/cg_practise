#include "config_parser.h"

bool GetOFNPath(TCHAR* szFile, bool isSave)
{
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

void LoadCameraConfig()
{
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