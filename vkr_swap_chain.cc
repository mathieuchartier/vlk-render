#include "vkr_swap_chain.h"
#include "vkr_engine.h"

namespace vkr {

void SwapChain::Initialize(Engine& engine) {
  engine_ = &engine;
  auto device = engine.phys_devices_.Device();
  auto surface = engine.Surface();
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities_);
  uint32_t count = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, nullptr);
  formats_.resize(count);
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, formats_.data());
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, nullptr);
  present_modes_.resize(count);
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, present_modes_.data());
}

void SwapChain::Create(VkExtent2D extent, uint32_t image_count) {
  VkSwapchainCreateInfoKHR swap_create{};
  swap_create.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swap_create.surface = engine_->Surface();
  swap_create.minImageCount = image_count;
  swap_create.imageFormat = surface_format_.format;
  swap_create.imageColorSpace = surface_format_.colorSpace;
  swap_create.imageExtent = extent;
  swap_create.imageArrayLayers = 1;
  swap_create.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  if (engine_->gfx_family_.value() != engine_->present_family_.value()) {
    swap_create.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swap_create.queueFamilyIndexCount = engine_->queue_family_indices_.size();
    swap_create.pQueueFamilyIndices = engine_->queue_family_indices_.data();
  } else {
    swap_create.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }
  swap_create.preTransform = capabilities_.currentTransform;
  swap_create.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swap_create.presentMode = present_mode_;
  swap_create.clipped = VK_TRUE;
  swap_create.oldSwapchain = VK_NULL_HANDLE;
  
  VkSwapchainKHR swap_chain{};
  if (vkCreateSwapchainKHR(engine_->Device(), &swap_create, nullptr, &swap_chain) != VK_SUCCESS) {
    std::cerr << "Failed to create swap chain" << std::endl;
  }
  uint32_t count = 0;
  vkGetSwapchainImagesKHR(engine_->Device(), swap_chain, &count, nullptr);
  swap_images_.resize(count);
  vkGetSwapchainImagesKHR(engine_->Device(), swap_chain, &count, swap_images_.data());
  for (auto& image : swap_images_) {
    auto view = ImageView::Create(engine_, image, surface_format_.format);
    swap_views_.push_back(std::move(view));
  }
}

}