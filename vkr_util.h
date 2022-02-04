#pragma once

#include <vulkan/vulkan.h>

#include <cstdlib>
#include <iostream>

namespace vkr {

class ResultHelper {
public:
  VkResult value_;

  ResultHelper(VkResult value) : value_(value) {}
  ResultHelper(const ResultHelper&) = default;
  ResultHelper& operator=(const ResultHelper&) = default;

  bool Success() const { return value_ == VK_SUCCESS; }
  bool Failed() const { return !Success(); }

  void Assert(const char* message = "") {
    if (!Success()) {
      std::cerr << "VkResult failed value=" << static_cast<uint32_t>(value_) << " " << message << std::endl;
      std::abort();
    }
  }
};

inline std::vector<VkLayerProperties> EnumerateInstanceLayerProperties() {
  uint32_t count = 0;
  vkEnumerateInstanceLayerProperties(&count, nullptr);
  std::vector<VkLayerProperties> ret(count);
  vkEnumerateInstanceLayerProperties(&count, ret.data());
  return ret;
}

inline std::vector<VkPhysicalDevice> EnumeratePhysicalDevices(VkInstance instance) {
  uint32_t count = 0;
  auto res = vkEnumeratePhysicalDevices(instance, &count, nullptr);
  std::vector<VkPhysicalDevice> ret(count);
  vkEnumeratePhysicalDevices(instance, &count, ret.data());
  return ret;
}

}