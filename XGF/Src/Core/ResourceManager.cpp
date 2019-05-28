#include "../../Include/ResourceManager.hpp"
#include "../../Include/Texture.hpp"
#include "../../Include/Tools.hpp"
#include "../../Include/Logger.hpp"
#include "../../Include/Context.hpp"

namespace XGF
{
	Texture * TextureResourceManager::GetResource(const wchar_t * name)
	{
		auto it = mResourceMap.find(name);
		if (it == mResourceMap.end())
		{
			XGF_Warn(Application, "TextureResource can't find", Logger::WCharToChar(name));
			return nullptr;
		}
		return it->second;
	}

	Texture * TextureResourceManager::GetResourceByAlias(const wchar_t * alias)
	{
		auto it = mNameToAliasMap.find(alias);
		if (it == mNameToAliasMap.end())
		{
			return nullptr;
		}
		return GetResource(it->second.c_str());
	}

	void TextureResourceManager::LoadResourceAsync(std::vector<ResourceInfo> & infoArray, Asyn * gameThread,std::function<void(std::vector<ResourceInfo>, int success)> finishFunction)
	{
		auto & context = Context::Current();
		AsyncTask::NewTask(gameThread, [this, infoArray, &context](std::shared_ptr<AsyncTask> that)
		{
			Context::JoinContext(context);
			if (mIsLoadingResource)
				while (mIsLoadingResource);
			mIsLoadingResource = true;
			wchar_t buffer[MAX_PATH];
			int okCount = 0;
			for (auto it : infoArray)
			{
				Texture * texture = new Texture();
				if (it.inMemory)
				{
					if (texture->Load( it.mem, it.memSize))
						okCount++;
				}
				else
				{
					Tools::GetPathBy(it.name.c_str(), buffer, MAX_PATH);
					if (texture->Load( buffer))
						okCount++;
				}
				if (texture->GetRawTexture() != nullptr)
				{
					this->InsertResource(it.name, it.alias, texture);
				}
			}
			mIsLoadingResource = false;
			that->Finish(okCount, 0);
			Context::DetachContext();
		})->SetFinshListener([finishFunction, infoArray, this](int code, std::any data)
		{
			finishFunction(infoArray, code);
		});
	}

	void TextureResourceManager::LoadResource(std::vector<ResourceInfo> & infoArray)
	{
		if (mIsLoadingResource)
			while (mIsLoadingResource) ;
		int okCount = 0;
		wchar_t buffer[MAX_PATH];
		mIsLoadingResource = true;
		for (auto it : infoArray)
		{
			Texture * texture = new Texture();
			if (it.inMemory)
			{
				if (texture->Load(it.mem, it.memSize))
					okCount++;
			}
			else
			{
				Tools::GetPathBy(it.name.c_str(), buffer, MAX_PATH);
				if (texture->Load( buffer))
					okCount++;
			}
			if (texture->GetRawTexture() != nullptr)
			{
				this->InsertResource(it.name, it.alias, texture);
			}
			else
			{
				delete texture;
			}
		}
		mIsLoadingResource = false;
	}


	void TextureResourceManager::ReleaseAllResource()
	{
		for (auto it : mResourceMap)
			delete it.second;
	}

	void TextureResourceManager::InsertResource(std::wstring name, std::wstring alias, Texture * texture)
	{
		std::lock_guard<std::mutex> mtx(mutex);

		this->mResourceMap.insert(std::make_pair(name, texture));
		if ( alias != L"" && !alias.empty())
		{
			this->mNameToAliasMap.insert(std::make_pair(alias, name));
		}
	}
}
