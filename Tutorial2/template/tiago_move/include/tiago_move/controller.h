#ifndef TIAGO_MOVE_H_
#define TIAGO_MOVE_H_

#include <string>
#include <vector>
#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <tf/transform_broadcaster.h>
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;
namespace tiago_move
{

  class Controller
  {
  private:
    // #>>>>TODO:Exercise4 create a MoveGroupInterface instance with name "body_planner_" for MoveGroup "armtorso"
    moveit::planning_interface::MoveGroupInterface body_planner_ = moveit::planning_interface::MoveGroupInterface("arm_torso");

  public:
    Controller();

    virtual ~Controller();

    bool initialize(ros::NodeHandle &nh);

    move_base_msgs::MoveBaseGoal createGoal(std::vector<double> &);

    // MoveBaseClient ac("move_base", true);

    std::vector<double> target_pose;

    // #>>>>TODO:Exercise3 Creates a SimpleActionClient x communicate with the move_base action server.
    //;

    // MoveBaseClient ac("move_base",true);
    actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> ac;

    // #>>>>TODO:Exercise3 create a vector called nav_goals to store the waypoints
    std::vector<move_base_msgs::MoveBaseGoal> nav_goals; //<< move_base_msgs::MoveBaseGoal;

    // Uncomment the function for Exercise 4
    int move_arm(std::vector<double> &);
  };
}

#endif
