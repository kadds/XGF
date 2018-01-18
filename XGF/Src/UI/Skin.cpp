#include "../../Include/Skin.hpp"

namespace XGF
{
	std::shared_ptr<Skin> Skin::CreateFromTextures(Texture * normal, Texture * hover, Texture * active, Texture * disable)
	{
		auto skin = std::make_shared<Skin>();
		skin->mSkinStates[SkinState::normal] = normal;
		skin->mSkinStates[SkinState::hover] = hover;
		skin->mSkinStates[SkinState::active] = active;
		skin->mSkinStates[SkinState::disable] = disable;
		return skin;
	}
	Texture & Skin::GetTexture(SkinState state)
	{
		return * (mSkinStates.find(state)->second);
	}
}