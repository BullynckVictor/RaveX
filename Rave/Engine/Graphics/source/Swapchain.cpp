#include "Engine/Graphics/Swapchain.h"
#include "Engine/Graphics/Window.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/Logger.h"

template<>
void rv::destroy_vk(VkSurfaceKHR surface, VkDevice device, VkInstance instance)
{
	vkDestroySurfaceKHR(instance, surface, nullptr);
}

template<>
void rv::destroy_vk(VkSwapchainKHR swap, VkDevice device, VkInstance instance)
{
	vkDestroySwapchainKHR(device, swap, nullptr);
}

rv::SwapchainPreferences::SwapchainPreferences(bool vsync)
	:
	format({ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }),
	presentMode(vsync ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR),
	imageCount(vsync ? 3 : 2)
{
}

rv::Surface::Surface(Surface&& rhs) noexcept
	:
	surface(move_ptr(rhs.surface)),
	capabilities(rhs.capabilities),
	formats(std::move(rhs.formats)),
	presentModes(std::move(rhs.presentModes)),
	device(move_ptr(rhs.device)),
	window(move_ptr(rhs.window))
{
}

rv::Surface::~Surface()
{
	Release();
}

rv::Surface& rv::Surface::operator=(Surface&& rhs) noexcept
{
	Release();

	surface = move_ptr(rhs.surface);
	capabilities = rhs.capabilities;
	formats = std::move(rhs.formats);
	presentModes = std::move(rhs.presentModes);
	device = move_ptr(rhs.device);
	window = move_ptr(rhs.window);

	return *this;
}

rv::Result rv::Surface::Create(Surface& surface, const Device& device, const Window& window)
{
	rv_result;
	if (&window != surface.window)
	{
		surface.Release();
		surface.window = &window;
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.hwnd = window.hwnd;
		surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
		rif_check_condition_msg(device.instance, strvalid(u"Device not properly initialised"));
		rif_check_vkr_msg(vkCreateWin32SurfaceKHR(device.instance->instance, &surfaceCreateInfo, nullptr, &surface.surface), strvalid(u"Unable to get win32 surface"));
	}
	surface.device = &device;
	rif_check_vkr(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.physical.device, surface.surface, &surface.capabilities));
	u32 nFormats;
	u32 nPresentModes;
	rif_check_vkr(vkGetPhysicalDeviceSurfaceFormatsKHR(device.physical.device, surface.surface, &nFormats, nullptr));
	rif_check_vkr(vkGetPhysicalDeviceSurfacePresentModesKHR(device.physical.device, surface.surface, &nPresentModes, nullptr));
	rif_check_condition(nFormats);
	rif_check_condition(nPresentModes);
	surface.formats.resize(nFormats);
	surface.presentModes.resize(nPresentModes);
	rif_check_vkr(vkGetPhysicalDeviceSurfaceFormatsKHR(device.physical.device, surface.surface, &nFormats, surface.formats.data()));
	rif_check_vkr(vkGetPhysicalDeviceSurfacePresentModesKHR(device.physical.device, surface.surface, &nPresentModes, surface.presentModes.data()));
	return result;
}

void rv::Surface::Release()
{
	if (device)
		release_vk(surface, device->instance);
	device = nullptr;
	window = nullptr;
}

VkSurfaceFormatKHR rv::Surface::GetClosestFormat(const SwapchainPreferences& preferences) const
{
	VkSurfaceFormatKHR finalFormat = formats[0];
	u8 best = 0;
	for (const auto& format : formats) 
	{
		u8 hits = 0;
		if (format.format == preferences.format.format)
			++hits;
		if (format.colorSpace == preferences.format.colorSpace)
			++hits;
		if (hits == 2)
			return format;
		if (hits > best)
		{
			best = hits;
			finalFormat = format;
		}
	}
	return finalFormat;
}

VkPresentModeKHR rv::Surface::GetClosestPresentMode(const SwapchainPreferences& preferences) const
{
	for (auto presentMode : presentModes)
		if (presentMode == preferences.presentMode)
			return presentMode;
	return presentModes[0];
}

VkExtent2D rv::Surface::GetExtent(const Size& windowSize) const
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return capabilities.currentExtent;
	VkExtent2D extent {};
	extent.width = std::clamp(windowSize.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	extent.height = std::clamp(windowSize.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
	return extent;
}

rv::Swapchain::Swapchain(Swapchain&& rhs) noexcept
	:
	chain(move_ptr(rhs.chain)),
	surface(std::move(rhs.surface)),
	present(rhs.present),
	device(move_ptr(rhs.device))
{
	rhs.present = {};
}

rv::Swapchain::~Swapchain()
{
	Release();
}

rv::Swapchain& rv::Swapchain::operator=(Swapchain&& rhs) noexcept
{
	Release();
	chain = move_ptr(rhs.chain);
	surface = std::move(rhs.surface);
	present = rhs.present;
	device = move_ptr(rhs.device);
	rhs.present = {};
	return *this;
}

rv::Result rv::Swapchain::Create(Swapchain& swap, const Device& device, const Window& window, const SwapchainPreferences& preferences)
{
	rv_result;
	swap.device = &device;

	rv_rif(Surface::Create(swap.surface, device, window));

	VkBool32 presentSupport = false;
	rif_check_vkr(vkGetPhysicalDeviceSurfaceSupportKHR(device.physical.device, device.graphics.family, swap.surface.surface, &presentSupport));
	rif_check_condition_msg(presentSupport, "Unable to present with current graphics queue");
	swap.present = device.GetQueue(device.graphics.family, device.graphics.index);
	swap.extent = swap.surface.GetExtent(window.size);
	swap.format = swap.surface.GetClosestFormat(preferences);
	swap.presentMode = swap.surface.GetClosestPresentMode(preferences);

	VkSwapchainCreateInfoKHR createInfo {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = swap.surface.surface;
	createInfo.oldSwapchain = swap.chain;
	createInfo.imageExtent = swap.extent;
	createInfo.imageFormat = swap.format.format;
	createInfo.imageColorSpace = swap.format.colorSpace;
	createInfo.presentMode = swap.presentMode;
	createInfo.minImageCount = swap.surface.capabilities.maxImageCount ? std::clamp(preferences.imageCount, swap.surface.capabilities.minImageCount, swap.surface.capabilities.maxImageCount) : preferences.imageCount;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.preTransform = swap.surface.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.clipped = VK_TRUE;
	rif_check_vkr_msg(vkCreateSwapchainKHR(device.device, &createInfo, nullptr, &swap.chain), strvalid(u"Unable to create swapchain"));

	u32 nImages;
	rif_check_vkr(vkGetSwapchainImagesKHR(device.device, swap.chain, &nImages, nullptr));
	swap.images.resize(nImages);
	rif_check_vkr(vkGetSwapchainImagesKHR(device.device, swap.chain, &nImages, swap.images.data()));

	return result;
}

void rv::Swapchain::Release()
{
	release_vk(chain, device);
	surface.Release();
	device = nullptr;
}
