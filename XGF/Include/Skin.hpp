#pragma once
#include "Defines.hpp"
#include <map>
#include "Texture.hpp"
#include <memory>
namespace XGF
{
	enum class SkinState
	{
		normal,
		hover,
		active,
		disable,
	};
	class Skin
	{
	public:
		//static Skin & CreateFromSkinPack();
		static std::shared_ptr<Skin> CreateFromTextures(Texture * normal, Texture * hover = nullptr, Texture * active = nullptr, Texture * disable = nullptr);

		Texture * GetTexture(SkinState state);
	private:
		std::map<SkinState, Texture> mSkinStates;
	};
}