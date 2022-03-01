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

  
  void InitSync() {
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

  uint32_t BeginFrame() {
    auto res = vkWaitForFences(engine_->Device(), 1, &render_fence_[current_frame_], VK_TRUE, UINT64_MAX);
    uint32_t image_idx{};
    VkResult res = vkAcquireNextImageKHR(engine_->Device(), swap_chain_, UINT64_MAX, img_available_[current_frame_], VK_NULL_HANDLE, &image_idx);
    if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
      // Recreate swap chain??
    } else if (res != VK_SUCCESS) {
      std::cerr << "Error with acquiring next image" << std::endl;
    }
    return image_idx;

  }

  void EndFrame(uint32_t image_idx) {
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
