#pragma once
#include "Engine/Graphics/Vulkan.h"
#include "Engine/Utility/Result.h"
#include <vector>

#if !defined(RV_DEBUG_VALIDATION_LAYER) && defined(RV_DEBUG)
#define RV_DEBUG_VALIDATION_LAYER
#endif

#if !defined(RV_DEBUG_EXTENSION) && defined(RV_DEBUG)
#define RV_DEBUG_EXTENSION
#endif

namespace rv
{
	struct ApplicationInfo
	{
		ApplicationInfo(u32 apiVersion = VK_API_VERSION_1_0, valid_utf8_cstring name = u8"Rave Application", u32 version = VK_MAKE_API_VERSION(0, 1, 0, 0));

		static constexpr u32 engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
		VkApplicationInfo info;
	};

	struct InstanceValidationLayers
	{
		InstanceValidationLayers();

		static std::vector<const char*> DefaultLayers();
		bool Supported() const;

		std::vector<const char*> layers;
	};

	struct InstanceExtensions
	{
		InstanceExtensions();

		static std::vector<const char*> DefaultExtensions();
		bool Supported() const;

		std::vector<const char*> extensions;
	};

	struct Instance
	{
		Instance() = default;
		~Instance();
		
		static Result Create(
			Instance& instance, 
			const InstanceExtensions& extensions = {}, 
			const InstanceValidationLayers& validation = {}, 
			const ApplicationInfo& app = {}
		);

		template<typename F = PFN_vkVoidFunction>
		F GetProcAddr(const char* name) const { return instance ? reinterpret_cast<F>(vkGetInstanceProcAddr(instance, name)) : nullptr; }

		void Release();

		VkInstance instance = VK_NULL_HANDLE;
		ApplicationInfo app;
		InstanceExtensions extensions;
		InstanceValidationLayers validation;
	};

	template<typename T>
	void release_vk(T*& object, const Instance& instance)
	{
		if (object)
		{
			destroy_vk(object, VK_NULL_HANDLE, instance.instance);
			object = VK_NULL_HANDLE;
		}
	}

	template<typename T>
	void release_vk(T*& object, const Instance* instance)
	{
		if (object)
		{
			if (instance)
				destroy_vk(object, VK_NULL_HANDLE, instance->instance);
			object = VK_NULL_HANDLE;
		}
	}
}