#pragma once

#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

template<class ValueType>
bool SaveBinary(const char* filename, const ValueType* pBuffer, const unsigned int count)
{
	ofstream out_file(filename, ios::out | ios::binary);
	if (!out_file) 
	{
		cout << "can't open file " << filename << endl;
		return false;
	}

	out_file.write((const char*)pBuffer, count * sizeof(ValueType));

	if (out_file) 
		return true;
	else
	{
		cout << "fail to save binary to " << filename << endl;
		return false;
	}
}

template<class ValueType>
bool SaveBinary(const char* filename, const vector<ValueType>& data)
{
	return SaveBinary(filename, &data[0], data.size());
}

template<class ValueType>
bool LoadBinary(const char* filename, ValueType* pBuffer, const unsigned int count)
{
	ifstream in_file(filename, ios::in | ios::binary);
	if (!in_file)
	{
		cout << "can't open file " << filename << endl;
		return false;
	}

	in_file.read((char*)pBuffer, count * sizeof(ValueType));

	if (in_file) 
		return true;
	else 
	{
		cout << "fail to load binary from " << filename << endl;
		return false;
	}
}

template<class ValueType>
bool LoadBinary(const char* filename, vector<ValueType>& data)
{
	return LoadBinary(filename, &data[0], data.size());
}

template<class ValueType>
bool LoadBinary(const char* filename, vector<ValueType>& buffer, unsigned int& rows, unsigned int& cols)
{
	ifstream in(filename, ios::in | ios::binary);
	if (!in)
	{
		cout << "can't open file " << filename << endl;
		return false;
	}

	in.read((char*)&rows, sizeof(unsigned int));
	in.read((char*)&cols, sizeof(unsigned int));
	if (!in)
	{
		cout << "fail to load binary row and col number from " << filename << endl;
		return false;
	}

	buffer.resize(rows * cols);
	in.read((char*)&buffer[0], sizeof(ValueType) * rows * cols);

	if (in)
		return true;
	else
	{
		cout << "fail to load binary from " << filename << endl;
		return false;
	}
}

#include <string>
#include <vector>

// load plain text files into lines
// line1
// line2
// line3
inline bool LoadLines(const char* filename, vector<string>& lines)
{
	lines.clear();

	ifstream in(filename);
	if (!in)
	{
		cout << "can't open file " << filename << endl;
		return false;
	}

	while (1)
	{
		string line;
		getline(in, line);

		if (line == "")
			break;

		lines.push_back(line);
	}
	return true;
}

// save lines in format like
// line11 line12 line
// line21
// line31 line32 line33
inline bool SaveLines(const char* filename, const vector<vector<string>>& lines)
{
	ofstream out(filename);
	if (!out)
	{
		cout << "can't open file " << filename << endl;
		return false;
	}

	out << lines.size() << endl;
	for(unsigned int n = 0; n < lines.size(); n++)
	{
		out << lines[n].size();
		for(unsigned i = 0; i < lines[n].size(); i++)
			out << " " << lines[n][i];
		out << endl;
	}

	return true;
}

// save lines in format like
// line11 line12 line
// line21
// line31 line32 line33
inline bool LoadLines(const char* filename, vector<vector<string>>& lines)
{
	lines.clear();

	ifstream in(filename);

	if (!in)
	{
		cout << "can't open file " << filename << endl;
		return false;
	}

	int NLines;
	in >> NLines;
	if (!in)
	{
		cout << "fail to load number of lines from " << filename << endl;
		return false;
	}

	lines.resize(NLines);
	for(int n = 0; n < NLines; n++)
	{
		int N;
		in >> N;
		if (!in)
		{
			cout << "fail to load line " << n << "'s length from " << filename << endl;
			return false;
		}
		lines[n].resize(N);
		for(int i = 0; i < N; i++)
			in >> lines[n][i];
		if (!in)
		{
			cout << "fail to load line " << n << " from " << filename << endl;
			return false;
		}
	}

	return true;
}