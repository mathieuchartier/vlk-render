#pragma once

#include "vkr_util.h"

namespace vkr {

class Image {
public:
  Engine* engine_;
  VkImage image_{};
  VkDeviceMemory memory_{};
  VkImageCreateInfo image_info_{};

  Image(Engine* engine, VkImage image, VkDeviceMemory memory, const VkImageCreateInfo& info)
      : engine_(engine), image_(image), memory_(memory), image_info_(info) {}
  virtual ~Image();
};

class ImageView {
public:
  Engine* engine_{};
  VkImageView view_{};

  ImageView(Engine* engine, VkImageView view) : engine_(engine), view_(view) {}
  virtual ~ImageView();

  static std::unique_ptr<ImageView> Create(Engine* engine, VkImage image, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM, VkImageAspectFlags aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT);
};

class DepthBuffer {
public:
};

}