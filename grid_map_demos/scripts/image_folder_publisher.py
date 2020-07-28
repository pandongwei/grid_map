#!/usr/bin/env python
# simple script to publish a image from a file.
import rospy
import rospkg
import time
import cv2
import sensor_msgs.msg
import os



#Main function initializes node and subscribers and starts the ROS loop
def main_program():
    rospack = rospkg.RosPack()
    rospy.init_node('image_publisher')
    folder_path = rospy.get_param('~folder_path')
    path_save = rospy.get_param('~path_save')
    topicName = rospy.get_param('~topic')

    publisher = rospy.Publisher(topicName, sensor_msgs.msg.Image, queue_size=1000)
    rate = rospy.Rate(100)

    # folder_path = "/mrtstorage/users/pan/material_dataset_v1/depth/"
    # path_save = "/mrtstorage/users/pan/material_dataset_v1/label/"
    while not rospy.is_shutdown():
        for dir_1 in os.listdir(folder_path):
            for dir_2 in os.listdir(folder_path + dir_1):
                file_folder_path = folder_path + dir_1 + "/" + dir_2
                # os.makedirs(os.path.dirname(path_save + dir_1 + "/" + dir_2 + "/"))
                for root, dirs, files in os.walk(file_folder_path, topdown=True):
                    for file in files:
                        file_path = os.path.join(file_folder_path, file)
                        save_path = path_save + dir_1 + "/" + dir_2 + "/" + "traversabilityMap_" +file
                        img = cv2.imread(file_path, cv2.IMREAD_UNCHANGED)
                        # print(file_path)
                        rosimage = sensor_msgs.msg.Image()
                        if img.dtype.itemsize == 2:
                            if len(img.shape) == 3:
                                if img.shape[2] == 3:
                                    rosimage.encoding = 'bgr16'
                                if img.shape[2] == 4:
                                    rosimage.encoding = 'bgra16'
                            else:
                                rosimage.encoding = 'mono16'
                        if img.dtype.itemsize == 1:
                            if len(img.shape) == 3:
                                if img.shape[2] == 3:
                                    rosimage.encoding = 'bgr8'
                                if img.shape[2] == 4:
                                    rosimage.encoding = 'bgra8'
                            else:
                                rosimage.encoding = 'mono8'
                        #    print "Encoding: ", rosimage.encoding

                        rosimage.width = img.shape[1]
                        rosimage.height = img.shape[0]
                        rosimage.step = img.strides[0]
                        rosimage.data = img.tostring()
                        rosimage.header.stamp = rospy.Time.now()
                        # load the path of saving the result image
                        rosimage.header.frame_id = save_path

                        publisher.publish(rosimage)
                        # rate.sleep()
    # rospy.spin()

if __name__ == '__main__':
    try:
        main_program()
    except rospy.ROSInterruptException: pass
