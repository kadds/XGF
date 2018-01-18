#pragma once
#include "Defines.hpp"
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace XGF
{
	class ResourceManager
	{
	public:
		void GetResource(wchar_t * name);
		void LoadResource(wchar_t * name);
		void ReleaseAllResource();
	private:
		//std::map<const wchar_t *, std::>
	};
}