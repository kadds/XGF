// ShaderToCpp.cpp
// Data 2017/8/21 Time 17:05

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <d3dcompiler.h>
#include "gflags/gflags.h"
#include <time.h>
#include <iomanip>
#include <vector>

#pragma comment(lib,"d3dcompiler.lib")
#ifdef _DEBUG
#pragma comment(lib,"lib/Debug/gflagsd.lib")
#else
#pragma comment(lib,"lib/Release/gflags.lib")
#endif


typedef std::string string;
DEFINE_string(O, "out.cpp", "output source file name");
DEFINE_string(OH, "out.h", "output header file  name");
DEFINE_string(I, "", "input File List, split by ,");
DEFINE_string(VSEP, "VS", "VSEntryPointName");
DEFINE_string(VSV, "vs_4_0", "VSVersion");
DEFINE_string(PSEP, "PS", "PSEntryPointName");
DEFINE_string(PSV, "ps_4_0", "PSVersion");
DEFINE_string(ExStr, "", "Extern String");
DEFINE_string(NS, "ShaderConst", "NameSpace's Name");

wchar_t* CharToWchar(const char* c);
void split(string str, string pattern, std::vector<string> & output);
void TransformAShader(std::ofstream & os, std::ofstream & osh, string file, string name, string & entryPoint, string & version);
void TransformToFile(std::ofstream & os, std::ofstream & osh ,ID3DBlob & bb, string & name);
void OutputInformation(std::ofstream & os, std::ofstream & osh );


int main(int argc, char *argv[])
{
	google::SetVersionString("1.0");
	google::SetUsageMessage("Make Shader file to cpp file");
	google::ParseCommandLineFlags(&argc, &argv, false);
	google::CommandLineFlagInfo info;
	if ((google::GetCommandLineFlagInfo("I", &info) && info.is_default))
	{
		std::cerr << "Error! no input file";
		google::ShutDownCommandLineFlags();
		return -4;
	}
	
	string output = FLAGS_O;
	string outputh = FLAGS_OH;
	string input = FLAGS_I;
	string VSEntryPoint = FLAGS_VSEP;
	string VSVersion = FLAGS_VSV;
	string PSEntryPoint = FLAGS_PSEP;
	string PSVersion = FLAGS_PSV;
	char exeFullPath[MAX_PATH]; // MAX_PATH在WINDEF.h中定义了，等于260  
	memset(exeFullPath, 0, MAX_PATH);

	GetModuleFileNameA(NULL, exeFullPath, MAX_PATH);
	std::string Exefilepath = exeFullPath;
	Exefilepath = std::tr2::sys::path(Exefilepath).parent_path().string();
	
	std::vector<string> inputFiles;
	split(input, ",", inputFiles);
	std::ofstream out;
	std::ofstream outh;
	if (std::tr2::sys::path(output).is_relative())
	{
		output = Exefilepath + "\\" + output;
		output = std::tr2::sys::path(output).make_preferred().string();
	}
	if (std::tr2::sys::path(outputh).is_relative())
	{
		outputh = Exefilepath + "\\" + outputh;
	}
	out.open(output, std::ios::out);
	outh.open(outputh, std::ios::out);
	OutputInformation(out, outh);

	std::cout << "Transform ..." << std::endl << std::flush;
	string infile;
	for each (auto var in inputFiles)
	{
		if (std::tr2::sys::path(var).is_relative())
		{
			infile = Exefilepath + "\\" + var;
			std::cout << "handle relative dir..." << std::endl <<std::flush;
		}
		else
		{
			infile = var;
		}
		TransformAShader(out, outh, infile, std::tr2::sys::path(var).stem().string() + "VS", VSEntryPoint, VSVersion);
		TransformAShader(out, outh, infile, std::tr2::sys::path(var).stem().string() + "PS", PSEntryPoint, PSVersion);
	}
	out << "}" << std::endl;
	outh << "}" << std::endl;
	outh.flush();
	outh.close();
	out.flush();
	out.close();
	std::cout << "Output :" << output << "  " << outputh << "  OK "  << std::endl << std::flush;
	google::ShutDownCommandLineFlags();
    return 0;
}

void OutputInformation(std::ofstream & os, std::ofstream & osh)
{
	
	std::time_t t = std::time(NULL);
	std::tm timeinfo;
	localtime_s(&timeinfo, &t);
	os.imbue(std::locale("CHS"));
	os << "/*" << std::endl << "This source file is made by ShaderToCpp" << std::endl
		<< "DateTime:" << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << std::endl
		<< "*/" << std::endl << std::endl;
	osh << "/*" << std::endl << "This header file is made by ShaderToCpp" << std::endl
		<< "DateTime:" << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << std::endl
		<< "*/" << std::endl << std::endl;

	google::CommandLineFlagInfo info;
	if (!(google::GetCommandLineFlagInfo("ExStr", &info) && info.is_default))
	{
		os << "/*" << FLAGS_ExStr << "*/" << std::endl;
		osh << "/*" << FLAGS_ExStr << "*/" << std::endl;
	}
	os << "#include \""<< FLAGS_OH <<"\"" << std::endl;
	os << "namespace "<< FLAGS_NS <<"{" << std::endl;
	osh << "namespace " << FLAGS_NS << "{" << std::endl;
}
void TransformToFile(std::ofstream & os, std::ofstream & osh, ID3DBlob & bb, string & name)
{
	size_t sz = bb.GetBufferSize() / sizeof(byte);
	byte *bt = (byte * )bb.GetBufferPointer() ;
	os << "const unsigned char " << name << "[] = {";
	for (long i = 0; i < sz; i++)
	{
		os << "\'\\x"<< std::hex<< (unsigned)bt[i]<<"\'";
		if(i != sz -1 )
		{
			os << ",";
		}
		if(i % 30 == 0)
			os << std::endl << "    ";
	}
	os << "};" << std::endl << std::endl;
	os << "const unsigned int " << name << "Size = " << std::dec << static_cast<unsigned int>(sz) << ";" << std::endl;
	osh << "extern const unsigned char " << name << "[] ;" << std::endl;
	osh << "extern const unsigned int " << name << "Size;" << std::endl;

}
void TransformAShader(std::ofstream & os, std::ofstream & osh, string file,string name, string & entryPoint, string & version)
{
	ID3DBlob* pErrorBlob = nullptr;
	ID3DBlob* pBlob = nullptr;
	wchar_t * f = CharToWchar(file.c_str());
	HRESULT hr = D3DCompileFromFile(f, NULL,
		NULL, entryPoint.c_str(), version.c_str(), 0, 0, &pBlob, &pErrorBlob);
	
	if (FAILED(hr))
	{
		std::cout << "File: " << file ;
		if (pErrorBlob == nullptr)
			std::cout << "   Can't find it!";
		else
			std::cout << "   " <<(char*)pErrorBlob->GetBufferPointer() ;
		std::cout << std::endl << std::flush;
	}
	else
	{
		TransformToFile(os, osh, *pBlob, name);
		std::cout << "Transform File: "<< file <<" OK" << std::endl << std::flush;
	}
	delete[] f;
}
//字符串分割函数
void split(string str, string pattern, std::vector<string> & output)
{
	std::wstring::size_type pos;
	str += pattern;//扩展字符串以方便操作
	size_t size = str.size();

	for (size_t i = 0; i<size; i++)
	{
		pos = str.find(pattern, i);
		if (pos<size)
		{
			string s = str.substr(i, pos - i);
			output.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	
}
wchar_t* CharToWchar(const char* c)
{
	wchar_t *m_wchar;
	int len = MultiByteToWideChar(CP_ACP, 0, c,static_cast<int>(strlen(c)), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, c, strlen(c), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}