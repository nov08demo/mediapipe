# Gesture Recognition On Desktop
Running the following commands from the terminal will start the gesture recognition script:
### Build the project using:
```
bazel build -c opt --define MEDIAPIPE_DISABLE_GPU=1 \
mediapipe/examples/desktop/hand_tracking:hand_tracking_cpu
```
### Run the project using:
```
GLOG_logtostderr=1 bazel-bin/mediapipe/examples/desktop/hand_tracking/hand_tracking_cpu \
    --calculator_graph_config_file=mediapipe/graphs/hand_tracking/hand_tracking_desktop_live.pbtxt
```