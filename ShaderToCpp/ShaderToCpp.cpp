// ShaderToCpp.cpp
// convert hlsl to hlslcode

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <d3dcompiler.h>
#include "gflags/gflags.h"
#include <time.h>
#include <iomanip>
#include <vector>
#include <io.h>
#include <Windows.h>
#include <sstream>
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
DEFINE_string(DIR, "", "input File dir, split by ,");
DEFINE_string(VSEP, "VS", "VSEntryPointName");
DEFINE_string(VSV, "vs_4_0", "VSVersion");
DEFINE_string(PSEP, "PS", "PSEntryPointName");
DEFINE_string(PSV, "ps_4_0", "PSVersion");
DEFINE_string(GSEP, "GS", "GSEntryPointName");
DEFINE_string(GSV, "gs_4_0", "GSVersion");
DEFINE_string(CSEP, "CS", "CSEntryPointName");
DEFINE_string(CSV, "cs_5_0", "CSVersion");
DEFINE_string(ExStr, "", "Extern String");
DEFINE_string(NS, "ShaderConst", "Iner NameSpace");
DEFINE_string(ANS, "XGF", "NameSpace");
DEFINE_string(IP, "../../Include/", "include Path at cpp file");

wchar_t* CharToWchar(const char* c);
void split(string str, string pattern, std::vector<string> & output);
void TransformAShader(std::ostream & os, std::ostream & osh, string file, string name, string & entryPoint, string & version);
void TransformToFile(std::ostream & os, std::ostream & osh ,ID3DBlob & bb, string & name);
void OutputInformation(std::ostream & os, std::ostream & osh );
HANDLE  gConsole;
HANDLE  gErrorConsole;
int gErrorCount = 0;
int gWarningCount = 0;

int main(int argc, char *argv[])
{
	google::SetVersionString("1.1");
	google::SetUsageMessage("convert hlsl file to hlslcode");
	google::ParseCommandLineFlags(&argc, &argv, false);
	google::CommandLineFlagInfo info, info2;
	if ((google::GetCommandLineFlagInfo("I", &info) && info.is_default) && google::GetCommandLineFlagInfo("DIR", &info2) && info2.is_default)
	{
		std::cerr << "Error! no input file";
		google::ShutDownCommandLineFlags();
		return -4;
	}
	gConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	gErrorConsole = GetStdHandle(STD_ERROR_HANDLE);
	//cpp file
	string output = FLAGS_O;
	//h file
	string outputh = FLAGS_OH;


	string input = FLAGS_I;
	string VSEntryPoint = FLAGS_VSEP;
	string VSVersion = FLAGS_VSV;
	string PSEntryPoint = FLAGS_PSEP;
	string PSVersion = FLAGS_PSV;
	string GSEntryPoint = FLAGS_GSEP;
	string GSVersion = FLAGS_GSV;
	string CSEntryPoint = FLAGS_CSEP;
	string CSVersion = FLAGS_CSV;
	string inputDir = FLAGS_DIR;
	char exeFullPath[MAX_PATH];
	memset(exeFullPath, 0, MAX_PATH);

	GetModuleFileNameA(NULL, exeFullPath, MAX_PATH);
	std::string Exefilepath = exeFullPath;
	Exefilepath = std::experimental::filesystem::path(Exefilepath).parent_path().string();
	
	std::vector<string> inputFiles;
	if (input != "")
	{
		split(input, ",", inputFiles);
	}
	if (inputDir != "")
	{
		std::vector<string> inputDirs;
		split(inputDir, ",", inputDirs);
		while (!inputDirs.empty())//搜索目录下所有文件
		{
			string str = inputDirs.back();
			std::experimental::filesystem::path path(str);
			std::cout << "Finding files at Dir " << str << std::endl;
			inputDirs.pop_back();
			_finddata_t fd;
			auto handle = _findfirst(str.c_str(), &fd);
			if(handle != -1L)
			{
				if (fd.attrib != _A_SUBDIR)
					inputFiles.push_back(path.parent_path().string() + '/' + fd.name);
				std::cout << "file find:" << path.parent_path().string() + '/' + fd.name << std::endl;
				while (_findnext(handle, &fd) == FALSE)
				{
					if(fd.attrib != _A_SUBDIR)
						inputFiles.push_back(path.parent_path().string() + '/' + fd.name);
					std::cout << "file find:" << path.parent_path().string() + '/' + fd.name << std::endl;
				}
				_findclose(handle);
			}
			else
				std::cout << "Dir " << str << " can't find file" << std::endl;
		}
	}
	std::stringstream out;
	std::stringstream outh;
	
	
	OutputInformation(out, outh);

	std::cout << "Transform ..." << std::endl << std::flush;
	string infile;
	for each (auto var in inputFiles)
	{
		if (std::experimental::filesystem::path(var).is_relative())
		{
			infile = Exefilepath + "\\" + var;
			std::cout << "handle relative dir..." << std::endl <<std::flush;
		}
		else
		{
			infile = var;
		}
		TransformAShader(out, outh, infile, std::experimental::filesystem::path(var).stem().string() + "VS", VSEntryPoint, VSVersion);
		TransformAShader(out, outh, infile, std::experimental::filesystem::path(var).stem().string() + "PS", PSEntryPoint, PSVersion);
		TransformAShader(out, outh, infile, std::experimental::filesystem::path(var).stem().string() + "GS", GSEntryPoint, GSVersion);
		TransformAShader(out, outh, infile, std::experimental::filesystem::path(var).stem().string() + "CS", CSEntryPoint, CSVersion);
	}
	out << "}" << std::endl << "}" << std::endl;
	outh << "}" << std::endl << "}" << std::endl;
	
	SetConsoleTextAttribute(gConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	if (gErrorCount == 0)//无错误，输出到文件
	{
		std::ofstream outfile;
		std::ofstream outhfile;
		
		if (std::experimental::filesystem::path(output).is_relative())
		{
			output = Exefilepath + "\\" + output;
			output = std::experimental::filesystem::path(output).make_preferred().string();
		}
		if (std::experimental::filesystem::path(outputh).is_relative())
		{
			outputh = Exefilepath + "\\" + outputh;
			outputh = std::experimental::filesystem::path(outputh).make_preferred().string();
		}

		outfile.open(output, std::ios::out);
		outhfile.open(outputh, std::ios::out);
		outhfile << outh.str();
		outfile << out.str();
		outhfile.flush();
		outhfile.close();
		outfile.flush();
		outfile.close();
		std::cout << std::endl << "Output :" << output << "  \n" << outputh << "  OK " << std::endl;
	}
	else
	{
		std::cout << "Can't Output" << std::endl;
	}
	google::ShutDownCommandLineFlags();
	std::cout << gWarningCount << " Warning(s), " << gErrorCount << " Error(s)" << std::endl;
	
    return 0;
}
//开始信息头
void OutputInformation(std::ostream & os, std::ostream & osh)
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
	os << "#include \""<< FLAGS_IP << std::experimental::filesystem::path(FLAGS_OH).filename() <<"\"" << std::endl;
	os << "namespace " << FLAGS_ANS << "{" << std::endl;
	os << "namespace "<< FLAGS_NS <<"{" << std::endl;
	osh << "namespace " << FLAGS_ANS << "{" << std::endl;
	osh << "namespace " << FLAGS_NS << "{" << std::endl;
}
//写入数据
void TransformToFile(std::ostream & os, std::ostream & osh, ID3DBlob & bb, string & name)
{
	size_t sz = bb.GetBufferSize() / sizeof(byte);
	byte *bt = (byte * )bb.GetBufferPointer() ;
	os << "const unsigned char " << name << "[] = {";
	for (long i = 0; i < sz; i++)
	{
		os << "u\'\\x"<< std::hex<< (unsigned)bt[i]<<"\'";
		if(i != sz -1 )
		{
			os << ",";
		}
		if(i % 30 == 0)
			os << std::endl << "    ";
	}
	os << "};" << std::endl << std::endl << std::hex;
	os << "const unsigned int " << name << "Size = 0x" << static_cast<unsigned int>(sz) << ";" << std::endl;
	osh << "extern const unsigned char " << name << "[] ;" << std::endl;
	osh << "extern const unsigned int " << name << "Size;" << std::endl;

}
void TransformAShader(std::ostream & os, std::ostream & osh, string file,string name, string & entryPoint, string & version)
{
	ID3DBlob* pErrorBlob = nullptr;
	ID3DBlob* pBlob = nullptr;
	wchar_t * f = CharToWchar(file.c_str());
	HRESULT hr = D3DCompileFromFile(f, NULL,
		NULL, entryPoint.c_str(), version.c_str(), 0, 0, &pBlob, &pErrorBlob);
	
	if (FAILED(hr))
	{
		SetConsoleTextAttribute(gErrorConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
		
		if (pErrorBlob == nullptr)
		{
			std::cerr << "File: " << file << " Can't find file" << std::endl;
			gErrorCount++;
		}
			
		else
		{
			
			string buffer = (char*)pErrorBlob->GetBufferPointer();
			if (buffer.find("error X3501:") != buffer.npos)
			{
				gWarningCount++;
				SetConsoleTextAttribute(gConsole, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
				std::cout << "File: " << file << " Warn:" << entryPoint << " Entrypoint not found" << std::endl;
			}
			else
			{
				gErrorCount++;
				std::cerr << " " << buffer;
			}
		}
		SetConsoleTextAttribute(gConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		
	}
	else
	{
		TransformToFile(os, osh, *pBlob, name);
		std::cout << "File: "<< file << " " << entryPoint << " compile ok" << std::endl;
	}
	delete[] f;
}
//字符串分割函数
void split(string str, string pattern, std::vector<string> & output)
{
	std::wstring::size_type pos;
	str += pattern;
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