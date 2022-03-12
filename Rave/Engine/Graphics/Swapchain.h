#pragma once
#include "Engine/Graphics/Device.h"
#include "Engine/Utility/Vector.h"

namespace rv
{
	class Window;

	struct SwapchainPreferences
	{
		SwapchainPreferences(bool vsync = true);

		VkSurfaceFormatKHR format;
		VkPresentModeKHR presentMode;
		u32 imageCount;
	};

	struct Surface
	{
		Surface() = default;
		Surface(const Surface&) = delete;
		Surface(Surface&& rhs) noexcept;
		~Surface();

		Surface& operator= (const Surface&) = delete;
		Surface& operator= (Surface&& rhs) noexcept;

		static Result Create(Surface& surface, const Device& device, const Window& window);

		void Release();

		VkSurfaceFormatKHR GetClosestFormat(const SwapchainPreferences& preferences) const;
		VkPresentModeKHR GetClosestPresentMode(const SwapchainPreferences& preferences) const;
		VkExtent2D GetExtent(const Size& windowSize) const;

		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkSurfaceCapabilitiesKHR capabilities {};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
		const Window* window = nullptr;
		const Device* device = nullptr;
	};

	struct Swapchain
	{
		Swapchain() = default;
		Swapchain(const Swapchain&) = delete;
		Swapchain(Swapchain && rhs) noexcept;
		~Swapchain();

		Swapchain& operator= (const Swapchain&) = delete;
		Swapchain& operator= (Swapchain && rhs) noexcept;

		static Result Create(Swapchain& swap, const Device& device, const Window& window, const SwapchainPreferences& preferences = {});

		void Release();

		VkSwapchainKHR chain = VK_NULL_HANDLE;
		Surface surface;
		VkExtent2D extent{};
		VkSurfaceFormatKHR format {};
		VkPresentModeKHR presentMode {};
		std::vector<VkImage> images;
		std::vector<VkImageView> views;
		DeviceQueue present;
		const Device* device = nullptr;
	};
}