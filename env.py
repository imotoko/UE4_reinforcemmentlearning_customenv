import gym
import socket
import json
import numpy as np

HOST = '127.0.0.1'
PORT = 7794

class custom_env(gym.Env):
    def __init__(self):
        self.state = None

        '''
num     high     low       name
 0      180      -180      angle
 1      200      -200      location
 done   angle==50,location==100
                
'''
        high = np.array(
            [
                400,
                180,
            ],
            dtype=np.float32,
        )
        
        self.action_space = gym.spaces.Discrete(2)
        self.observation_space = gym.spaces.Box(-high, high, dtype=np.float32)

        self.hasConnection = False
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        try:
            self.socket.connect((HOST, PORT))
            print("successfully connected")
            self.hasConnection = True
        except socket.error:
            print("couldn't connect" + socket.error)

    def step(self, action):                         #->action = 0,1
        
        action += 1                                 #->axis矫正 1,2 ->server(-1,1)
        
        self.socket.send(bytes([action]))           #tcp
                                                    
        # wait till you get a response
        while(True):
            data = self.socket.recv(4096)

            
            if not data:
                print("nodata")
                break

            #json处理
            data = json.loads(data.decode())
            angle = data.get("angle")               #angle
            location = data.get("location")         #loaction
            break
    
        done = bool(
        abs(angle) > 50
        or abs(location) > 200
        )
        
        '''
两种奖励
    @角度
    @存活时间

'''
        reward = 1 - abs(angle)/20                  
        #if not done:
        #    reward = 1
        #else:
        #    reward = 0


        self.state = (angle,location)
        info = {}
        return np.array(self.state), reward, done, info

    def reset(self):
        while True:
            self.socket.send(bytes([10]))
            
            data = self.socket.recv(4096)
            if data:
                break
     
        angle = 0
        location = 0
        self.state =(angle,location)
        
        return np.array(self.state)
