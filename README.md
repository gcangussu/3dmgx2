# C library and ROS node for 3DM-GX2 IMU

This project is composed by two main components. First is a C library that can be used for comunicating with a wireless or wired 3DM-GX2 inertial measurement unit. This library can create and analise packets exchanged with the device's protocol. The other component is a ROS (Robot Operating System) node. This node publishes messages on the `/imu/raw_data` topic with the ROS standard message for IMUs ([`sensor_msgs/IMU`](http://docs.ros.org/api/sensor_msgs/html/msg/Imu.html)).

To compile the ROS node, use this repository as a ROS packet inside a catkin workspace, such as:
```
my_workspace/
├── src/
│   ├── CMakeLists.txt
│   └── i3dmgx2/
│       ├── src/
│       ├── doc/
│       ├── CMakeLists.txt
│       └── package.xml
├── build/
├── devel/
└── install/
```

Then go to your workspace root and build it:
```
cd /path/to/my_workspace
catkin_make
```

To use the ROS node, first source your workspace setup file and then use `rosrun` to initialize the node:
```
source devel/setup.bash
rosrun i3dmgx2 i3dmgx2_node /dev/ttyUSB0 95
```

In the previous example the node will try to connect to the basestation on the serial port `/dev/ttyUSB0` and will comunicate with the IMU with address 95. The arguments accepted by the node are the following:
```
rosrun i3dmgx2 i3dmgx2_node PORT ADDRESS [DIVIDER]
```

Where `PORT` is the serial port wich the IMU's basestation is connected, be sure to have user permissions to read and write on the used port; `ADDRESS` is the IMU assigned address, must be a positive integer up to 65535; and `DIVIDER` is an optional argument to write a new divider constant on the IMU memory, must be a positive integer in the range [170 51200]. (see page 29 of "doc/3DM-GX2 Data Communications Protocol.pdf" for more details)
