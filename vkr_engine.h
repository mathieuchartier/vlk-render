#pragma once

#include <map>
#include <vulkan/vulkan.h>
#include <vector>

#include "vkr_image.h"
#include "vkr_physical_devices.h"
#include "vkr_swap_chain.h"
#include "vkr_util.h"

#pragma comment(lib, "vulkan-1.lib")

namespace vkr {

class Engine {
public:
  VkInstance instance_{};
  PhysicalDevices phys_devices_{};
  VkDevice device_{};
  VkSurfaceKHR surface_{};
  std::optional<uint32_t> gfx_family_, present_family_;
  std::vector<uint32_t> queue_family_indices_;
  SwapChain swap_chain_;

  VkSurfaceKHR Surface() const { return surface_; }
  VkDevice Device() const { return device_; }

  const VkAllocationCallbacks* Callbacks() {
    return nullptr;
  }

  virtual ~Engine() {
    vkDestroySurfaceKHR(instance_, surface_, Callbacks());
    vkDestroyInstance(instance_, Callbacks());
  }
  
  void InitSwapChain() {
    swap_chain_.Initialize(*this);
  }

  static inline std::vector<VkLayerProperties> EnumerateInstanceLayerProperties();
  VkInstance Instance() const { return instance_; }
  ResultHelper CreateInstance(
    const std::vector<const char*>& instance_ext,
    const char* app_name = "app name",
    uint32_t app_version = VK_MAKE_VERSION(1, 0, 0));
  ResultHelper ChoosePhysicalDevice(
    VkSurfaceKHR surface,
    const std::vector<VkPhysicalDeviceType>& device_type_prio) {
    surface_ = surface;
    std::map<VkPhysicalDeviceType, int> dev_type_scores;
    int score_mult = 10;
    for (auto it = device_type_prio.rbegin(); it != device_type_prio.rend(); ++it) {
      dev_type_scores[*it] = score_mult;
      score_mult *= 10;
    }
    if (!phys_devices_.Choose([&](
      const VkPhysicalDeviceProperties& properties,
      const VkPhysicalDeviceFeatures& features) -> int {
        auto it = dev_type_scores.find(properties.deviceType);
        if (it == dev_type_scores.end()) return -1;
        auto ret = it->second;
        return ret;
      }, true)) {
      return VK_ERROR_INITIALIZATION_FAILED;
    }

    int i = 0;
    std::optional<uint32_t> gfx_family, present_family;
    for (const auto& queue : phys_devices_.GetPhysicalDeviceQueueFamilyProperties()) {
      if (queue.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        gfx_family = i;
      }
      VkBool32 support = VK_FALSE;
      vkGetPhysicalDeviceSurfaceSupportKHR(phys_devices_.Device(), i, surface, &support);
      if (support == VK_TRUE) {
        present_family = i;
      }
      i++;
    }
    if (!gfx_family.has_value() || !present_family.has_value()) {
      return VK_ERROR_INITIALIZATION_FAILED;
    }
    return VK_SUCCESS;
  }

  ResultHelper CreateDevice(const std::vector<const char*>& layers) {
    float prio = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    queue_family_indices_ = {
      gfx_family_.value(),
      present_family_.value()
    };
    for (auto& family : queue_family_indices_) {
      VkDeviceQueueCreateInfo queue_create_info = {};
      queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_create_info.queueFamilyIndex = family;
      queue_create_info.queueCount = 1;
      queue_create_info.pQueuePriorities = &prio;
      queue_create_infos.push_back(queue_create_info);
    }
    const std::vector<const char*> device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    VkPhysicalDeviceFeatures device_features{};
    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.queueCreateInfoCount = queue_create_infos.size();
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledLayerCount = static_cast<uint32_t>(layers.size()),
    create_info.ppEnabledLayerNames = layers.data();
    create_info.enabledExtensionCount = device_extensions.size();
    create_info.ppEnabledExtensionNames = device_extensions.data();

    return vkCreateDevice(phys_devices_.Device(), &create_info, nullptr, &device_);
  }

  std::unique_ptr<Image> CreateImage(VkExtent3D ext, VkFormat fmt, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent = ext;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = fmt;
    image_info.tiling = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = usage;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkImage image;
    if (vkCreateImage(Device(), &image_info, nullptr, &image) != VK_SUCCESS) {
      return nullptr;
    }
    VkMemoryRequirements reqs;
    vkGetImageMemoryRequirements(Device(), image, &reqs);
    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = reqs.size;
    alloc_info.memoryTypeIndex = phys_devices_.GetMemoryTypeIdx(reqs.memoryTypeBits, properties).value();
    VkDeviceMemory memory;
    if (vkAllocateMemory(Device(), &alloc_info, nullptr, &memory) != VK_SUCCESS) {
      return nullptr;
    }
    vkBindImageMemory(Device(), image, memory, 0);
    return std::unique_ptr<Image>(new Image(this, image, memory, image_info));
  }
};

}
