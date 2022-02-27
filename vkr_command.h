#pragma once

#include "vkr_util.h"

namespace vkr {

class CommandBuffer {
  VkCommandBuffer buf_;
public:
  CommandBuffer(VkCommandBuffer buf) : buf_(buf) {}
  VkCommandBuffer Buf() const { return buf_; }
};

class CommandPool {
  VkDevice device_{};
  VkCommandPool pool_{};
public:
  CommandPool(VkDevice device, VkCommandPool pool) : device_(device), pool_(pool) {}

  static CommandPool* Create(VkDevice device, uint32_t family_idx) {
	VkCommandPoolCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.queueFamilyIndex = family_idx;
	info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	VkCommandPool pool{};
	auto res = vkCreateCommandPool(device, &info, nullptr, &pool);
	return new CommandPool(device, pool);
  }

  CommandBuffer* Allocate() {
	VkCommandBufferAllocateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.commandPool = pool_;
	info.commandBufferCount = 1;
	info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	VkCommandBuffer buffer;
	vkAllocateCommandBuffers(device_, &info, &buffer);
	return new CommandBuffer(buffer);
  }
};

}