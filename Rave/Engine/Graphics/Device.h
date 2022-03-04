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
		~Device();

		static Result Create(
			Device& device, 
			const Instance& instance, 
			const DeviceRater& rater = {}, 
			const DeviceValidationLayers& validation = {},
			const DeviceExtensions& extensions = {}
		);

		void Release();

		DeviceQueue GetQueue(u32 family, u32 index = 0);

		VkDevice device = VK_NULL_HANDLE;
		PhysicalDevice physical;
		DeviceQueue graphics;
		DeviceQueue compute;
		const Instance* instance = nullptr;
	};
}