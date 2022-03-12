#include "Engine/Graphics/Device.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/Logger.h"
#include "Engine/Utility/Safety.h"

template<>
void rv::destroy_vk(VkDevice device, VkDevice, VkInstance)
{
	vkDestroyDevice(device, nullptr);
}

bool rv::DeviceQueueFinder::Fits(const VkQueueFamilyProperties& properties, u32 index) const
{
	return false;
}

rv::DeviceQueueFlagFinder::DeviceQueueFlagFinder(Flags<VkQueueFlagBits> flags)
	:
	flags(flags)
{
}

bool rv::DeviceQueueFlagFinder::Fits(const VkQueueFamilyProperties& properties, u32 index) const
{
	return properties.queueFlags & flags.data();
}

rv::DeviceValidationLayers::DeviceValidationLayers()
	:
	layers(DefaultLayers())
{
}

std::vector<const char*> rv::DeviceValidationLayers::DefaultLayers()
{
	return InstanceValidationLayers::DefaultLayers();
}

bool rv::DeviceValidationLayers::Supported(const PhysicalDevice& physical) const
{
	if (layers.empty())
		return true;

	u32 count = 0;
	Vkr result = vkEnumerateDeviceLayerProperties(physical.device, &count, nullptr);
	if (result.failed())
		return false;

	std::vector<VkLayerProperties> available(count);
	result = vkEnumerateDeviceLayerProperties(physical.device, &count, available.data());
	if (result.failed())
		return false;

	for (const char* layer : layers)
	{
		bool supported = false;
		for (const auto& properties : available)
		{
			if (detail::byte_string_equal(layer, properties.layerName))
			{
				supported = true;
				break;
			}
		}
		if (!supported)
			return false;
	}
	return true;
}

rv::DeviceExtensions::DeviceExtensions()
	:
	extensions(DefaultExtensions())
{
}

std::vector<const char*> rv::DeviceExtensions::DefaultExtensions()
{
	return { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
}

bool rv::DeviceExtensions::Supported(const PhysicalDevice& physical) const
{
	if (extensions.empty())
		return true;

	u32 count = 0;
	Vkr result = vkEnumerateDeviceExtensionProperties(physical.device, nullptr, &count, nullptr);
	if (result.failed())
		return false;

	std::vector<VkExtensionProperties> available(count);
	result = vkEnumerateDeviceExtensionProperties(physical.device, nullptr, &count, available.data());
	if (result.failed())
		return false;

	for (const char* extension : extensions)
	{
		bool supported = false;
		for (const auto& e : available)
		{
			if (detail::byte_string_equal(extension, e.extensionName))
			{
				supported = true;
				break;
			}
		}
		if (!supported)
			return false;
	}
	return true;
}

rv::DeviceRater::DeviceRater()
	:
	requiredFeatures(),
	optionalFeatures(),
	propertiesRater(DefaultPropertiesRater)
{
}

u64 rv::DeviceRater::Rate(const PhysicalDevice& device) const
{
	u64 rating = 0;

	if (!extensions.Supported(device) || !validation.Supported(device))
		return 0;

	for (const auto& finder : queueFinders)
	{
		bool found = false;
		for (u32 i = 0; i < safe_cast<u32>(device.queueFamilies.size()); ++i)
			if (finder->Fits(device.queueFamilies[i], i))
			{
				found = true;
				break;
			}
		if (!found)
			return 0;
	}

	const VkBool32* deviceFeatures = reinterpret_cast<const VkBool32*>(&device.features);
	const VkBool32* required = reinterpret_cast<const VkBool32*>(&requiredFeatures);
	for (size_t i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); ++i)
		if (required[i] && !deviceFeatures[i])
			return 0;
	const VkBool32* optional = reinterpret_cast<const VkBool32*>(&optionalFeatures);
	for (size_t i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); ++i)
		rating += static_cast<typename rv::of_size<sizeof(VkBool32)>::type>(optional[i]);

	u64 propertiesRating = propertiesRater ? propertiesRater(device.properties) : 0;
	return propertiesRating ? rating + propertiesRating : 0;
}

rv::Result rv::PhysicalDevice::Create(std::vector<PhysicalDevice>& devices, const Instance& instance)
{
	rv_result;
	u32 size;
	rif_check_vkr_msg(vkEnumeratePhysicalDevices(instance.instance, &size, nullptr), strvalid(u"Unable to enumerate physical devices"));
	std::vector<VkPhysicalDevice> pdevices(size);
	devices.resize(size);
	rif_check_vkr_msg(vkEnumeratePhysicalDevices(instance.instance, &size, pdevices.data()), strvalid(u"Unable to enumerate physical devices"));
	for (u32 i = 0; i < size; ++i)
	{
		devices[i].device = pdevices[i];
		vkGetPhysicalDeviceProperties(devices[i].device, &devices[i].properties);
		vkGetPhysicalDeviceFeatures(devices[i].device, &devices[i].features);
		u32 familySize;
		vkGetPhysicalDeviceQueueFamilyProperties(devices[i].device, &familySize, nullptr);
		devices[i].queueFamilies.resize(familySize);
		vkGetPhysicalDeviceQueueFamilyProperties(devices[i].device, &familySize, devices[i].queueFamilies.data());
	}
	return result;
}

static constexpr rv::valid_utf16_cstring __type(VkPhysicalDeviceType type)
{
	switch (type)
	{
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			return u"INTEGRATED";
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			return u"DISCRETE";
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			return u"VIRTUAL GPU";
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			return u"CPU";
		case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
			return u"MAX";
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
		default:
			return u"OTHER";
	}
}

rv::u64 rv::DefaultPropertiesRater(const VkPhysicalDeviceProperties& properties)
{
	u64 score = 0;
	score += properties.limits.maxImageDimension2D / 1024;
	switch (properties.deviceType)
	{
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			score += 500;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			score += 5000;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			score += 50;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			score += 5;
			break;
	}
	return score;
}

rv::Device::Device(Device&& rhs) noexcept
	:
	device(move_ptr(rhs.device)),
	physical(std::move(rhs.physical)),
	graphics(rhs.graphics),
	compute(rhs.compute),
	queueCounts(std::move(rhs.queueCounts)),
	instance(move_ptr(rhs.instance))
{
}

rv::Device::~Device()
{
	Release();
}

rv::Device& rv::Device::operator=(Device&& rhs) noexcept
{
	Release();
	device = move_ptr(rhs.device);
	physical = std::move(rhs.physical);
	graphics = rhs.graphics;
	compute = rhs.compute;
	queueCounts = std::move(rhs.queueCounts);
	instance = move_ptr(rhs.instance);
	return *this;
}

rv::Result rv::Device::Create(Device& device, const Instance& instance, const DeviceRater& rater)
{
	rv_result;
	device.instance = &instance;

	std::vector<PhysicalDevice> devices;
	rv_rif(PhysicalDevice::Create(devices, instance));
	PhysicalDevice* best = nullptr;
	u64 bestRating = 0;
	for (auto& physical : devices)
	{
		u64 rating = rater.Rate(physical);
		if (rating > bestRating)
		{
			best = &physical;
			bestRating = rating;
		}
	}
	rif_check_condition_msg(best, strvalid(u"No suitable Vulkan devices found"));
	device.physical = *best;

	device.validation = rater.validation;
	device.extensions = rater.extensions;

	VkPhysicalDeviceFeatures features = {};
	VkBool32* deviceFeatures = reinterpret_cast<VkBool32*>(&features);
	const VkBool32* required = reinterpret_cast<const VkBool32*>(&rater.requiredFeatures);
	const VkBool32* optional = reinterpret_cast<const VkBool32*>(&rater.optionalFeatures);
	for (size_t i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); ++i)
		deviceFeatures[i] = bool(bool(required[i]) || bool(optional[i]));

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.enabledLayerCount = safe_cast<u32>(rater.validation.layers.size());
	createInfo.ppEnabledLayerNames = rater.validation.layers.data();
	createInfo.enabledExtensionCount = safe_cast<u32>(rater.extensions.extensions.size());
	createInfo.ppEnabledExtensionNames = rater.extensions.extensions.data();
	createInfo.pEnabledFeatures = &features;

	std::vector<float> queuePriorities(2 + rater.queueFinders.size(), 1.0f);
	std::vector<VkDeviceQueueCreateInfo> queues;
	device.queueCounts.resize(device.physical.queueFamilies.size());
	for (u32 family = 0; family < safe_cast<u32>(device.physical.queueFamilies.size()); ++family)
	{
		const auto& properties = device.physical.queueFamilies[family];
		u32 fitting = 0;
		if (device.graphics.family == Optional<u32>::invalid_value && (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT))
		{
			device.graphics.family = family;
			++fitting;
		}
		if (device.compute.family == Optional<u32>::invalid_value && (properties.queueFlags & VK_QUEUE_COMPUTE_BIT))
		{
			device.compute.family = family;
			++fitting;
		}
		for (const auto& finders : rater.queueFinders)
			if (finders->Fits(properties, family))
				++fitting;
		if (fitting)
		{
			VkDeviceQueueCreateInfo queueCreateInfo {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.pQueuePriorities = queuePriorities.data();
			queueCreateInfo.queueFamilyIndex = family;
			queueCreateInfo.queueCount = fitting;
			queues.push_back(queueCreateInfo);
		}
	}

	createInfo.queueCreateInfoCount = safe_cast<u32>(queues.size());
	createInfo.pQueueCreateInfos = queues.data();

	result = rv_check_vkr_msg(vkCreateDevice(device.physical.device, &createInfo, nullptr, &device.device), strvalid(u"Unable to create device"));
	if (result.succeeded())
	{
		if (device.graphics.family != Optional<u32>::invalid_value)
			device.graphics = device.GetQueue(device.graphics.family);
		if (device.compute.family != Optional<u32>::invalid_value)
			device.compute = device.GetQueue(device.compute.family);
		rv_logstr(strvalid(u"Created Device \""), device.physical.properties.deviceName, u'\"');
	}
	return result;
}

void rv::Device::Release()
{
	release_vk(device, instance);
	instance = nullptr;
}

rv::DeviceQueue rv::Device::GetQueue(u32 family)
{
	DeviceQueue queue;
	queue.family = family;
	queue.index = (queueCounts[family]++ % physical.queueFamilies[family].queueCount);
	vkGetDeviceQueue(device, family, queue.index, &queue.queue);
	return queue;
}

rv::DeviceQueue rv::Device::GetQueue(u32 family, u32 index) const
{
	DeviceQueue queue;
	queue.family = family;
	queue.index = index;
	vkGetDeviceQueue(device, family, index, &queue.queue);
	return queue;
}
