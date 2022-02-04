#pragma once

#include <optional>
#include <vector>

#include "vkr_util.h"

namespace vkr {

class PhysicalDevices {
public:
  VkPhysicalDeviceMemoryProperties mem_properties_{};
  VkPhysicalDeviceProperties properties_{};
  VkPhysicalDeviceLimits limits_{};
  VkPhysicalDevice device_{};
  VkInstance instance_{};
  
  VkPhysicalDevice Device() const { return device_; }

  // Visitor returns priorities.
  template <typename Visitor>
  bool Choose(const Visitor& score_visitor, bool verbose = true) {
    auto devices = EnumeratePhysicalDevices(instance_);
    VkPhysicalDevice best_device = {};
    size_t device_idx = 0;
    int best_score = -1;
    if (verbose) {
      std::cout << "Found " << devices.size() << " physical devices" << std::endl;
    }
    for (auto& dev : devices) {  
      VkPhysicalDeviceProperties properties;
      VkPhysicalDeviceFeatures features;
      vkGetPhysicalDeviceProperties(dev, &properties);
      vkGetPhysicalDeviceFeatures(dev, &features);
      const auto score = score_visitor(properties, features);
      if (verbose) {
        std::cout << "Device " << properties.deviceName << " type=" << properties.deviceType << " score=" << score << std::endl;
      }
      if (score > best_score) {
        best_score = score;
        device_ = dev;
      }
    }
    if (best_score < 0) {
      return false;
    }
    vkGetPhysicalDeviceMemoryProperties(device_, &mem_properties_);
    vkGetPhysicalDeviceProperties(device_, &properties_);
    if (verbose) {
      for (size_t i = 0; i < mem_properties_.memoryHeapCount; ++i) {
        std::cout
          << "Memory heap " << i
          << " size=" << mem_properties_.memoryHeaps[i].size
          << " flags=" << mem_properties_.memoryHeaps[i].flags << std::endl;
      }
    }
    return true;
  }

  VkFormat FindDepthFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, bool stencil) {
    VkFormatFeatureFlags features = stencil ? VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT : 0;
    for (auto fmt : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(device_, fmt, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
          return fmt;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
          return fmt;
        }
    }
    return VK_FORMAT_UNDEFINED;
  }

  std::optional<size_t> GetMemoryTypeIdx(size_t supported, size_t requirements) const {
    for (size_t i = 0; i < mem_properties_.memoryTypeCount; ++i) {
      if ((supported & (1u << i)) && (mem_properties_.memoryTypes[i].propertyFlags & requirements) == requirements) {
        return i;
      }
    }
    return {};
  }

  const VkPhysicalDeviceMemoryProperties& MemProperties() const { return mem_properties_; }
  const VkPhysicalDeviceProperties& Properties() const { return properties_; }
};

}