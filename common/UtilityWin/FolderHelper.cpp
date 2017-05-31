#ifdef _AFXDLL
#include <afxdlgs.h>	// must be at first for compilation
#endif


#include <shlobj.h>
#include <atlcomcli.h>
#include <io.h>
#include <tchar.h>	// for _tfinddata_t
#include <direct.h> /// for _mkdir

#include "FolderHelper.h"

#ifdef _AFXDLL

string ChooseFile()
{
	CFileDialog dlg(true);
	//return (dlg.DoModal() == IDOK) ? string((LPCTSTR)dlg.GetPathName()) : string("");
	return "";
}

#else

string ChooseFile() { return string(""); }

#endif

bool ChooseFolder(LPTSTR szPath, LPCTSTR szCurrent)
{
	HWND hParent = NULL;
	BROWSEINFO bi;
	::ZeroMemory(&bi, sizeof(bi));

	bi.hwndOwner = hParent;
	bi.pszDisplayName = szPath;
	bi.lpszTitle = szCurrent;
	bi.pidlRoot = 0;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

	bool success = false;
	LPITEMIDLIST pItem = ::SHBrowseForFolder(&bi);
	if (pItem != NULL)
	{
		success = (::SHGetPathFromIDList(pItem, szPath) == TRUE);

		if (success)
		{
			CComPtr<IMalloc> pMalloc;
			if (SUCCEEDED(::SHGetMalloc(&pMalloc)))
				pMalloc->Free(pItem);
		}
	}

	return success;
}

int FindAllFiles(const char* szPicDir, LPCSTR lpFileFilter, vector<string>& vecPicFile)
{	
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = ::FindFirstFileA((string(szPicDir) + "\\" + lpFileFilter).c_str(), &FindFileData);

	if (INVALID_HANDLE_VALUE == hFind) return 0;

	int count = 0;
	BOOL next = TRUE;
	while (next)
	{
		if (FILE_ATTRIBUTE_DIRECTORY == (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			continue;

		vecPicFile.push_back(string(szPicDir) + "\\" + FindFileData.cFileName);
		count++;

		next = ::FindNextFileA(hFind, &FindFileData);
	}
	::FindClose(hFind);

	return count;
}

int FindAllFiles(const WCHAR* wzPicDir, LPCWSTR lpFileFilter, vector<wstring>& vecPicFile)
{
	WIN32_FIND_DATAW FindFileData;
	HANDLE hFind = ::FindFirstFileW((wstring(wzPicDir) + L"\\" + lpFileFilter).c_str(), &FindFileData);

	if (INVALID_HANDLE_VALUE == hFind) return 0;

	int count = 0;
	BOOL next = TRUE;
	while (next)
	{
		if (FILE_ATTRIBUTE_DIRECTORY == (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			continue;

		vecPicFile.push_back(wstring(wzPicDir) + L"\\" + FindFileData.cFileName);
		count++;

		next = ::FindNextFileW(hFind, &FindFileData);
	}
	::FindClose(hFind);

	return count;
}

int GetPicFilesinDir(const char* szPicDir, vector<string>& vecPicFile)
{
	int count = 0;
	count += FindAllFiles(szPicDir, "*.jpg", vecPicFile);
	count += FindAllFiles(szPicDir, "*.jpeg", vecPicFile);
	count += FindAllFiles(szPicDir, "*.bmp", vecPicFile);
	count += FindAllFiles(szPicDir, "*.gif", vecPicFile);
	count += FindAllFiles(szPicDir, "*.png", vecPicFile);
	return count;
}

bool ThereIsFile(const char* szfilename)
{
	//_tfinddata_t ft;
	//return -1 != _tfindfirst(szfilename, &ft);
	_finddata_t ft;
	return -1 != _findfirst(szfilename, &ft);
}

bool CreateFolder(const char* szFolder)
{
	if (!ThereIsFile(szFolder))
		//return 0 == _tmkdir(szFolder);
		return 0 == _mkdir(szFolder);
	else return true;
}


bool CreateFolder(const WCHAR* wszFolder)
{
	return 0 == _wmkdir(wszFolder);
}