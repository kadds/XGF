#include "../../Include/ResourceManager.hpp"
#include "../../Include/Texture.hpp"
#include "../../Include/Tools.hpp"
namespace XGF
{
	TextureResource * TextureResourceManager::GetResource(const wchar_t * name)
	{
		auto it = mResourceMap.find(name);
		if (it == mResourceMap.end())
		{
			XGF_ReportWarn("TextureResource can't find", name);
			return nullptr;
		}
		return &it->second;
	}

	TextureResource * TextureResourceManager::GetResourceByAlias(const wchar_t * alias)
	{
		auto it = mNameToAliasMap.find(alias);
		if (it == mNameToAliasMap.end())
		{
			return nullptr;
		}
		return GetResource(it->second.c_str());
	}

	void TextureResourceManager::LoadResourceAsync(GDI * gdi, std::vector<ResourceInfo> & infoArray, Asyn * gameThread,std::function<void(std::vector<ResourceInfo>, int success)> finishFunction)
	{
		
		AsyncTask::NewTask(gameThread, [this, gdi, infoArray](AsyncTask * that) 
		{
			if (mIsLoadingResource)
				while (mIsLoadingResource);
			mIsLoadingResource = true;
			wchar_t buffer[MAX_PATH];
			int okCount = 0;
			for (auto it : infoArray)
			{
				TextureResource textureResource;
				if (it.inMemory)
				{
					if (textureResource.Load(gdi, it.mem, it.memSize))
						okCount++;
				}
				else
				{
					Tools::GetPathBy(it.name.c_str(), buffer, MAX_PATH);
					if (textureResource.Load(gdi, buffer))
						okCount++;
				}
				if (textureResource.GetTexture() != nullptr)
				{
					this->InsertResource(it.name, it.alias, textureResource);
				}
			}
			mIsLoadingResource = false;
			that->Finish(okCount, 0);
		})->SetFinshListener([finishFunction, infoArray, this](int code, std::any data)
		{
			finishFunction(infoArray, code);
		});
	}

	void TextureResourceManager::LoadResource(GDI * gdi, std::vector<ResourceInfo> & infoArray)
	{
		if (mIsLoadingResource)
			while (mIsLoadingResource) ;
		int okCount = 0;
		wchar_t buffer[MAX_PATH];
		mIsLoadingResource = true;
		for (auto it : infoArray)
		{
			TextureResource textureResource;
			if (it.inMemory)
			{
				if (textureResource.Load(gdi, it.mem, it.memSize))
					okCount++;
			}
			else
			{
				Tools::GetPathBy(it.name.c_str(), buffer, MAX_PATH);
				if (textureResource.Load(gdi, buffer))
					okCount++;
			}
			if (textureResource.GetTexture() != nullptr)
			{
				this->InsertResource(it.name, it.alias, textureResource);
			}
		}
		mIsLoadingResource = false;
	}


	void TextureResourceManager::ReleaseAllResource()
	{
	}

	void TextureResourceManager::InsertResource(std::wstring name, std::wstring alias, TextureResource & tRes)
	{
		std::lock_guard<std::mutex> mtx(mutex);

		this->mResourceMap.insert(std::make_pair(name, tRes));
		if ( alias != L"" && !alias.empty())
		{
			this->mNameToAliasMap.insert(std::make_pair(alias, name));
		}
	}
}