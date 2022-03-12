#pragma once
#include "Engine/Utility/Result.h"
#include "Engine/Graphics/Device.h"
#include <utility>

namespace rv
{
	template<typename R> concept Renderer = requires(R renderer, Device device, typename R::Descriptor descriptor, Result result) { 
		result = renderer.Render(); 
		result = R::Create(renderer, device, std::move(descriptor));
	};

	typedef Result(*RenderFunction)(void*);
	typedef Result(*CreateFunction)(void*, const Device*, void*);

	namespace detail
	{
		template<Renderer R>
		Result make_render_function(void* renderer) { return reinterpret_cast<R*>(renderer)->Render(); }
		template<Renderer R>
		Result make_create_function(void* renderer, const Device* device, void* descriptor) { return R::Create(*reinterpret_cast<R*>(renderer), *device, std::move(*reinterpret_cast<typename R::Descriptor*>(descriptor))); }
	}
}