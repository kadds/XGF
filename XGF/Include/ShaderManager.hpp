#pragma once
#include "Defines.hpp"
#include "Shader.hpp"
#include "ResourceManager.hpp"
#include <functional>
#include "Logger.hpp"
#include "Shadow.hpp"

namespace XGF
{
	class FileData
	{
	public:
		FileData(size_t size, std::unique_ptr<char[]> ptr);

		FileData();

		size_t GetSize() const;

		void* GetPtr() const;

		FileData(FileData&& fileData) noexcept;
		FileData(const FileData & fileData) = delete;
		FileData & operator= (const FileData & fileData) = delete;
	private:
		std::unique_ptr<char[]> data;
		size_t size;
	};
	struct CompileDefine
	{
	public:
		std::string name;
		std::string definition;
		CompileDefine(std::string && n, std::string && d = "") : name(std::move(n)), definition(std::move(d)) {  }
		bool operator ==(const CompileDefine& def) const;

		size_t hash() const;

		struct CompileDefinePair
		{
			CompileDefine * def;
			CompileDefinePair(CompileDefine * ptr) : def(ptr) {	 }
			CompileDefinePair & operator=(std::string&& str);
		};
	};
	class CompileDefines
	{
	public:
		
		std::unique_ptr<char*> GetDataPtr() const;

		CompileDefine::CompileDefinePair operator[](std::string&& key);

		void Remove(const std::string& key);

		const std::vector<CompileDefine>& GetDefines() const;
		CompileDefines(const CompileDefines & defs) = default;
		CompileDefines & operator = (const CompileDefines & defs) = default;
		CompileDefines() = default;
		bool operator ==(const CompileDefines& defs) const;
		bool operator !=(const CompileDefines& defs) const;

		size_t hash() const;
	private:
		std::vector<CompileDefine> defines;
	};

	class MemoryLoader
	{
	public:
		virtual std::vector<char> Load(std::string url);
		virtual std::vector<char> LoadAsync(std::string url);
	};
	class DiskLoader
	{
	public:
		virtual FileData Load(std::string url);
	};
	class NetworkLoader
	{
	public:
		virtual std::vector<char> Load(std::string url);
		virtual std::vector<char> LoadAsync(std::string url);
	};
	class ShaderConfiguration
	{
	public:
		ShaderConfiguration() = default;
		~ShaderConfiguration() = default;
        static const int salt;
		CompileDefines defines;
		std::string sourceName;
		// 5_0, 4_1, 4_0,
		std::string version;
		bool operator ==(const ShaderConfiguration& sc) const;

		bool operator !=(const ShaderConfiguration& sc) const;
		size_t hash() const;
	};
	class ShaderConfigurationWrapper
	{
	public:
		ShaderConfiguration configuration;
		size_t typeIdentifier;
		std::string url;
		bool operator==(const ShaderConfigurationWrapper& c) const;

		bool operator !=(const ShaderConfigurationWrapper& c) const;
		ShaderConfigurationWrapper(const std::string & url, const ShaderConfiguration& config, size_t typeId);
	};
	struct TypeConfigurationPackerHash
	{
		typedef ShaderConfigurationWrapper argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const
		{
			const std::hash<size_t> intHash;
			const std::hash<std::string> strHash;
			result_type hash = strHash(s.url) ^ (s.configuration.hash() ^ (intHash(s.typeIdentifier) << 1) << 1);
			return hash;
		}
	};
	class ShaderCompiler
	{
	public:
		template<typename TShader>
		std::unique_ptr<TShader> Compile(const FileData & fileData, const ShaderConfiguration & config)
		{
			ID3DBlob* pErrorBlob = nullptr;
			ID3DBlob* pBlob = nullptr;
			std::string versionName = TShader::GetPrefixName() + config.version;
			auto ptr = config.defines.GetDataPtr();
			int flag = 0;
#ifdef XGF_DEBUG_SHADER
			flag |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif;
			HRESULT hr = D3DCompile(fileData.GetPtr(), fileData.GetSize(), config.sourceName.c_str(),
				(const D3D_SHADER_MACRO*)(ptr.get()), NULL, TShader::GetEntrypoint().c_str(), versionName.c_str(), flag, 0, &pBlob, &pErrorBlob);
			if (FAILED(hr))
				if (pErrorBlob == nullptr)
					XGF_Error(IO, "Shader file not find ", config.sourceName);
				else
					XGF_Error_Check(Shader, hr, std::string((const char *)pErrorBlob->GetBufferPointer()));
			std::unique_ptr<TShader> shader = std::make_unique<TShader>();
			shader->Initialize((unsigned char *)pBlob->GetBufferPointer(), (int)pBlob->GetBufferSize());
			pBlob->Release();
			return std::move(shader);
		};
	};
	class ShaderManager
	{
		template<typename TShader>
		ShaderConfigurationWrapper GetConfigurationWrapper(const std::string & url, const ShaderConfiguration & cfg)
		{
			std::hash<std::string> hash;
			return ShaderConfigurationWrapper(url, cfg, hash(TShader::GetPrefixName()));
		}
	public:
		ShaderManager();
		ShaderManager(const ShaderManager &) = delete;
		ShaderManager & operator = (const ShaderManager &) = delete;
		// Dynamic
		template<typename TShader>
		TShader * LoadShader(const std::string & url, const ShaderConfiguration & config)
		{
			auto cfg = GetConfigurationWrapper<TShader>(url, config);
			auto * shader = static_cast<TShader *>(FindShader(cfg));
			if(!shader)
			{
				auto fileData = FindCacheData(url);
				if(!fileData)
				{
					FileData fd = mLoader.Load(url);
					fileData = SaveCache(url, std::move(fd));
				}
				auto uShader = mCompiler.Compile<TShader>(*fileData, config);
				shader = uShader.get();
				mMap.emplace(cfg, std::move(uShader));
			}
			return shader;
		}

		FileData* FindCacheData(const std::string& url);
		Shader* FindShader(const ShaderConfigurationWrapper& config);

		FileData* SaveCache(const std::string& url, FileData fileData);

	public:
		void ReleaseAll();

		~ShaderManager();
	
	private:
		ShaderCompiler mCompiler;
		DiskLoader mLoader;
		std::unordered_map<std::string, FileData> mDataCache;
		std::unordered_map<ShaderConfigurationWrapper, std::unique_ptr<Shader>, TypeConfigurationPackerHash> mMap;
	};
	
}
