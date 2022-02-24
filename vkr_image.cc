#include "vkr_image.h"

#include <memory>

#include "vkr_engine.h"

namespace vkr {

Image::~Image() {
  vkDestroyImage(engine_->Device(), image_, engine_->Callbacks());
}

ImageView::~ImageView() {
  vkDestroyImageView(engine_->Device(), view_, engine_->Callbacks());
}

std::unique_ptr<ImageView> ImageView::Create(Engine* engine, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags) {
  VkImageViewCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.image = image;
  info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  info.format = format;
  info.subresourceRange.aspectMask = aspect_flags;
  info.subresourceRange.baseMipLevel = 0;
  info.subresourceRange.levelCount = 1;
  info.subresourceRange.baseArrayLayer = 0;
  info.subresourceRange.layerCount = 1;
  VkImageView view;
  if (vkCreateImageView(engine->Device(), &info, nullptr, &view) != VK_SUCCESS) {
    return nullptr;
  }
  return std::make_unique<ImageView>(engine, view);
}

}