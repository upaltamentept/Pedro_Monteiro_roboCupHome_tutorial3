#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <string>
#include <vector>
#include <map>

#include <tf/transform_broadcaster.h>
#include <tiago_move/controller.h>

namespace tiago_move
{
  // Creates a typedef for a SimpleActionClient that communicates with actions that adhere to the MoveBaseAction action interface.
  // typedef /*#>>>>TODO: ACTION CLIENT TYPE*/</*#>>>>TODO: ACTION NAME*/> MoveBaseClient;
  Controller::Controller() : ac("move_base", true), body_planner_("arm_torso")
  {
  }

  Controller::~Controller()
  {
  }

  bool Controller::initialize(ros::NodeHandle &nh)
  {
    // Ver
    // actionlib::SimpleActionClient<move_base_msgs / MoveBaseAction> ac("your_action_server", true);
    /*#>>>>TODO:Exercise3 wait for the action server to connet*/
    // actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> actionClient("move_base", true);
    while (!ac.waitForServer(ros::Duration(5.0)))
    {
      ROS_INFO("Waiting for the move_base action server to come up");
    }
    std::vector<double> waypointA, waypointB, waypointC;
    ros::param::get("waypoint_A", waypointA);
    ros::param::get("waypoint_B", waypointB);
    ros::param::get("waypoint_C", waypointC);
    move_base_msgs::MoveBaseGoal msgA = createGoal(waypointA);
    move_base_msgs::MoveBaseGoal msgB = createGoal(waypointB);
    move_base_msgs::MoveBaseGoal msgC = createGoal(waypointC);
    nav_goals.push_back(msgA);
    nav_goals.push_back(msgB);
    nav_goals.push_back(msgC);
    // Ver
    /*#>>>>TODO:Exercise3 Load the 3D coordinates of three waypoints from the ROS parameter server*/
    /*#>>>>TODO:Exercise3 Store three waypoints in the vector nav_goals*/

    // #>>>>TODO:Exercise4 Load the target pose from parameter
    if (!ros::param::get(/*#>>>>TODO: PARAMETER NAME*/ "target_pose", target_pose))
      return false;
    body_planner_.setPlannerId("RRtConnectkConfigDefault");
    body_planner_.setPoseReferenceFrame("map");
    // #>>>>TODO:Exercise4 Set the planner of your MoveGroupInterface

    // armtorso.setPoseReferenceFrame("reference_frame");
    //  #>>>>TODO:Exercise4 Set the reference frame of your target pose
    return true;
  }

  move_base_msgs::MoveBaseGoal Controller::createGoal(std::vector<double> &goal)
  {
    move_base_msgs::MoveBaseGoal goal_msg;

    goal_msg.target_pose.header.frame_id = "map"; // #>>>>TODO: the reference frame name
    goal_msg.target_pose.header.stamp = ros::Time::now();
    goal_msg.target_pose.pose.position.x = goal[0];
    goal_msg.target_pose.pose.position.y = goal[1];
    goal_msg.target_pose.pose.orientation.w = goal[2];

    return goal_msg;
  }

  // Uncomment the function for Exercise 4
  int Controller::move_arm(std::vector<double> &goal)
  { // #>>>>TODO:Exercise4 Create a msg of type geometry_msgs::PoseStamped from the input vector
    geometry_msgs::PoseStamped targetpose;
    // input: std::vector<double> &goal [x,y,z,r,p,y]
    // do the same as for create Goal
    targetpose.header.frame_id = "map"; // #>>>>TODO: the reference frame name
    targetpose.header.stamp = ros::Time::now();
    targetpose.pose.position.x = goal[0];
    targetpose.pose.position.y = goal[1];
    targetpose.pose.position.z = goal[2];
    //Orientation
    targetpose.pose.orientation.x = goal[3];
    targetpose.pose.orientation.y = goal[4];
    targetpose.pose.orientation.z = goal[5];
    targetpose.pose.orientation.w = 1;
    // #>>>>TODO:Exercise4 Set the target pose for the planner setPoseTarget function of your MoveGroupInterface instance
    body_planner_.setPoseTarget(targetpose);
    ROS_INFO_STREAM("Planning to move " << body_planner_.getEndEffectorLink() << " to a target pose expressed in " << body_planner_.getPlanningFrame());

    body_planner_.setStartStateToCurrentState();
    body_planner_.setMaxVelocityScalingFactor(1.0);

    moveit::planning_interface::MoveGroupInterface::Plan motion_plan;
    // set maximum time to find a plan
    body_planner_.setPlanningTime(5.0);

    // #>>>>TODO:Exercise4 Start the planning by calling member function "plan" and pass the motion_plan as argument
    body_planner_.plan(motion_plan);
    ROS_INFO_STREAM("Plan found in " << motion_plan.planning_time_ << " seconds");
    // #>>>>TODO:Exercise4 Execute the plan by calling member function "move" of the MoveGroupInterface instance
    body_planner_.move();
    return EXIT_SUCCESS;
  }

}
int main(int argc, char **argv)
{
  ros::init(argc, argv, "tiago_move");
  ros::NodeHandle nh;

  ros::AsyncSpinner spinner(4);
  spinner.start();
  tiago_move::Controller controller;
  if (!controller.initialize(nh))
  {
    ROS_ERROR_STREAM("tiago_move::Controller failed to initialize");
    return -1;
  }

  int goal_index = 0, final = 0;
  while (ros::ok())
  {

    ROS_INFO("Sending goal %d", goal_index + 1);
    if (goal_index == 2)
    {
      final = 1;
    }
    // #>>>>TODO:Exercise3 send the current goal to the action server with sendGoal function of SimpleActionClient instace.
    // actionlib::SimpleActionClient<move_base_msgs::MoveBaseGoal> actionClient("move_base", true);

    controller.ac.sendGoal(controller.nav_goals[goal_index]);

    // actionClient.sendGoal(nav_goals[goal_index]);

    // #>>>>TODO:Exercise3 blocks until this goal finishes with waitForResult function of SimpleActionClient instace.
    while (!controller.ac.waitForResult())
    {
      ROS_INFO("Waiting for the move_base action server to send coordinates");
    }
    actionlib::SimpleClientGoalState state = controller.ac.getState();
    if (state == actionlib::SimpleClientGoalState::SUCCEEDED /*#>>>>TODO:Exercise3 Check if the state of this goal is SUCCEEDED use getState function of SimpleActionClient instace*/)
    // #>>>> Hint: see https://docs.ros.org/en/diamondback/api/actionlib/html/classactionlib\_1\_1SimpleActionClient.html for the return type of getState function
    {
      ROS_INFO("Reached goal %d", goal_index + 1);
      goal_index = (goal_index + 1) % controller.nav_goals.size();
    }
    else
    {
      ROS_WARN("Failed to reach goal %d", goal_index + 1);
      ros::Duration(1.0).sleep();
    }
    // #>>>>TODO:Exercise4 Call the move_arm function at propoer waypoint
    if (final)
    {
      controller.move_arm(controller.target_pose);
    }
  }
  spinner.stop();
  return 0;
}
