#pragma once
#include "Engine/Utility/Result.h"
#include <utility>

namespace rv
{
	template<typename R> concept Renderer = requires(R renderer, typename R::Descriptor descriptor, Result result) { 
		result = renderer.Render(); 
		result = R::Create(renderer, std::move(descriptor)); 
	};

	typedef Result(*RenderFunction)(void*);
	typedef Result(*CreateFunction)(void*, void*);

	namespace detail
	{
		template<Renderer R>
		Result make_render_function(void* renderer) { return reinterpret_cast<R*>(renderer)->Render(); }
		template<Renderer R>
		Result make_create_function(void* renderer, void* descriptor) { return R::Create(*reinterpret_cast<R*>(renderer), std::move(*reinterpret_cast<typename R::Descriptor*>(descriptor))); }
	}
}