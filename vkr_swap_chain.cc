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

void SwapChain::InitSync() {
  VkSemaphoreCreateInfo sem_info{};
  sem_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  for (size_t i = 0; i < ImageCount(); ++i) {
    VkSemaphore sem{};
    vkCreateSemaphore(engine_->Device(), &sem_info, engine_->Callbacks(), &sem);
    img_available_.push_back(sem);
    vkCreateSemaphore(engine_->Device(), &sem_info, engine_->Callbacks(), &sem);
    render_finished_.push_back(sem);
    VkFenceCreateInfo fence_info;
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    VkFence fence{};
    vkCreateFence(engine_->Device(), &fence_info, engine_->Callbacks(), &fence);
    render_fence_.push_back(fence);
  }
}

uint32_t SwapChain::BeginFrame() {
  vkWaitForFences(engine_->Device(), 1, &render_fence_[current_frame_], VK_TRUE, UINT64_MAX);
  uint32_t image_idx{};
  VkResult res = vkAcquireNextImageKHR(engine_->Device(), swap_chain_, UINT64_MAX, img_available_[current_frame_], VK_NULL_HANDLE, &image_idx);
  if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
    // Recreate swap chain??
  } else if (res != VK_SUCCESS) {
    std::cerr << "Error with acquiring next image" << std::endl;
  }
  return image_idx;
}

void SwapChain::EndFrame(VkQueue gfx_queue, VkQueue present_queue, uint32_t image_idx) {
  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore wait_sem[] = {img_available_[current_frame_]};
  VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_sem;
  submit_info.pWaitDstStageMask = wait_stages;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = nullptr; // &command_buffer[image_idx_];
  VkSemaphore signal_sems[] = {render_finished_[current_frame_]};
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signal_sems;  
  vkResetFences(engine_->Device(), 1, &render_fence_[current_frame_]);
  if (vkQueueSubmit(gfx_queue, 1, &submit_info, render_fence_[current_frame_]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signal_sems;
  VkSwapchainKHR swap_chains[] = {swap_chain_};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swap_chains;
  presentInfo.pImageIndices = &image_idx;
  presentInfo.pResults = nullptr; // Optional
  vkQueuePresentKHR(present_queue, &presentInfo);

  current_frame_ = (current_frame_ + 1) % img_available_.size();
}

}