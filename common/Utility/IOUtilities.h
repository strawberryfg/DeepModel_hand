#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

/*
This file contains some commonly used file I/O utilities.
It should not have any non-standard dependencies. Therefore it could be used in any projects by including the source code.
*/

/** 
This function finds an absolute path name given an absolute path 
and a relative path name.
@param rAbsolutePath an absolute path (which can be an absolute path name)
For example, "D:\data\mesh.obj" will be parsed as "D:\data\".
@param rRelativePathName a relative path name (e.g. ".\mesh.sel")
@param rAbsPathName the final absolute path name will be stored into this string.
For example, rAbsPathName = "D:\data\.\mesh.sel".
@return if successful, return true; otherwise return false.
Note: the current version cannot correctly handle the path which contains blanks.
*/
bool AbsolutePathName(const char* rAbsolutePath
	,const char* rRelativePathName, char* rAbsPathName);

// for generation of batch file names, ordered in numbers
class BatchFileName
{
public:
	string prefix, postfix, dir;
	int base_number, step_number;	
	int num_files;	

	BatchFileName();
	BatchFileName(istream & in);
	bool isValid() const;	// is it ok to extract file names from it?
	void Clear();
	void InverseFileNamingNumbers();	// useful when step_number is negative
	string FileName(int n) const;
	vector<string> GetAbsoluteFileNames(const char* dir_path = "") const;	

	friend istream & operator>> (istream & in, BatchFileName& names);
	friend ostream & operator<< (ostream & out, const BatchFileName& names);
};

// class IOToolBase implements template methods for general purpose load/save of plain text file in following format
// file header
// cmd 1 data 1
// cmd 2 data 2
// ...
// cmd n data n
// the file header, and all commands and data should be explained by derived class

// Derived classes should at least implement following methods for load/save
	// bool ProcessLoadingCommand(const string& cmd, ifstream& in_file, const char* file_name);
	// void WriteContent(ofstream& out_file) const;

class IOToolBase
{
public:	
	// return false if fail to load or save
	bool Load(const char* file_name);
	bool Save(const char* file_name) const;

	string ConfigFileName() const {	return config_file_name; }

protected:	
	virtual void BeginLoad(){}
	virtual bool LoadHead(ifstream& in_file, const char* file_name)	{ return true; }
	virtual void EndLoad()	{}

	// default WriteContent() does nothing
	virtual void WriteContent(ofstream& out_file) const;

	// Given a command in file stream in_file with file_name, function ProcessLoadingCommand() 
	// should read the corresponding data from the input stream.
	// It returns true if command is recognized and data is read successfully, otherwise it returns false
	// Future work: is it necessary to differentiate unknown cmd or known cmd but failure data loading?
	// default ProcessLoadingCommand() explains two commands for commenting text files
	// "#" comments the remaining content on the same line
	// "/*" comments the remaining content until "*/" is met
	virtual bool ProcessLoadingCommand(const string& cmd, ifstream& in_file, const char* file_name);

	// helper to handling comments
	void WriteComment(ofstream& out_file, const string& comment) const;

	string config_file_name;
};