#include "IOUtilities.h"

bool AbsolutePathName(const char* rAbsolutePath
	,const char* rRelativePathName, char* rAbsPathName)
{	
	if (!rAbsolutePath || !rRelativePathName) return false;

	// rRelativePathName is already an absolute path name
	if (strstr(rRelativePathName, ":"))		
	{
		strcpy(rAbsPathName, rRelativePathName);
		return true;
	}

	int i, size = (int)strlen(rAbsolutePath);
	for (i = size - 1; i >= 0; i--)
		if (rAbsolutePath[i] == '\\' || rAbsolutePath[i] == '/') break;
	if (i > 0)
	{
		strncpy(rAbsPathName, rAbsolutePath, i + 1);	// copy a substring
		rAbsPathName[i + 1] = '\0';						// flag: end of string
		strcat(rAbsPathName, rRelativePathName);
	}
	else
		strcpy(rAbsPathName, rRelativePathName);
	return true;	// successful	
}

//////////////////////////////////////////////////////////////////
/////////////   implementation of BatchFileName   ////////////////
//////////////////////////////////////////////////////////////////

BatchFileName::BatchFileName() : num_files(0)
	,base_number(0), step_number(1)
{
}

BatchFileName::BatchFileName(istream & in) : num_files(0)
	,base_number(0), step_number(1)
{
	in >> *this;
}

void BatchFileName::Clear()
{
	num_files = 0;
	base_number = 0;
	step_number = 1;
	prefix = "";
	postfix = "";
	dir = "";
}

void BatchFileName::InverseFileNamingNumbers()
{
	base_number += (num_files-1) * step_number;
	step_number = -step_number;
}

string BatchFileName::FileName(int n) const
{
	char buffer[255];
	itoa(base_number + n * step_number, buffer, 10);
	return dir + prefix + buffer + postfix;
}

vector<string> BatchFileName::GetAbsoluteFileNames(const char* dir_path) const
{
	vector<string> file_names;
	for(int n = 0; n < num_files; n++)
	{
		string str = FileName(n);
		if (dir.empty())	// pre-concatenate dir_path
		{				
			char absPathName[255];
			if (AbsolutePathName(dir_path, str.c_str(), absPathName))
				file_names.push_back(absPathName);
			else cout << "invalid dir path : " << dir_path << endl;
		}
		else file_names.push_back(str);
	}

	return file_names;
}

bool BatchFileName::isValid() const
{
	return (num_files > 0) && (dir != "");
}

istream & operator>> (istream & in, BatchFileName& names)
{	
	string flag;
	in >> flag;	// "{"
	char buffer[255];
	names.Clear();
	while (true)
	{			
		in >> flag;
		if ("#" == flag) in.getline(buffer, 255);
		else if ("NumFiles" == flag) in >> names.num_files;
		else if ("Dir" == flag) in >> names.dir;
		else if ("Prefix" == flag) in >> names.prefix;
		else if ("Postfix" == flag) in >> names.postfix;
		else if ("BaseNumber" == flag) in >> names.base_number;
		else if ("StepNumber" == flag) in >> names.step_number;
		else if ("}" == flag) break;
		else cout << "unknown command in reading batch file names : " << flag << endl;
	}

	return in;
}

ostream & operator<< (ostream & out, const BatchFileName& names)
{
	out << "{" << endl;
	out << "NumFiles " << names.num_files << endl;
	out << "Dir " << names.dir << endl;
	out << "Prefix " << names.prefix << endl;
	out << "Postfix " << names.postfix << endl;
	out << "BaseNumber " << names.base_number << endl;
	out << "StepNumber " << names.step_number << endl;
	out << "}" << endl;

	return out;
}

//////////////////////////////////////////////////////////////////
/////////////   implementation of IOToolBase   ////////////////
//////////////////////////////////////////////////////////////////

bool IOToolBase::Save(const char* file_name) const
{
	ofstream out_file(file_name);
	if (!out_file) 
	{
		cout << "IOToolBase::Save() error : cannot save file as " << file_name << endl;
		return false;
	}

	cout << "saving to file " << file_name << "..." << endl;

	WriteContent(out_file);

	out_file.close();
	return true;
}

void IOToolBase::WriteContent(ofstream& out_file) const
{
	//cout << "ExampleCmd ExampleData" << endl;
	//cout << "unimplemented WriteContent() of IOToolBase" << endl;
	//cout << "you should implement your own save function in derived class" << endl;
}

bool IOToolBase::Load(const char* file_name)
{
	ifstream in_file(file_name);
	if (!in_file) 
	{
		cout << "IOToolBase::Load() error : file " << file_name << " not exist" << endl;
		return false;
	}

	config_file_name = file_name;
	cout << "loading from file " << file_name << "..." << endl;

	BeginLoad();

	if (!LoadHead(in_file, file_name))
	{
		cout << "cannot load file head appropriately" << endl;
	}

	string cmd;
	while (in_file >> cmd)
	{	
		if (!ProcessLoadingCommand(cmd, in_file, file_name))
			cout << "unprocessed command : " << cmd << endl;
	}
	EndLoad();

	in_file.close();
	return true;
}

bool IOToolBase::ProcessLoadingCommand(const string& cmd, ifstream& in_file, const char* file_name)
{
	char buffer[255];
	if ("#" == cmd) 
	{
		in_file.getline(buffer, 255);
		return true;
	}

	string flag;
	if ("/*" == cmd)
	{
		do { in_file >> flag; }	while (in_file && ("*/" != flag));
		// if file is still valid, "*/" has been read and "/*" command is processed successfully
		return in_file ? true : false;
	}

	return false;
}

void IOToolBase::WriteComment(ofstream& out_file, const string& comment) const
{
	out_file << "# " << comment << endl;
}