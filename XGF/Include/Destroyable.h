#pragma once

class Destroyable
{
private:
	bool _is_created;
	void Create();
	void Destroy();
	Destroyable(): _is_created(true)
	{
		
	}
	~Destroyable()
	{
		if(_is_created)
		{
			
		}
		_is_created = false;
	}
};