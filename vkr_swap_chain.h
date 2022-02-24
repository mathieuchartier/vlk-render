#pragma once

#include <algorithm>
#include <vector>
#include <optional>

#include "vkr_image.h"
#include "vkr_util.h"

namespace vkr {

class Engine;

class SwapChain {
public:
  VkSurfaceFormatKHR surface_format_;
  VkPresentModeKHR present_mode_;
  Engine* engine_;

  // Helper
  VkSurfaceCapabilitiesKHR capabilities_;
  std::vector<VkSurfaceFormatKHR> formats_;
  std::vector<VkPresentModeKHR> present_modes_;

  // Images
  std::vector<VkImage> swap_images_;
  std::vector<std::unique_ptr<ImageView>> swap_views_;

  uint32_t ImageMin() const { return capabilities_.minImageCount; }
  uint32_t ImageMax() const { return capabilities_.maxImageCount; }

  template <typename Visitor>
  std::optional<VkSurfaceFormatKHR> ChooseFormat(const Visitor& visitor) {
    for (const auto& format : formats_) {
      if (visitor(format)) {
        return format;
      }
    }
    return {};
  }

  template <typename Visitor>
  std::optional<VkPresentModeKHR> ChoosePresentMode(const Visitor& visitor) {
    for (const auto& mode : present_modes_) {
      if (visitor(mode)) {
        return mode;
      }
    }
    return {};
  }

  VkExtent2D ChooseSwapExtent(uint32_t ext_w, uint32_t ext_h) {
    if (capabilities_.currentExtent.width != UINT32_MAX) {
      return capabilities_.currentExtent;
    }
    return {
      std::clamp(ext_w, capabilities_.minImageExtent.width, capabilities_.maxImageExtent.width),
      std::clamp(ext_h, capabilities_.minImageExtent.height, capabilities_.maxImageExtent.height)
    };
  }

  void Initialize(Engine& engine);

  void Create(VkExtent2D extent, uint32_t image_count);
};

}