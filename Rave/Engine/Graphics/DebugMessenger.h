#pragma once
#include "Engine/Graphics/Instance.h"
#include <mutex>
#include <memory>
#include <queue>

#if not defined(RV_DEBUG_MESSENGER) and defined(RV_DEBUG) and not defined (RV_NO_DEBUG_MESSENGER)
#define RV_DEBUG_MESSENGER
#endif

namespace rv
{
	static constexpr Identifier32 vulkan_debug_result = "Vulkan Debug Result";

	struct VulkanDebugMessage
	{
		VulkanDebugMessage() = default;

		Result Result() const;
		utf16_string Describe() const;

		bool Valid() const;
		operator bool() const;

		VkDebugUtilsMessageSeverityFlagBitsEXT severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT;
		std::string message;
	};

	struct DebugMessenger
	{
	public:
		DebugMessenger();
		~DebugMessenger();

		static VkDebugUtilsMessengerCreateInfoEXT CreateInfo(Flags<VkDebugUtilsMessageSeverityFlagBitsEXT> severity = defaultFlags);
		static Result Create(DebugMessenger& debug, const Instance& instance, Flags<VkDebugUtilsMessageSeverityFlagBitsEXT> severity = defaultFlags);
		void Release();
		
		static VkBool32 Callback(
			VkDebugUtilsMessageSeverityFlagBitsEXT severity,
			VkDebugUtilsMessageTypeFlagsEXT types,
			const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
			void* data
		);

		static Severity Convert(VkDebugUtilsMessageSeverityFlagBitsEXT severity);

		VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
		Flags<VkDebugUtilsMessageSeverityFlagBitsEXT> severity;
		const Instance* instance = nullptr;

		Result CheckResult();
		VulkanDebugMessage GetNextMessage();

		static Result CheckStaticResult();
		static VulkanDebugMessage GetNextStaticMessage();

#ifndef RV_DEBUG_MESSENGER
		static constexpr bool enabled = false;
#else
		static constexpr bool enabled = true;

	private:
		std::unique_ptr<std::mutex> mutex;
		std::deque<VulkanDebugMessage> messages;
		static std::deque<VulkanDebugMessage> staticMessages;
		static std::mutex staticMutex;
#endif
	private:
		static constexpr Flags<VkDebugUtilsMessageSeverityFlagBitsEXT> defaultFlags = make_flags<VkDebugUtilsMessageSeverityFlagBitsEXT>(
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
		);
	};
}