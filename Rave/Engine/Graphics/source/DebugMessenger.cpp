#include "Engine/Graphics/DebugMessenger.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/Logger.h"

#ifdef RV_DEBUG_LOGGER
static constexpr bool debugLogger = true;
std::deque<rv::VulkanDebugMessage> rv::DebugMessenger::staticMessages;
std::mutex rv::DebugMessenger::staticMutex;
#else
static constexpr bool debugLogger = false;
#include <iostream>
#endif

template<>
void rv::destroy_vk(VkDebugUtilsMessengerEXT messenger, VkDevice, VkInstance instance)
{
	auto vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
	if (vkDestroyDebugUtilsMessengerEXT)
		vkDestroyDebugUtilsMessengerEXT(instance, messenger, nullptr);
}

rv::DebugMessenger::DebugMessenger()
#ifdef RV_DEBUG_MESSENGER
	:
	mutex(new std::mutex())
#endif
{
}

rv::DebugMessenger::~DebugMessenger()
{
	Release();
}

VkDebugUtilsMessengerCreateInfoEXT rv::DebugMessenger::CreateInfo(Flags<VkDebugUtilsMessageSeverityFlagBitsEXT> severity)
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = severity.data();
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = Callback;
	return createInfo;
}

rv::Result rv::DebugMessenger::Create(DebugMessenger& debug, const Instance& instance, Flags<VkDebugUtilsMessageSeverityFlagBitsEXT> severity)
{
	if constexpr (enabled)
	{
		rv_result;

		debug.severity = severity;
		debug.instance = &instance;

		VkDebugUtilsMessengerCreateInfoEXT createInfo = CreateInfo(severity);
		createInfo.pUserData = &debug;

		auto vkCreateDebugUtilsMessengerEXT = instance.GetProcAddr<PFN_vkCreateDebugUtilsMessengerEXT>("vkCreateDebugUtilsMessengerEXT");
		rif_check_condition_msg(vkCreateDebugUtilsMessengerEXT, strvalid(u"Unable to get vkCreateDebugUtilsMessengerEXT function from instance"));
		return rv_check_vkr_msg(vkCreateDebugUtilsMessengerEXT(instance.instance, &createInfo, nullptr, &debug.messenger), strvalid(u"Unable to create Vulkan debug messenger"));
	}
	else
		return success;
}

void rv::DebugMessenger::Release()
{
	if constexpr (enabled)
		release_vk(messenger, instance);
}

VkBool32 rv::DebugMessenger::Callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT types, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* data)
{
#ifdef RV_DEBUG_MESSENGER
	VulkanDebugMessage message;
	message.severity = severity;
	message.message = callbackData ? callbackData->pMessage : "<!> Callback data empty <!>";
	utf16_string utf16_message;

	if constexpr (resultHandler.enabled || debugLogger)
		utf16_message = message.message;

	if (data)
	{
		DebugMessenger& messenger = *reinterpret_cast<DebugMessenger*>(data);
		std::lock_guard guard(*messenger.mutex);
		messenger.messages.push_back(message);
	}
	else
	{
		std::lock_guard guard(staticMutex);
		staticMessages.push_back(message);
	}

#ifdef RV_DEBUG_LOGGER
	debug.Log(utf16_message, Convert(severity));
#else
	std::cout << message.message << '\n';
#endif

	if constexpr (resultHandler.enabled)
	{
		Result result = message.Result();
		resultHandler.PushResult(result, {}, std::move(message));
	}

#endif
	return VK_FALSE;
}

rv::Severity rv::DebugMessenger::Convert(VkDebugUtilsMessageSeverityFlagBitsEXT severity)
{
	switch (severity)
	{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			return RV_SEVERITY_INFO;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			return RV_SEVERITY_WARNING;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		default:
			return RV_SEVERITY_ERROR;
	}
}

rv::Result rv::DebugMessenger::CheckResult()
{
#ifdef RV_DEBUG_LOGGER
	if (!messages.empty())
	{
		Result worst;
		std::lock_guard guard(*mutex);
		for (const auto& message : messages)
		{
			Result result = message.Result();
			if (result.severity() > worst.severity())
				worst = result;
		}
		return worst;
	}
	if (staticMessages.empty())
		return {};
	else
	{
		Result worst;
		std::lock_guard guard(staticMutex);
		for (const auto& message : staticMessages)
		{
			Result result = message.Result();
			if (result.severity() > worst.severity())
				worst = result;
		}
		return worst;
	}
#else
	return success;
#endif
}

rv::VulkanDebugMessage rv::DebugMessenger::GetNextMessage()
{
#ifdef RV_DEBUG_LOGGER
	if (!messages.empty())
	{
		std::lock_guard guard(*mutex);
		VulkanDebugMessage message = std::move(messages.front());
		messages.pop_front();
		return message;
	}
	if (staticMessages.empty())
		return {};
	else
	{
		std::lock_guard guard(staticMutex);
		VulkanDebugMessage message = std::move(staticMessages.front());
		staticMessages.pop_front();
		return message;
	}
#else
	return {};
#endif
}

rv::Result rv::DebugMessenger::CheckStaticResult()
{
#ifdef RV_DEBUG_LOGGER
	if (staticMessages.empty())
		return {};
	else
	{
		Result worst;
		std::lock_guard guard(staticMutex);
		for (const auto& message : staticMessages)
		{
			Result result = message.Result();
			if (result.severity() > worst.severity())
				worst = result;
		}
		return worst;
	}
#else
	return success;
#endif
}

rv::VulkanDebugMessage rv::DebugMessenger::GetNextStaticMessage()
{
#ifdef RV_DEBUG_LOGGER
	if (staticMessages.empty())
		return {};
	else
	{
		std::lock_guard guard(staticMutex);
		VulkanDebugMessage message = std::move(staticMessages.front());
		staticMessages.pop_front();
		return message;
	}
#else
	return {};
#endif
}

rv::Result rv::VulkanDebugMessage::Result() const
{
	return rv::Result(DebugMessenger::Convert(severity), vulkan_debug_result);
}

rv::utf16_string rv::VulkanDebugMessage::Describe() const
{
	return message;
}

bool rv::VulkanDebugMessage::Valid() const
{
	return !message.empty();
}

rv::VulkanDebugMessage::operator bool() const
{
	return Valid();
}
