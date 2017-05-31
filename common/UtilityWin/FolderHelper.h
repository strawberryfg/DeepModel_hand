#pragma once

#include <vector>
#include <string>
using namespace std;

#include <windows.h>

string ChooseFile();
bool ChooseFolder(char* szPath, const char* szCurrent = NULL);
bool CreateFolder(const char* szFolder);
bool CreateFolder(const WCHAR* wszFolder);

int FindAllFiles(const char* szPicDir, LPCSTR lpFileFilter, vector<string>& vecPicFile);
int GetPicFilesinDir(const char* szPicDir, vector<string>& vecPicFile);

int FindAllFiles(const WCHAR* wzPicDir, LPCWSTR lpFileFilter, vector<wstring>& vecPicFile);