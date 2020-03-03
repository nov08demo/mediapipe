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



 
handHover= False

while True:
    # with open('../example.csv', newline='') as csvfile:
    #     spamreader = csv.reader(csvfile, delimiter=' ', quotechar='|')
    #     for row in spamreader:
    #         if(row[0] =="five"):
    #             sio.emit("edge.hover")
    #             print(row)
    #             handHover=True
    #             f = open("../example.csv", "w")
    #             f.truncate()
    #             f.close()
    #             time.sleep(0.5)
    #         else:
    #             print(row)
    #             if handHover:
    #                 time.sleep(0.5)
    #                 sio.emit("edge.handleave")
    #                 handHover=False

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
    else:
        if handHover:
            time.sleep(1)
            sio.emit("edge.handleave")
            handHover=False
