#include <ros/ros.h>
#include <std_srvs/Empty.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <tiago_localization/localizer.h>

namespace tiago_localization
{

    Localizer::Localizer()
    {
    }

    Localizer::~Localizer()
    {
    }

    bool Localizer::initialize(ros::NodeHandle &nh)
    {
        covariance_sum = 0.0;
        // setup ros connections sudo apt-get install ros-noetic-key-teleop
        // #>>>>TODO: Subscribe to the pose msg published by the package amcl
        pose_sub_ = nh.subscribe("/amcl_pose", 1, &Localizer::poseCallback, this);
        // #>>>> Hint: see http://wiki.ros.org/amcl

        // #>>>>TODO: advertise a geometry_msgs::Twist to command the robot by velocity
        vel_pub_ = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 10);

        // #>>>>TODO: create a serviceClient that calls /global_localization service with empty srv of type std_srvs::Empty
        localization_client_ = nh.serviceClient<std_srvs::Empty>("/global_localization");

        /* spin_speed: 0.0
        thresho d: 0.001 */
        if (!ros::param::get(/*#>>>>TODO: PARAMETER NAME*/ "spin_speed", speed))
            return false;
        if (!ros::param::get(/*#>>>>TODO: PARAMETER NAME*/ "threshold", threshold))
            return false;

        // #>>>>TODO: create an empty srv
        std_srvs::Empty empty_srv;
        if (localization_client_.call(empty_srv /*#>>>>TODO: YOUR SRV NAME*/))
        {
            ROS_INFO("Global localization initialized.");
        }
        else
        {
            ROS_ERROR("Failed to call service /global_localization");
        }

        return true;
    }

    void Localizer::robot_spin(double speed)
    {

        geometry_msgs::Twist msg;
        msg.angular.z = 1; // adjust value as needed for the robot to spin

        ros::Rate rate(10); // 10 Hz
        while (ros::ok())
        {

            vel_pub_.publish(msg);
            ros::spinOnce();
            rate.sleep();
        }
    }
    //----------------------------------------------------------------------------
    // callbacks

    void Localizer::poseCallback(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr &msg)
    {
        // #>>>>TODO: Calculate the time interval between the current ROS time and the timestamp in the message.

        ros::Time now = ros::Time::now();
        // Take current ros time
        ros::Time msg_time = msg->header.stamp;
        // take current message time
        //  Calculate the time interval in seconds
        // double interval = (now-msg_time).toSec();
        ros::Duration interval = (now - msg_time);

        /***********************+Was using double but switched to ros::Time*************++*/

        // std::vector<double> covariance = msg->pose.covariance;
        covariance_sum = 0;
        if (/*#>>>>TODO: CHECK THE INTERVAL IS WITHIN 0.5 SECOND*/ interval < ros::Duration(0.5))
            // #>>>>TODO: Calculate the sum of the covariance vector in the msg (covariance_sum)

            for (const auto &value : msg->pose.covariance)
            {
                covariance_sum += value;
            }
        /* msg->pose.covariance
            covariance_sum += ; */
        if (covariance_sum < threshold)
        {
            ROS_INFO("Localization successful, covariance sum below 0.1. Terminating node.");
            ros::shutdown();
        }
    }
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "tiago_localization_node");
    tiago_localization::Localizer localizer;

    ros::NodeHandle nh;
    if (!localizer.initialize(nh))
    {
        ROS_ERROR_STREAM("Tiago_localizer::Localizer failed to initialize");
        return -1;
    }
    ROS_INFO("Publishing robot vel cmd.");
    localizer.robot_spin(localizer.speed);
    ros::spin();
    return 0;
}
