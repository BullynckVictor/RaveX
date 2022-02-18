#pragma once
#include "Engine/Core/Windows.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include VULKAN_INCLUDE

namespace rv
{
	struct Instance;
	struct Device;

	template<typename T>
	T* move_ptr(T*& ptr)
	{
		T* temp = ptr;
		ptr = nullptr;
		return temp;
	}

	template<typename T>
	void destroy_vk(T* object, VkDevice device, VkInstance instance)
	{

	}

	template<typename T>
	void release_vk(T*& object)
	{
		if (object)
		{
			destroy_vk(object, VK_NULL_HANDLE, VK_NULL_HANDLE);
			object = VK_NULL_HANDLE;
		}
	}
}