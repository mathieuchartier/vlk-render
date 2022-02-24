#pragma once

#include <string>

namespace vkr {

class Pipeline {
public:
  class StaticState {
  public:
    bool alpha_blending_;
    std::string vertex_shader_;
    std::string frag_shader_;
  };
private:
  StaticState state_;
public:
  const StaticState& GetStaticState() const { return state_; }

  void Create() {
  }
};

}
