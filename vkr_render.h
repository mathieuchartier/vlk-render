#pragma once

#include "vkr_util.h"

class SubPass {
public:
  std::vector<VkAttachmentReference> attachments_;
  VkSubpassDescription subpass_;

  SubPass(const std::vector<VkAttachmentReference>& attachments) : attachments_(attachments) {
	subpass_.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_.colorAttachmentCount = attachments_.size();
	subpass_.pColorAttachments = attachments_.data();
  }
};

class FrameBuffer {
  VkFramebuffer buf_;
public:
  FrameBuffer(VkFramebuffer buf) : buf_(buf) {}
};

class RenderPass {
  VkRenderPass render_pass_;
public:
  static void Create(VkFormat format) {
	VkAttachmentDescription ca{};
	ca.format = format;
	ca.samples = VK_SAMPLE_COUNT_1_BIT;
	ca.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	ca.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	ca.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	ca.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	ca.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	ca.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkRenderPassCreateInfo rpi{};
	rpi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	rpi.attachmentCount = 1;
	rpi.pAttachments = &ca;
	rpi.subpassCount = 1;
	rpi.pSubpasses = &subpass;
	VkRenderPass ret;
	vkCreateRenderPass(_device, &rpi, nullptr, &ret);
  }

  std::unique_ptr<FrameBuffer> CreateFrameBuffer(VkDevice device, VkImageView swap_chain_view, VkExtent2D extent) {
	//create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
	VkFramebufferCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	info.renderPass = render_pass_;
	info.attachmentCount = 1;
	info.width = extent.width;
	info.height = extent.height;
	info.layers = 1;
	info.pAttachments = &swap_chain_view;
	VkFramebuffer fb{};
	auto res = vkCreateFramebuffer(device, &info, nullptr, &fb);
	return std::make_unique<FrameBuffer>(fb);
  }
};
