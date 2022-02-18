#pragma once
#include "Engine/Graphics/Vulkan.h"
#include "Engine/Utility/Result.h"

namespace rv
{
	struct Vkr
	{
		constexpr Vkr() = default;
		constexpr Vkr(VkResult result) : result(result) {}

		constexpr bool succeeded_strict() const { return result != VK_SUCCESS; }
		constexpr bool failed_strict() const { return result != VK_SUCCESS; }

		constexpr Severity severity() const 
		{
			switch (result)
			{
				case VK_SUCCESS:
				case VK_OPERATION_DEFERRED_KHR:
				case VK_OPERATION_NOT_DEFERRED_KHR:
				case VK_EVENT_SET:
				case VK_EVENT_RESET:
				return RV_SEVERITY_INFO;

				case VK_NOT_READY:
				case VK_TIMEOUT:
				case VK_INCOMPLETE:
				case VK_SUBOPTIMAL_KHR:
				case VK_THREAD_IDLE_KHR:
				case VK_THREAD_DONE_KHR:
				case VK_PIPELINE_COMPILE_REQUIRED:
				return RV_SEVERITY_WARNING;

				case VK_ERROR_OUT_OF_HOST_MEMORY:
				case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				case VK_ERROR_INITIALIZATION_FAILED:
				case VK_ERROR_DEVICE_LOST:
				case VK_ERROR_MEMORY_MAP_FAILED:
				case VK_ERROR_LAYER_NOT_PRESENT:
				case VK_ERROR_EXTENSION_NOT_PRESENT:
				case VK_ERROR_FEATURE_NOT_PRESENT:
				case VK_ERROR_INCOMPATIBLE_DRIVER:
				case VK_ERROR_TOO_MANY_OBJECTS:
				case VK_ERROR_FORMAT_NOT_SUPPORTED:
				case VK_ERROR_FRAGMENTED_POOL:
				case VK_ERROR_SURFACE_LOST_KHR:
				case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
				case VK_ERROR_OUT_OF_DATE_KHR:
				case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
				case VK_ERROR_INVALID_SHADER_NV:
				case VK_ERROR_OUT_OF_POOL_MEMORY:
				case VK_ERROR_INVALID_EXTERNAL_HANDLE:
				case VK_ERROR_FRAGMENTATION:
				case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
				case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
				case VK_ERROR_UNKNOWN:
				default:
				return RV_SEVERITY_ERROR;
			}
		}

		constexpr valid_utf8_cstring description8() const
		{
			switch (result)
			{
				case VK_SUCCESS:									return u8"Command successfully completed.";
				case VK_NOT_READY:									return u8"A fence or query has not yet completed.";
				case VK_TIMEOUT:									return u8"A wait operation has not completed in the specified time.";
				case VK_EVENT_SET:									return u8"An event is signaled.";
				case VK_EVENT_RESET:								return u8"An event is unsignaled.";
				case VK_INCOMPLETE:									return u8"A return array was too small for the result.";
				case VK_SUBOPTIMAL_KHR:								return u8"A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully.";
				case VK_THREAD_IDLE_KHR:							return u8"A deferred operation is not complete but there is currently no work for this thread to do at the time of this call.";
				case VK_THREAD_DONE_KHR:							return u8"A deferred operation is not complete but there is no work remaining to assign to additional threads.";
				case VK_OPERATION_DEFERRED_KHR:						return u8"A deferred operation was requested and at least some of the work was deferred.";
				case VK_OPERATION_NOT_DEFERRED_KHR:					return u8"A deferred operation was requested and no operations were deferred.";
				case VK_PIPELINE_COMPILE_REQUIRED:					return u8"A requested pipeline creation would have required compilation, but the application requested compilation to not be performed.";

				case VK_ERROR_OUT_OF_HOST_MEMORY:					return u8"A host memory allocation has failed.";
				case VK_ERROR_OUT_OF_DEVICE_MEMORY:					return u8"A device memory allocation has failed.";
				case VK_ERROR_INITIALIZATION_FAILED:				return u8"Initialization of an object could not be completed for implementation-specific reasons.";
				case VK_ERROR_DEVICE_LOST:							return u8"The logical or physical device has been lost.";
				case VK_ERROR_MEMORY_MAP_FAILED:					return u8"Mapping of a memory object has failed.";
				case VK_ERROR_LAYER_NOT_PRESENT:					return u8"A requested layer is not present or could not be loaded.";
				case VK_ERROR_EXTENSION_NOT_PRESENT:				return u8"A requested extension is not supported.";
				case VK_ERROR_FEATURE_NOT_PRESENT:					return u8"A requested feature is not supported.";
				case VK_ERROR_INCOMPATIBLE_DRIVER:					return u8"The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons.";
				case VK_ERROR_TOO_MANY_OBJECTS:						return u8"Too many objects of the type have already been created.";
				case VK_ERROR_FORMAT_NOT_SUPPORTED:					return u8"A requested format is not supported on this device.";
				case VK_ERROR_FRAGMENTED_POOL:						return u8"A pool allocation has failed due to fragmentation of the pool’s memory.";
				case VK_ERROR_SURFACE_LOST_KHR:						return u8"A surface is no longer available.";
				case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:				return u8"The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again.";
				case VK_ERROR_OUT_OF_DATE_KHR:						return u8"A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail. Applications must query the new surface properties and recreate their swapchain if they wish to continue presenting to the surface.";
				case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:				return u8"The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image.";
				case VK_ERROR_INVALID_SHADER_NV:					return u8"One or more shaders failed to compile or link. More details are reported back to the application via VK_EXT_debug_report if enabled.";
				case VK_ERROR_OUT_OF_POOL_MEMORY:					return u8"A pool memory allocation has failed.";
				case VK_ERROR_INVALID_EXTERNAL_HANDLE:				return u8"An external handle is not a valid handle of the specified type.";
				case VK_ERROR_FRAGMENTATION:						return u8"A descriptor pool creation has failed due to fragmentation.";
				case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:			return u8"A buffer creation failed because the requested address is not available.";
				case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:	return u8"An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exlusive full-screen access. This may occur due to implementation-dependent reasons, outside of the application’s control.";
				case VK_ERROR_UNKNOWN:								return u8"An unknown error has occurred; either the application has provided invalid input, or an implementation failure has occurred.";
				default:											return u8"???";
			}
		}

		constexpr valid_utf16_cstring description16() const
		{
			switch (result)
			{
				case VK_SUCCESS:									return u"Command successfully completed.";
				case VK_NOT_READY:									return u"A fence or query has not yet completed.";
				case VK_TIMEOUT:									return u"A wait operation has not completed in the specified time.";
				case VK_EVENT_SET:									return u"An event is signaled.";
				case VK_EVENT_RESET:								return u"An event is unsignaled.";
				case VK_INCOMPLETE:									return u"A return array was too small for the result.";
				case VK_SUBOPTIMAL_KHR:								return u"A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully.";
				case VK_THREAD_IDLE_KHR:							return u"A deferred operation is not complete but there is currently no work for this thread to do at the time of this call.";
				case VK_THREAD_DONE_KHR:							return u"A deferred operation is not complete but there is no work remaining to assign to additional threads.";
				case VK_OPERATION_DEFERRED_KHR:						return u"A deferred operation was requested and at least some of the work was deferred.";
				case VK_OPERATION_NOT_DEFERRED_KHR:					return u"A deferred operation was requested and no operations were deferred.";
				case VK_PIPELINE_COMPILE_REQUIRED:					return u"A requested pipeline creation would have required compilation, but the application requested compilation to not be performed.";

				case VK_ERROR_OUT_OF_HOST_MEMORY:					return u"A host memory allocation has failed.";
				case VK_ERROR_OUT_OF_DEVICE_MEMORY:					return u"A device memory allocation has failed.";
				case VK_ERROR_INITIALIZATION_FAILED:				return u"Initialization of an object could not be completed for implementation-specific reasons.";
				case VK_ERROR_DEVICE_LOST:							return u"The logical or physical device has been lost.";
				case VK_ERROR_MEMORY_MAP_FAILED:					return u"Mapping of a memory object has failed.";
				case VK_ERROR_LAYER_NOT_PRESENT:					return u"A requested layer is not present or could not be loaded.";
				case VK_ERROR_EXTENSION_NOT_PRESENT:				return u"A requested extension is not supported.";
				case VK_ERROR_FEATURE_NOT_PRESENT:					return u"A requested feature is not supported.";
				case VK_ERROR_INCOMPATIBLE_DRIVER:					return u"The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons.";
				case VK_ERROR_TOO_MANY_OBJECTS:						return u"Too many objects of the type have already been created.";
				case VK_ERROR_FORMAT_NOT_SUPPORTED:					return u"A requested format is not supported on this device.";
				case VK_ERROR_FRAGMENTED_POOL:						return u"A pool allocation has failed due to fragmentation of the pool’s memory.";
				case VK_ERROR_SURFACE_LOST_KHR:						return u"A surface is no longer available.";
				case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:				return u"The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again.";
				case VK_ERROR_OUT_OF_DATE_KHR:						return u"A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail. Applications must query the new surface properties and recreate their swapchain if they wish to continue presenting to the surface.";
				case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:				return u"The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image.";
				case VK_ERROR_INVALID_SHADER_NV:					return u"One or more shaders failed to compile or link. More details are reported back to the application via VK_EXT_debug_report if enabled.";
				case VK_ERROR_OUT_OF_POOL_MEMORY:					return u"A pool memory allocation has failed.";
				case VK_ERROR_INVALID_EXTERNAL_HANDLE:				return u"An external handle is not a valid handle of the specified type.";
				case VK_ERROR_FRAGMENTATION:						return u"A descriptor pool creation has failed due to fragmentation.";
				case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:			return u"A buffer creation failed because the requested address is not available.";
				case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:	return u"An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exlusive full-screen access. This may occur due to implementation-dependent reasons, outside of the application’s control.";
				case VK_ERROR_UNKNOWN:								return u"An unknown error has occurred; either the application has provided invalid input, or an implementation failure has occurred.";
				default:											return u"???";
			}
		}

		constexpr valid_utf8_cstring name8() const
		{
			switch (result)
			{
				case VK_SUCCESS:									return u8"VK_SUCCESS";
				case VK_OPERATION_DEFERRED_KHR:						return u8"VK_OPERATION_DEFERRED_KHR";
				case VK_OPERATION_NOT_DEFERRED_KHR:					return u8"VK_OPERATION_NOT_DEFERRED_KHR";
				case VK_EVENT_SET:									return u8"VK_EVENT_SET";
				case VK_EVENT_RESET:								return u8"VK_EVENT_RESET";

				case VK_NOT_READY:									return u8"VK_NOT_READY";
				case VK_TIMEOUT:									return u8"VK_TIMEOUT";
				case VK_INCOMPLETE:									return u8"VK_INCOMPLETE";
				case VK_SUBOPTIMAL_KHR:								return u8"VK_SUBOPTIMAL_KHR";
				case VK_THREAD_IDLE_KHR:							return u8"VK_THREAD_IDLE_KHR";
				case VK_THREAD_DONE_KHR:							return u8"VK_THREAD_DONE_KHR";
				case VK_PIPELINE_COMPILE_REQUIRED:					return u8"VK_PIPELINE_COMPILE_REQUIRED";

				case VK_ERROR_OUT_OF_HOST_MEMORY:					return u8"VK_ERROR_OUT_OF_HOST_MEMORY";
				case VK_ERROR_OUT_OF_DEVICE_MEMORY:					return u8"VK_ERROR_OUT_OF_DEVICE_MEMORY";
				case VK_ERROR_INITIALIZATION_FAILED:				return u8"VK_ERROR_INITIALIZATION_FAILED";
				case VK_ERROR_DEVICE_LOST:							return u8"VK_ERROR_DEVICE_LOST";
				case VK_ERROR_MEMORY_MAP_FAILED:					return u8"VK_ERROR_MEMORY_MAP_FAILED";
				case VK_ERROR_LAYER_NOT_PRESENT:					return u8"VK_ERROR_LAYER_NOT_PRESENT";
				case VK_ERROR_EXTENSION_NOT_PRESENT:				return u8"VK_ERROR_EXTENSION_NOT_PRESENT";
				case VK_ERROR_FEATURE_NOT_PRESENT:					return u8"VK_ERROR_FEATURE_NOT_PRESENT";
				case VK_ERROR_INCOMPATIBLE_DRIVER:					return u8"VK_ERROR_INCOMPATIBLE_DRIVER";
				case VK_ERROR_TOO_MANY_OBJECTS:						return u8"VK_ERROR_TOO_MANY_OBJECTS";
				case VK_ERROR_FORMAT_NOT_SUPPORTED:					return u8"VK_ERROR_FORMAT_NOT_SUPPORTED";
				case VK_ERROR_FRAGMENTED_POOL:						return u8"VK_ERROR_FRAGMENTED_POOL";
				case VK_ERROR_SURFACE_LOST_KHR:						return u8"VK_ERROR_SURFACE_LOST_KHR";
				case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:				return u8"VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
				case VK_ERROR_OUT_OF_DATE_KHR:						return u8"VK_ERROR_OUT_OF_DATE_KHR";
				case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:				return u8"VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
				case VK_ERROR_INVALID_SHADER_NV:					return u8"VK_ERROR_INVALID_SHADER_NV";
				case VK_ERROR_OUT_OF_POOL_MEMORY:					return u8"VK_ERROR_OUT_OF_POOL_MEMORY";
				case VK_ERROR_INVALID_EXTERNAL_HANDLE:				return u8"VK_ERROR_INVALID_EXTERNAL_HANDLE";
				case VK_ERROR_FRAGMENTATION:						return u8"VK_ERROR_FRAGMENTATION";
				case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:			return u8"VK_ERROR_INVALID_DEVICE_ADDRESS_EXT";
				case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:	return u8"VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
				case VK_ERROR_UNKNOWN:								return u8"VK_ERROR_UNKNOWN";
				default:											return u8"???";
			}
		}

		constexpr valid_utf16_cstring name16() const
		{
			switch (result)
			{
				case VK_SUCCESS:									return u"VK_SUCCESS";
				case VK_OPERATION_DEFERRED_KHR:						return u"VK_OPERATION_DEFERRED_KHR";
				case VK_OPERATION_NOT_DEFERRED_KHR:					return u"VK_OPERATION_NOT_DEFERRED_KHR";
				case VK_EVENT_SET:									return u"VK_EVENT_SET";
				case VK_EVENT_RESET:								return u"VK_EVENT_RESET";

				case VK_NOT_READY:									return u"VK_NOT_READY";
				case VK_TIMEOUT:									return u"VK_TIMEOUT";
				case VK_INCOMPLETE:									return u"VK_INCOMPLETE";
				case VK_SUBOPTIMAL_KHR:								return u"VK_SUBOPTIMAL_KHR";
				case VK_THREAD_IDLE_KHR:							return u"VK_THREAD_IDLE_KHR";
				case VK_THREAD_DONE_KHR:							return u"VK_THREAD_DONE_KHR";
				case VK_PIPELINE_COMPILE_REQUIRED:					return u"VK_PIPELINE_COMPILE_REQUIRED";

				case VK_ERROR_OUT_OF_HOST_MEMORY:					return u"VK_ERROR_OUT_OF_HOST_MEMORY";
				case VK_ERROR_OUT_OF_DEVICE_MEMORY:					return u"VK_ERROR_OUT_OF_DEVICE_MEMORY";
				case VK_ERROR_INITIALIZATION_FAILED:				return u"VK_ERROR_INITIALIZATION_FAILED";
				case VK_ERROR_DEVICE_LOST:							return u"VK_ERROR_DEVICE_LOST";
				case VK_ERROR_MEMORY_MAP_FAILED:					return u"VK_ERROR_MEMORY_MAP_FAILED";
				case VK_ERROR_LAYER_NOT_PRESENT:					return u"VK_ERROR_LAYER_NOT_PRESENT";
				case VK_ERROR_EXTENSION_NOT_PRESENT:				return u"VK_ERROR_EXTENSION_NOT_PRESENT";
				case VK_ERROR_FEATURE_NOT_PRESENT:					return u"VK_ERROR_FEATURE_NOT_PRESENT";
				case VK_ERROR_INCOMPATIBLE_DRIVER:					return u"VK_ERROR_INCOMPATIBLE_DRIVER";
				case VK_ERROR_TOO_MANY_OBJECTS:						return u"VK_ERROR_TOO_MANY_OBJECTS";
				case VK_ERROR_FORMAT_NOT_SUPPORTED:					return u"VK_ERROR_FORMAT_NOT_SUPPORTED";
				case VK_ERROR_FRAGMENTED_POOL:						return u"VK_ERROR_FRAGMENTED_POOL";
				case VK_ERROR_SURFACE_LOST_KHR:						return u"VK_ERROR_SURFACE_LOST_KHR";
				case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:				return u"VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
				case VK_ERROR_OUT_OF_DATE_KHR:						return u"VK_ERROR_OUT_OF_DATE_KHR";
				case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:				return u"VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
				case VK_ERROR_INVALID_SHADER_NV:					return u"VK_ERROR_INVALID_SHADER_NV";
				case VK_ERROR_OUT_OF_POOL_MEMORY:					return u"VK_ERROR_OUT_OF_POOL_MEMORY";
				case VK_ERROR_INVALID_EXTERNAL_HANDLE:				return u"VK_ERROR_INVALID_EXTERNAL_HANDLE";
				case VK_ERROR_FRAGMENTATION:						return u"VK_ERROR_FRAGMENTATION";
				case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:			return u"VK_ERROR_INVALID_DEVICE_ADDRESS_EXT";
				case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:	return u"VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
				case VK_ERROR_UNKNOWN:								return u"VK_ERROR_UNKNOWN";
				default:											return u"???";
			}
		}

		constexpr bool succeeded() const { return severity() == RV_SEVERITY_INFO; }
		constexpr bool failed() const { return severity() == RV_SEVERITY_ERROR || severity() == RV_SEVERITY_WARNING; }
		constexpr bool fatal() const { return severity() == RV_SEVERITY_ERROR; }

		VkResult result = VK_SUCCESS;
	};
}