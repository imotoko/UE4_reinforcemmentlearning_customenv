import env

import numpy as np
import torch

class DQNAgent(object):

    def __init__(self, q_func, optimizer, n_act, gamma=0.9, e_greed=0.1):
        self.q_func = q_func #Q函数

        self.optimizer = optimizer #优化器
        self.criterion = torch.nn.MSELoss() #损失函数

        self.n_act = n_act  # 动作数量
        self.gamma = gamma  # 收益衰减率
        self.epsilon = e_greed  # 探索与利用中的探索概率

    # 根据经验得到action
    def predict(self, obs):
        Q_list = self.q_func(obs)
        action = int(torch.argmax(Q_list).detach().numpy())
        return action

    # 根据探索与利用得到action
    def act(self, obs):
        if np.random.uniform(0, 1) < self.epsilon:  #探索
            action = np.random.choice(self.n_act)
        else: # 利用
            action = self.predict(obs)
        return action

    # 更新Q表格
    def learn(self, obs, action, reward, next_obs, done):
        # predict_Q
        pred_Vs = self.q_func(obs)
        predict_Q = pred_Vs[action]

        #target_Q
        next_pred_Vs = self.q_func(next_obs)
        best_V = next_pred_Vs.max()
        target_Q = reward + (1 - float(done))*self.gamma * best_V

        # 更新参数
        self.optimizer.zero_grad()
        loss = self.criterion(predict_Q,target_Q)
        loss.backward()
        self.optimizer.step()

'''
DQN算法
'''
class MLP(torch.nn.Module):

    def __init__(self, obs_size,n_act):
        super().__init__()
        self.mlp = self.__mlp(obs_size,n_act)

    def __mlp(self,obs_size,n_act):
        return torch.nn.Sequential(
            torch.nn.Linear(obs_size, 50),
            torch.nn.ReLU(),
            torch.nn.Linear(50, 50),
            torch.nn.ReLU(),
            torch.nn.Linear(50, n_act)
        )

    def forward(self, x):
        return self.mlp(x)
'''
moudle神经网路
'''
class TrainManager():

    def __init__(self,env,episodes=1000,lr=0.001,gamma=0.9,e_greed=0.1):
        self.env = env
        n_act = env.action_space.n             #.n/.shape()
        n_obs = env.observation_space.shape[0] #.n/.shape[0]/
        #print(n_obs)                          
        q_func = MLP(n_obs, n_act)
        print(q_func)
        optimizer = torch.optim.AdamW(q_func.parameters(), lr=lr)
        self.agent = DQNAgent(
            q_func=q_func,
            optimizer=optimizer,
            n_act=n_act,
            gamma=gamma,
            e_greed=e_greed)
        self.episodes = episodes

    # 训练一轮游戏
    def train_episode(self):
        total_reward = 0        
        obs = self.env.reset()
        
        obs = torch.FloatTensor(obs)
        while True:
            action = self.agent.act(obs)

            next_obs, reward, done, info = self.env.step(action)   
            next_obs = torch.FloatTensor(next_obs)

            self.agent.learn(obs, action, reward, next_obs, done)

            obs = next_obs
            total_reward += reward
            if done: break
        
        return total_reward

if __name__ == '__main__':
    env1 = env.custom_env()
    tm = TrainManager(env1)
        
    while(True):
        reward = tm.train_episode()
        print("this round reward =",reward) 
