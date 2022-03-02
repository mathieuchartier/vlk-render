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

  // Swap chain.
  VkSwapchainKHR swap_chain_; 

  // Frame and present info.
  std::vector<VkSemaphore> img_available_, render_finished_;
  std::vector<VkFence> render_fence_;
  uint32_t current_frame_ = 0u;

  uint32_t ImageMin() const { return capabilities_.minImageCount; }
  uint32_t ImageMax() const { return capabilities_.maxImageCount; }

  VkSwapchainKHR Get() const { return swap_chain_; }

  size_t ImageCount() const { return swap_images_.size(); }

  void InitSync();
  uint32_t BeginFrame();
  void EndFrame(VkQueue gfx_queue, VkQueue present_queue, uint32_t image_idx);

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
