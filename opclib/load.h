#pragma once

#include "loadworker.h"

namespace otherpeoplecode
{
	class Loader
	{
	public:
		Loader();
		~Loader();
		HttpResponse Load(HttpRequest request);

	private:
		HINTERNET m_session;
	};
}
