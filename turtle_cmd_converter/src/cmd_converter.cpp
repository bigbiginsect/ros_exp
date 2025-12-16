// cmd_converter.cpp
// 订阅一个 Twist，做简单缩放/反向变换后重新发布，并把收到/发布的值打印到终端
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>

class CmdConverter {
public:
  CmdConverter() : nh_(), pnh_("~") {
    // 从私有命名空间读取参数（在launch或命令行可以覆盖）
    pnh_.param("linear_scale", linear_scale_, 0.5);   // 线速度缩放系数（默认0.5）
    pnh_.param("angular_scale", angular_scale_, 0.5); // 角速度缩放系数（默认0.5）
    pnh_.param("invert_angular", invert_angular_, false); // 是否反向角速度

    // 订阅源话题（默认名 "cmd_vel"，在launch中可以 remap）
    sub_ = nh_.subscribe("cmd_vel", 10, &CmdConverter::cmdCallback, this);
    // 发布到转换后的话题 /cmd_vel_cov （也可以 remap）
    pub_ = nh_.advertise<geometry_msgs::Twist>("cmd_vel_cov", 10);

    ROS_INFO("CmdConverter started. linear_scale=%.3f angular_scale=%.3f invert_angular=%s",
             linear_scale_, angular_scale_, invert_angular_ ? "true" : "false");
  }

private:
  void cmdCallback(const geometry_msgs::Twist::ConstPtr& msg) {
    // 打印收到的原始值
    ROS_INFO("Received -> linear.x: %.3f, linear.y: %.3f, angular.z: %.3f",
             msg->linear.x, msg->linear.y, msg->angular.z);

    geometry_msgs::Twist out = *msg;
    // 只处理常用的线速度 x 和角速度 z，按参数缩放并可能反向
    out.linear.x = out.linear.x * linear_scale_;
    out.angular.z = out.angular.z * angular_scale_;
    if (invert_angular_) out.angular.z = -out.angular.z;

    // 发布转换后的消息
    pub_.publish(out);

    // 打印发布的值（便于验收时在终端看到）
    ROS_INFO("Published -> linear.x: %.3f, angular.z: %.3f", out.linear.x, out.angular.z);
  }

  ros::NodeHandle nh_, pnh_;
  ros::Subscriber sub_;
  ros::Publisher pub_;
  double linear_scale_, angular_scale_;
  bool invert_angular_;
};

int main(int argc, char** argv) {
  ros::init(argc, argv, "cmd_converter_node");
  CmdConverter node;
  ros::spin();
  return 0;
}
