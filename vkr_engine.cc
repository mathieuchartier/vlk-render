#include "vkr_engine.h"

namespace vkr {

std::vector<VkLayerProperties> Engine::EnumerateInstanceLayerProperties() {
  uint32_t count = 0;
  vkEnumerateInstanceLayerProperties(&count, nullptr);
  std::vector<VkLayerProperties> ret(count);
  vkEnumerateInstanceLayerProperties(&count, ret.data());
  return ret;
}

ResultHelper Engine::CreateInstance(const std::vector<const char*>& instance_ext, const char* app_name, uint32_t app_version) {
  VkApplicationInfo app = {};
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
    
  auto res = vkCreateInstance(&instance_info, nullptr, &instance_);
  phys_devices_.instance_ = instance_;
  return res;
}

}