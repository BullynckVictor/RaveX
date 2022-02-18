#include "Engine/Graphics/Instance.h"
#include "Engine/Graphics/DebugMessenger.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/Logger.h"
#include "Engine/Utility/Safety.h"

template<>
void rv::destroy_vk(VkInstance instance, VkDevice, VkInstance)
{
	vkDestroyInstance(instance, nullptr);
}

rv::ApplicationInfo::ApplicationInfo(u32 apiVersion, valid_utf8_cstring name, u32 version)
	:
	info()
{
	info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	info.pEngineName = "Rave Engine";
	info.engineVersion = engineVersion;
	info.apiVersion = apiVersion;
	info.pApplicationName = name.c_str<char>();
	info.applicationVersion = version;
}

rv::InstanceValidationLayers::InstanceValidationLayers()
	:
	layers(DefaultLayers())
{
}

std::vector<const char*> rv::InstanceValidationLayers::DefaultLayers()
{
#ifdef RV_DEBUG_VALIDATION_LAYER
	return { "VK_LAYER_KHRONOS_validation" };
#else
	return {  };
#endif
}

bool rv::InstanceValidationLayers::Supported() const
{
	u32 count = 0;
	Vkr result = vkEnumerateInstanceLayerProperties(&count, nullptr);
	if (result.failed())
		return false;

	std::vector<VkLayerProperties> available(count);
	result = vkEnumerateInstanceLayerProperties(&count, available.data());
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

rv::InstanceExtensions::InstanceExtensions()
	:
	extensions(DefaultExtensions())
{
}

std::vector<const char*> rv::InstanceExtensions::DefaultExtensions()
{
#ifdef RV_DEBUG_EXTENSION
	return { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
#else
	return { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
#endif
}

bool rv::InstanceExtensions::Supported() const
{
	return true;
}

rv::Instance::~Instance()
{
	Release();
}

rv::Result rv::Instance::Create(Instance& instance, const InstanceExtensions& extensions, const InstanceValidationLayers& validation, const ApplicationInfo& app)
{
	rv_result;

	rif_check_condition_msg(validation.Supported(), strvalid(u"Not all validation layers supported"));
	rif_check_condition_msg(extensions.Supported(), strvalid(u"Not all extensions supported"));

	instance.app = app;
	instance.extensions = extensions;
	instance.validation = validation;

	VkInstanceCreateInfo createInfo {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &instance.app.info;
	createInfo.enabledLayerCount = safe_cast<u32>(validation.layers.size());
	createInfo.ppEnabledLayerNames = validation.layers.data();
	createInfo.enabledExtensionCount = safe_cast<u32>(extensions.extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.extensions.data();
#ifdef RV_DEBUG_MESSENGER
	auto debugInfo = DebugMessenger::CreateInfo();
	createInfo.pNext = &debugInfo;
#endif
	rv_log(str16(strvalid(u8"Created instance \""), app.info.pEngineName, u'\"'));

	return rv_check_vkr_msg(vkCreateInstance(&createInfo, nullptr, &instance.instance), strvalid(u8"Unable to create instance"));
}

void rv::Instance::Release()
{
	release_vk(instance);
}
