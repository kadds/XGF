#include "../../Include/ShaderManager.hpp"
#include <fstream>
#include "../../Include/Logger.hpp"

namespace XGF
{
	FileData::FileData(size_t size, std::unique_ptr<char[]> ptr): data(std::move(ptr)), size(size)
	{
	}

	FileData::FileData(): data(nullptr), size(0)
	{
	}

	size_t FileData::GetSize() const
	{
		return size;
	}

	void* FileData::GetPtr() const
	{
		return data.get();
	}

	FileData::FileData(FileData&& fileData) noexcept
	{
		this->data = std::move(fileData.data);
		this->size = fileData.size;
		fileData.size = 0;
	}

	bool CompileDefine::operator==(const CompileDefine& def) const
	{
		return name == def.name && definition == def.definition;
	}

	size_t CompileDefine::hash() const
	{
		std::hash<std::string> hs;
		size_t const h1(hs(name));
		size_t const h2(hs(definition));
		return h1 ^ (h2 << 1);
	}

	CompileDefine::CompileDefinePair & CompileDefine::CompileDefinePair::operator=(std::string&& str)
	{
		def->definition = std::move(str);
		return *this;
	}

	std::unique_ptr<char*> CompileDefines::GetDataPtr() const
	{
		std::unique_ptr<char *> ptr = std::unique_ptr<char *>(new char*[defines.size() * 2 + 2]);
		size_t i = 0;
		for (auto& it : defines)
		{
			*static_cast<char **>(ptr.get() + i++) = const_cast<char *>(it.name.c_str());
			*static_cast<char **>(ptr.get() + i++) = const_cast<char *>(it.definition.c_str());
		}
		*static_cast<char **>(ptr.get() + i++) = nullptr;
		*static_cast<char **>(ptr.get() + i++) = nullptr;
		return ptr;
	}

	CompileDefine::CompileDefinePair CompileDefines::operator[](std::string&& key)
	{
		defines.emplace_back(std::move(key));
		return CompileDefine::CompileDefinePair(&defines.back());
	}

	void CompileDefines::Remove(const std::string& key)
	{
		auto td = std::find_if(defines.begin(), defines.end(), [&key](const CompileDefine& def)
		{
			return key == def.name;
		});
		if (td != defines.end())
		{
			defines.erase(td);
		}
	}

	const std::vector<CompileDefine>& CompileDefines::GetDefines() const
	{
		return defines;
	}

	bool CompileDefines::operator==(const CompileDefines& defs) const
	{
		return defines == defs.defines;
	}

	bool CompileDefines::operator!=(const CompileDefines& defs) const
	{
		return !operator==(defs);
	}

	size_t CompileDefines::hash() const
	{
		size_t hash = 0;
		for (auto& it : GetDefines())
		{
			hash = it.hash() ^ (hash << 1);
		}
		return hash;
	}

	FileData DiskLoader::Load(std::string url)
	{
		std::ifstream ifile(url, std::ios::in | std::ios::binary);
		if (!ifile)
		{
			XGF_Warn(IO, "can't find file ", url);
			return FileData();
		}
		auto pbuf = ifile.rdbuf();
		size_t size = static_cast<size_t>(pbuf->pubseekoff(0, std::ios::end, std::ios::in));
		auto cdata = new char[size];
		FileData fileData(size, std::unique_ptr<char[]>(cdata));
		ifile.seekg(0, std::ios::beg);
		pbuf->pubseekpos(0, std::ios::in);
		pbuf->sgetn(cdata, size);
		ifile.close();
		return fileData;
	}
	const int ShaderConfiguration::salt = 1073741827;

	bool ShaderConfiguration::operator==(const ShaderConfiguration& sc) const
	{
		return sourceName == sc.sourceName && version == sc.version && defines == sc.defines;
	}

	bool ShaderConfiguration::operator!=(const ShaderConfiguration& sc) const
	{
		return !operator==(sc);
	}

	size_t ShaderConfiguration::hash() const
	{
		std::hash<std::string> strHash;
		std::hash<int> intHash;
		size_t hash = strHash(sourceName) ^
		(strHash(version) ^
			(defines.hash() ^
				(intHash(salt) << 1) << 1) << 1);
		return hash;
	}

	bool ShaderConfigurationWrapper::operator==(const ShaderConfigurationWrapper& c) const
	{
		return url == c.url && typeIdentifier == c.typeIdentifier && configuration == c.configuration;
	}

	bool ShaderConfigurationWrapper::operator!=(const ShaderConfigurationWrapper& c) const
	{
		return !operator==(c);
	}

	ShaderConfigurationWrapper::ShaderConfigurationWrapper(const std::string & url, const ShaderConfiguration& config, size_t typeId): configuration(config),
	                                                                           typeIdentifier(typeId), url(url)
	{
	}

	ShaderManager::ShaderManager()
	{
	}

	FileData* ShaderManager::FindCacheData(const std::string& url)
	{
		auto it = mDataCache.find(url);
		if (it != mDataCache.end())
		{
			return &it->second;
		}
		return nullptr;
	}

	Shader* ShaderManager::FindShader(const ShaderConfigurationWrapper& config)
	{
		auto it = mMap.find(config);
		if (it != mMap.end())
		{
			return it->second.get();
		}
		return nullptr;
	}


	FileData* ShaderManager::SaveCache(const std::string& url, FileData fileData)
	{
		mDataCache.emplace(url, std::move(fileData));
		return &mDataCache[url];
	}

	void ShaderManager::ReleaseAll()
	{
		for (auto& it : mMap)
		{
			it.second->Shutdown();
		}
		mMap.clear();
		mDataCache.clear();
	}

	ShaderManager::~ShaderManager()
	{
	}
	
}
