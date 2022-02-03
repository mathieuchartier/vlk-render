#include <vulkan/vulkan.h>

#include <algorithm>
#include <fstream>
#include <optional>
#include <vector>

namespace vkr {
  class Instance {
  public:
    VkInstance instance_ = VK_NULL_HANDLE;

    static inline std::vector<VkLayerProperties> EnumerateInstanceLayerProperties() {
      uint32_t count = 0;
      vkEnumerateInstanceLayerProperties(&count, nullptr);
      std::vector<VkLayerProperties> ret(count);
      vkEnumerateInstanceLayerProperties(&count, ret.data());
      return ret;
    }

    std::unique_ptr<Instance> Create(
      const std::vector<const char*> instance_ext,
      const char* app_name = "app name",
      uint32_t app_version = VK_MAKE_VERSION(1, 0, 0),
      ) {
      const VkApplicationInfo app = {};
      app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      app.pApplicationName = app_name;
      app.applicationVersion = app_version;
      app.pEngineName = "vkr engine";
      app.engineVersion = VK_MAKE_VERSION(1, 0, 0);
      app.apiVersion = VK_API_VERSION_1_2;

      std::vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" };
      VkInstanceCreateInfo instance_info = {};
      instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      instance_info.pApplicationInfo = &app;
      instance_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
      instance_info.ppEnabledLayerNames = layers.data();
      instance_info.enabledExtensionCount = static_cast<uint32_t>(instance_ext.size());
      instance_info.ppEnabledExtensionNames = instance_ext.data();
    
      VkInstance instance;
      auto res = vkCreateInstance(&instance_info, nullptr, &instance);
      if (res != VK_SUCCESS) {
        return nullptr;
      }
      return std::unique_ptr<Instance>(new Instance{instance_});
    }
  };

  class Device {
  };

  class PhysicalDevice {
  };
}