#ifndef HDL_GRAPH_SLAM_KEYFRAME_VIEW_HPP
#define HDL_GRAPH_SLAM_KEYFRAME_VIEW_HPP

#include <memory>
#include <glk/pointcloud_buffer.hpp>
#include <glk/primitives/primitives.hpp>

#include <hdl_graph_slam/keyframe.hpp>
#include <hdl_graph_slam/view/drawable_object.hpp>

namespace hdl_graph_slam {

class KeyFrameView : public DrawableObject {
  public:
    using Ptr = std::shared_ptr<KeyFrameView>;

    KeyFrameView(const KeyFrame::Ptr& kf) {
        keyframe = kf;

        std::cout << kf->id() << " : ";
        pointcloud_buffer.reset(new glk::PointCloudBuffer(kf->cloud));
    }

    KeyFrame::Ptr lock() const { return keyframe.lock(); }

    virtual bool available() const override { return !keyframe.expired(); }

    virtual void draw(glk::GLSLShader& shader) override {
      if(!available()) {
        return;
      }

      KeyFrame::Ptr kf = keyframe.lock();
      Eigen::Matrix4f model_matrix = kf->estimate().matrix().cast<float>();
      shader.set_uniform("color_mode", 0);
      shader.set_uniform("model_matrix", model_matrix);
      shader.set_uniform("info_values", Eigen::Vector4i(POINTS, 0, 0, 0));

      pointcloud_buffer->draw(shader);

      shader.set_uniform("color_mode", 1);
      shader.set_uniform("material_color", Eigen::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
      shader.set_uniform("info_values", Eigen::Vector4i(KEYFRAME, kf->id(), 0, 0));

      model_matrix.block<3, 3>(0, 0) *= 0.35;
      shader.set_uniform("model_matrix", model_matrix);
      const auto& sphere = glk::Primitives::instance()->primitive(glk::Primitives::SPHERE);
      sphere.draw(shader);
    }

    virtual void draw(glk::GLSLShader& shader, const Eigen::Vector4f& color, const Eigen::Matrix4f& model_matrix) override {
      if(!available()) {
        return;
      }

      KeyFrame::Ptr kf = keyframe.lock();

      shader.set_uniform("color_mode", 1);
      shader.set_uniform("material_color", color);

      shader.set_uniform("model_matrix", model_matrix);
      shader.set_uniform("info_values", Eigen::Vector4i(POINTS, 0, 0, 0));

      pointcloud_buffer->draw(shader);

      shader.set_uniform("color_mode", 1);
      shader.set_uniform("info_values", Eigen::Vector4i(KEYFRAME, kf->id(), 0, 0));
      const auto& sphere = glk::Primitives::instance()->primitive(glk::Primitives::SPHERE);
      sphere.draw(shader);
    }

  private:
    std::weak_ptr<KeyFrame> keyframe;
    std::unique_ptr<glk::PointCloudBuffer> pointcloud_buffer;
};

}  // namespace hdl_graph_slam

#endif