#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "vkr_physical_devices.h"
#include "vkr_util.h"

namespace vkr {

class Engine {
public:
  VkInstance instance_{};
  PhysicalDevices phys_devices_{};

  static inline std::vector<VkLayerProperties> EnumerateInstanceLayerProperties();
  ResultHelper CreateInstance(
    const std::vector<const char*>& instance_ext,
    const char* app_name = "app name",
    uint32_t app_version = VK_MAKE_VERSION(1, 0, 0));
};

}
