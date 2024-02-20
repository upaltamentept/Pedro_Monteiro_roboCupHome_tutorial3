# Rover c++ project

This is a project that emulates a rover and it's movement, with obstacles included, autonomous and non-autonomously

To build this project, you will need the following:

 For the first exercise:

1 - Change directory to ros workspace (in my case "cd ros/workspaces/tutorial/")
2 - Type "source devel/setup.bash"
3 - "Catkin build" the project
4 - "roslaunch object_server object_server.launch"
5 - rosservice call /add_objects "number: 
  data: 4
seed:
  data: 2" 
6 - roslaunch rover rover_teleop.launch
7 - rosrun key_teleop key_teleop.py
 And then use arrow keys to control the rover

For the exercise number 3:

1 - Change directory to ros workspace (in my case "cd ros/workspaces/tutorial/")
2 - Type "source devel/setup.bash"
3 - "Catkin build" the project
4 - "roslaunch object_server object_server.launch"
5 - rosservice call /add_objects "number: 
  data: 4
seed:
  data: 2" 
6 - roslaunch rover_controller rover_controller.launch
7 - call "rosservice /add_obstacles" (I didn't have success doing this point as the when doing the "rosservice list", this command did not appear. However, the repulse function is made in the code, and theorically should work)
8 - In the Rviz, choose the option in the upper menu that says "2D Nav goal" and the rover will go to the place marked
 

