#pragma once
#include "Engine/Graphics/Instance.h"
#include "Engine/Utility/Optional.h"
#include "Engine/Utility/Flags.h"
#include <vector>

namespace rv
{
	struct PhysicalDevice;

	struct DeviceQueueFinder
	{
		DeviceQueueFinder() = default;
		virtual ~DeviceQueueFinder() = default;

		virtual bool Fits(const VkQueueFamilyProperties& properties, u32 index) const;
	};

	struct DeviceQueueFlagFinder : public DeviceQueueFinder
	{
		DeviceQueueFlagFinder(Flags<VkQueueFlagBits> flags = {});

		bool Fits(const VkQueueFamilyProperties& properties, u32 index) const override;

		Flags<VkQueueFlagBits> flags;
	};

	struct DeviceValidationLayers
	{
		DeviceValidationLayers();

		static std::vector<const char*> DefaultLayers();
		bool Supported(const PhysicalDevice& physical) const;

		std::vector<const char*> layers;
	};

	struct DeviceExtensions
	{
		DeviceExtensions();

		static std::vector<const char*> DefaultExtensions();
		bool Supported(const PhysicalDevice& physical) const;

		std::vector<const char*> extensions;
	};

	typedef u64(*DevicePropertiesRater)(const VkPhysicalDeviceProperties&);
	u64 DefaultPropertiesRater(const VkPhysicalDeviceProperties& limits);

	struct DeviceRater
	{
		DeviceRater();

		u64 Rate(const PhysicalDevice& device) const;

		VkPhysicalDeviceFeatures requiredFeatures;
		VkPhysicalDeviceFeatures optionalFeatures;
		DevicePropertiesRater propertiesRater;
		std::vector<std::unique_ptr<DeviceQueueFinder>> queueFinders;
		DeviceValidationLayers validation;
		DeviceExtensions extensions;
	};

	struct PhysicalDevice
	{
		PhysicalDevice() = default;

		static Result Create(std::vector<PhysicalDevice>& devices, const Instance& instance);

		VkPhysicalDevice device = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties properties = {};
		VkPhysicalDeviceFeatures features = {};
		std::vector<VkQueueFamilyProperties> queueFamilies;
	};

	struct DeviceQueue
	{
		DeviceQueue() = default;

		VkQueue queue = VK_NULL_HANDLE;
		u32 family = Optional<u32>::invalid_value;
		u32 index = Optional<u32>::invalid_value;
	};

	struct Device
	{
		Device() = default;
		Device(const Device&) = delete;
		Device(Device&& rhs) noexcept;
		~Device();

		Device& operator= (const Device&) = delete;
		Device& operator= (Device&& rhs) noexcept;

		static Result Create(Device& device, const Instance& instance, const DeviceRater& rater = {});

		void Release();

		DeviceQueue GetQueue(u32 family);
		DeviceQueue GetQueue(u32 family, u32 index) const;

		VkDevice device = VK_NULL_HANDLE;
		PhysicalDevice physical;
		DeviceQueue graphics;
		DeviceQueue compute;
		std::vector<u32> queueCounts;
		DeviceValidationLayers validation;
		DeviceExtensions extensions;
		const Instance* instance = nullptr;
	};

	template<typename T>
	void release_vk(T*& object, const Device& device)
	{
		if (object)
		{
			destroy_vk(object, device.device, VK_NULL_HANDLE);
			object = VK_NULL_HANDLE;
		}
	}

	template<typename T>
	void release_vk(T*& object, const Device& device, const Instance& instance)
	{
		if (object)
		{
			destroy_vk(object, device.device, instance.instance);
			object = VK_NULL_HANDLE;
		}
	}
	template<typename T>
	void release_vk(T*& object, const Device& device, const Instance* instance)
	{
		if (object)
		{
			if (instance)
				destroy_vk(object, device.device, instance->instance);
			object = VK_NULL_HANDLE;
		}
	}
	template<typename T>
	void release_vk(T*& object, const Device* device)
	{
		if (object)
		{
			if (device)
				destroy_vk(object, device->device, VK_NULL_HANDLE);
			object = VK_NULL_HANDLE;
		}
	}

	template<typename T>
	void release_vk(T*& object, const Device* device, const Instance& instance)
	{
		if (object)
		{
			if (device)
				destroy_vk(object, device->device, instance.instance);
			object = VK_NULL_HANDLE;
		}
	}
	template<typename T>
	void release_vk(T*& object, const Device* device, const Instance* instance)
	{
		if (object)
		{
			if (device && instance)
				destroy_vk(object, device->device, instance->instance);
			object = VK_NULL_HANDLE;
		}
	}
}