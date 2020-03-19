import socketio
import requests
import time
import csv


sio = socketio.Client()

@sio.on('connect')
def on_connect():
    print('connected to the brain')

#sio.connect('http://summerdevelopment-env-dev008.us-east-1.elasticbeanstalk.com/')

sio.connect('http://summer-dev.us-east-1.elasticbeanstalk.com')



 
handHover = False
fist = False

while True:

    f = open("../example.csv", "r")

    if(f.read() =="five"):
        f.close
        sio.emit("edge.hover")
        print("--hover--")
        handHover=True
        f = open("../example.csv", "w")
        f.truncate()
        f.close()
        time.sleep(1)
    elif(f.read() =="one"):
        f.close
        sio.emit("edge.oneGesture")
        print("--ONE--")
        f = open("../example.csv", "w")
        f.truncate()
        f.close()
        time.sleep(1)
    elif(f.read() =="two"):
        f.close
        sio.emit("edge.twoGesture")
        print("--TWO--")
        f = open("../example.csv", "w")
        f.truncate()
        f.close()
        time.sleep(1)
    elif(f.read() =="three"):
        f.close
        sio.emit("edge.threeGesture")
        print("--THREE--")
        f = open("../example.csv", "w")
        f.truncate()
        f.close()
        time.sleep(1)
    elif(f.read() =="fist"):
        f.close
        sio.emit("edge.hover")
        print("--FIST--")
        fist = True
        f = open("../example.csv", "w")
        f.truncate()
        f.close()
        time.sleep(1)
    else:
        if (handHover or fist):
            time.sleep(1)
            sio.emit("edge.handleave")
            handHover = False
            fist = False
