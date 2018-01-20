#include "../../Include/Skin.hpp"

namespace XGF
{
	std::shared_ptr<Skin> Skin::CreateFromTextures(Texture * normal, Texture * hover, Texture * active, Texture * disable)
	{
		auto skin = std::make_shared<Skin>();
		if (normal != nullptr)
			skin->mSkinStates[SkinState::normal] = *normal;
		if(hover != nullptr)
			skin->mSkinStates[SkinState::hover] = *hover;
		if(active != nullptr)
			skin->mSkinStates[SkinState::active] = *active;
		if(disable != nullptr)
			skin->mSkinStates[SkinState::disable] = *disable;
		return skin;
	}
	Texture * Skin::GetTexture(SkinState state)
	{
		auto it = mSkinStates.find(state);
		if (it != mSkinStates.end())
			return &(it->second);
		else
			return nullptr;//TODOL:: NULLTexure
	}
}