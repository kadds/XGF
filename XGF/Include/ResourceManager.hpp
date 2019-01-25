#pragma once
#include "Defines.hpp"
#include "AsyncTask.hpp"
#include <functional>
#include <any>
namespace XGF
{
	class Texture;
	struct ResourceInfo
	{
		std::wstring name;
		std::wstring alias;
		bool inMemory;
		void * mem;
		size_t memSize;
		ResourceInfo(std::wstring name, std::wstring alias = L"") :name(name), alias(alias), inMemory(false)
		{

		}
	};
	template<typename Type>
	class ResourceManagerBase
	{
		void ReleaseAllResource();

	protected:
		std::map<std::wstring, std::wstring> mNameToAliasMap;
		std::unordered_map<std::wstring, Type> mResourceMap;

		std::mutex mutex;
	};
	class TextureResourceManager
	{
	public:
		
		Texture * GetResource(const wchar_t * name);
		Texture * GetResourceByAlias(const wchar_t * alias);
		void LoadResourceAsync(std::vector<ResourceInfo> & infoArray, Asyn * gameThread,std::function<void(std::vector<ResourceInfo>, int success)> finishFunction);
		void LoadResource(std::vector<ResourceInfo> & infoArray);

		void ReleaseAllResource();

		void InsertResource(std::wstring name, std::wstring alias, Texture * texture);
	private:
		
		std::map<std::wstring, std::wstring> mNameToAliasMap;
		std::unordered_map<std::wstring, Texture *> mResourceMap;

		std::mutex mutex;
		
		bool mIsLoadingResource = false;
		//std::map<const wchar_t *, std::>
	};
}