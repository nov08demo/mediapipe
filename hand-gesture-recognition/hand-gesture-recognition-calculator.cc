#include <cmath>
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/rect.pb.h"
#include <iostream>
#include <Python/Python.h> 

#include <string>
#include <fstream>
#include <streambuf>
#include <thread>         // std::this_thread::sleep_for
#include <chrono> 
#include <unistd.h>
#include <fstream>



namespace mediapipe
{

namespace
{
constexpr char normRectTag[] = "NORM_RECT";
constexpr char normalizedLandmarkListTag[] = "NORM_LANDMARKS";
constexpr char recognizedHandGestureTag[] = "RECOGNIZED_HAND_GESTURE";
} // namespace

// Graph config:
//
// node {
//   calculator: "HandGestureRecognitionCalculator"
//   input_stream: "NORM_LANDMARKS:scaled_landmarks"
//   input_stream: "NORM_RECT:hand_rect_for_next_frame"
// }

void pyRun(){
    char filename[] = "socketConnection.py";
	FILE* fp;

	Py_Initialize();

    // std::ifstream t("file.txt");
    // std::stringstream buffer;
    // buffer << t.rdbuf();
    // char fileContent[] = buffer.str();
    // LOG(INFO) << fileContent;
	// fp = fopen(filename, "r");
	// PyRun_SimpleFile(fp, filename);
    PyRun_SimpleString("import socketio");
    PyRun_SimpleString("import requests");
    PyRun_SimpleString("import time");

    PyRun_SimpleString("sio = socketio.Client()");
    PyRun_SimpleString("sio.connect('http://summerdevelopment-env-dev008.us-east-1.elasticbeanstalk.com/')");
    PyRun_SimpleString("@sio.on('connect')\n"
                        "def on_connect():\n\t"
                        "print('connected to the brain')\n");
    PyRun_SimpleString("sio.emit('edge.startEdge')");
    PyRun_SimpleString("time.sleep(0.2)");
    PyRun_SimpleString("sio.disconnect()");
	Py_Finalize();
    
}

class HandGestureRecognitionCalculator : public CalculatorBase
{
public:
    static ::mediapipe::Status GetContract(CalculatorContract *cc);
    ::mediapipe::Status Open(CalculatorContext *cc) override;

    ::mediapipe::Status Process(CalculatorContext *cc) override;

private:
    float get_Euclidean_DistanceAB(float a_x, float a_y, float b_x, float b_y)
    {
        float dist = std::pow(a_x - b_x, 2) + pow(a_y - b_y, 2);
        return std::sqrt(dist);
    }

    bool isThumbNearFirstFinger(NormalizedLandmark point1, NormalizedLandmark point2)
    {
        float distance = this->get_Euclidean_DistanceAB(point1.x(), point1.y(), point2.x(), point2.y());
        return distance < 0.1;
    }
};

REGISTER_CALCULATOR(HandGestureRecognitionCalculator);

::mediapipe::Status HandGestureRecognitionCalculator::GetContract(
    CalculatorContract *cc)
{
    RET_CHECK(cc->Inputs().HasTag(normalizedLandmarkListTag));
    cc->Inputs().Tag(normalizedLandmarkListTag).Set<mediapipe::NormalizedLandmarkList>();

    RET_CHECK(cc->Inputs().HasTag(normRectTag));
    cc->Inputs().Tag(normRectTag).Set<NormalizedRect>();

    RET_CHECK(cc->Outputs().HasTag(recognizedHandGestureTag));
    cc->Outputs().Tag(recognizedHandGestureTag).Set<std::string>();

    return ::mediapipe::OkStatus();
}

::mediapipe::Status HandGestureRecognitionCalculator::Open(
    CalculatorContext *cc)
{
    cc->SetOffset(TimestampDiff(0));
    return ::mediapipe::OkStatus();
}

::mediapipe::Status HandGestureRecognitionCalculator::Process(
    CalculatorContext *cc)
{
    std::string *recognized_hand_gesture;

    // hand closed (red) rectangle
    const auto rect = &(cc->Inputs().Tag(normRectTag).Get<NormalizedRect>());
    float width = rect->width();
    float height = rect->height();
    float minBoxIndex = 0.0225;
    float boxArea = width * height;

    if (width < 0.01 || height < 0.01)
    {
        // LOG(INFO) << "No Hand Detected";
        recognized_hand_gesture = new std::string("___");
        cc->Outputs()
            .Tag(recognizedHandGestureTag)
            .Add(recognized_hand_gesture, cc->InputTimestamp());
        return ::mediapipe::OkStatus();
    }

    const auto &landmarkList = cc->Inputs()
                                   .Tag(normalizedLandmarkListTag)
                                   .Get<mediapipe::NormalizedLandmarkList>();
    RET_CHECK_GT(landmarkList.landmark_size(), 0) << "Input landmark vector is empty.";

    // finger states
    bool thumbIsOpen = false;
    bool firstFingerIsOpen = false;
    bool secondFingerIsOpen = false;
    bool thirdFingerIsOpen = false;
    bool fourthFingerIsOpen = false;
    //
    //hand size (box area) 
    bool handInRange = false;

    float pseudoFixKeyPoint = landmarkList.landmark(2).x();
    if (landmarkList.landmark(3).x() < pseudoFixKeyPoint && landmarkList.landmark(4).x() < pseudoFixKeyPoint)
    {
        thumbIsOpen = true;
    }

    pseudoFixKeyPoint = landmarkList.landmark(6).y();
    if (landmarkList.landmark(7).y() < pseudoFixKeyPoint && landmarkList.landmark(8).y() < pseudoFixKeyPoint)
    {
        firstFingerIsOpen = true;
    }

    pseudoFixKeyPoint = landmarkList.landmark(10).y();
    if (landmarkList.landmark(11).y() < pseudoFixKeyPoint && landmarkList.landmark(12).y() < pseudoFixKeyPoint)
    {
        secondFingerIsOpen = true;
    }

    pseudoFixKeyPoint = landmarkList.landmark(14).y();
    if (landmarkList.landmark(15).y() < pseudoFixKeyPoint && landmarkList.landmark(16).y() < pseudoFixKeyPoint)
    {
        thirdFingerIsOpen = true;
    }

    pseudoFixKeyPoint = landmarkList.landmark(18).y();
    if (landmarkList.landmark(19).y() < pseudoFixKeyPoint && landmarkList.landmark(20).y() < pseudoFixKeyPoint)
    {
        fourthFingerIsOpen = true;
    }
    if(boxArea > minBoxIndex) {
        handInRange = true;
    }
    std::ofstream myfile;
    // Hand gesture recognition
    if (thumbIsOpen && firstFingerIsOpen && secondFingerIsOpen && thirdFingerIsOpen && fourthFingerIsOpen && handInRange)
    {
        recognized_hand_gesture = new std::string("FIVE");
        LOG(INFO) << "FIVE";
        
        myfile.open ("example.csv");
        myfile << "five";
        myfile.close();
        
        //usleep(0.2);
        }
    else if (!thumbIsOpen && firstFingerIsOpen && secondFingerIsOpen && thirdFingerIsOpen && fourthFingerIsOpen && handInRange)
    {
        recognized_hand_gesture = new std::string("FOUR");
        LOG(INFO) << "FOUR";
        myfile.open ("example.csv");
        myfile << "four";
        myfile.close();
    }
    else if (thumbIsOpen && firstFingerIsOpen && secondFingerIsOpen && !thirdFingerIsOpen && !fourthFingerIsOpen && handInRange)
    {
        recognized_hand_gesture = new std::string("THREE");
        LOG(INFO) << "THREE";
        myfile.open ("example.csv");
        myfile << "three";
        myfile.close();
    }
    else if (thumbIsOpen && firstFingerIsOpen && !secondFingerIsOpen && !thirdFingerIsOpen && !fourthFingerIsOpen && handInRange)
    {
        recognized_hand_gesture = new std::string("TWO");
        LOG(INFO) << "TWO";
        myfile.open ("example.csv");
        myfile << "two";
        myfile.close();
    }
    else if (!thumbIsOpen && firstFingerIsOpen && !secondFingerIsOpen && !thirdFingerIsOpen && !fourthFingerIsOpen && handInRange)
    {
        recognized_hand_gesture = new std::string("ONE");
        LOG(INFO) << "ONE";
        myfile.open ("example.csv");
        myfile << "one";
        myfile.close();
    }
    else if (!thumbIsOpen && firstFingerIsOpen && secondFingerIsOpen && !thirdFingerIsOpen && !fourthFingerIsOpen && handInRange)
    {
        recognized_hand_gesture = new std::string("YEAH");
        LOG(INFO) << "YEAH";
        myfile.open ("example.csv");
        myfile << "yeah";
        myfile.close();
    }
    else if (!thumbIsOpen && firstFingerIsOpen && !secondFingerIsOpen && !thirdFingerIsOpen && fourthFingerIsOpen && handInRange)
    {
        recognized_hand_gesture = new std::string("ROCK");
        LOG(INFO) << "ROCK";
        myfile.open ("example.csv");
        myfile << "rock";
        myfile.close();
    }
    else if (thumbIsOpen && firstFingerIsOpen && !secondFingerIsOpen && !thirdFingerIsOpen && fourthFingerIsOpen && handInRange)
    {
        recognized_hand_gesture = new std::string("SPIDERMAN");
        LOG(INFO) << "SPIDERMAN";
        myfile.open ("example.csv");
        myfile << "spiderman";
        myfile.close();
    }
    else if (!thumbIsOpen && !firstFingerIsOpen && !secondFingerIsOpen && !thirdFingerIsOpen && !fourthFingerIsOpen && handInRange)
    {
        recognized_hand_gesture = new std::string("FIST");
        LOG(INFO) << "FIST";
        myfile.open ("example.csv");
        myfile << "fist";
        myfile.close();
    }
    else if (!firstFingerIsOpen && secondFingerIsOpen && thirdFingerIsOpen && fourthFingerIsOpen && handInRange && this->isThumbNearFirstFinger(landmarkList.landmark(4), landmarkList.landmark(8)))
    {
        recognized_hand_gesture = new std::string("OK");
        LOG(INFO) << "OK";
        myfile.open ("example.csv");
        myfile << "ok";
        myfile.close();
    }
    else
    {
        recognized_hand_gesture = new std::string("___");
        LOG(INFO) << "Finger States: " << thumbIsOpen << firstFingerIsOpen << secondFingerIsOpen << thirdFingerIsOpen << fourthFingerIsOpen; 
        myfile.open ("example.csv");
        myfile << "random sequence";
        myfile.close();      
    }
    
    // LOG(INFO) << recognized_hand_gesture;

    cc->Outputs()
        .Tag(recognizedHandGestureTag)
        .Add(recognized_hand_gesture, cc->InputTimestamp());

    return ::mediapipe::OkStatus();
} // namespace mediapipe

} // namespace mediapipe