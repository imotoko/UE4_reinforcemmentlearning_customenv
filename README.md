代码来源https://github.com/theboywhomakesgames/unreal-reinforcement
原项目提供cpp，py文件
作为初学者 包装成unproject，测试刚学python机器学习，py和原来不同
-----------------
结构树py

|——customenv

    |——init
		
      |——[obs,act]space           //用于nn网络（in-out）搭建
			
    |——step
		
      |——tcp>>ue4

|——agent

    |——nn
	
    |——act(obs)
	
    |——step(act)
	
    |——train_episode()
	
      while
		
        act(obs)
			
        step(act)
			
        if done:break
      
|——原作者使用baseline里的算法（nn）

    baseline

-----------------
流程

cd polebalancing
重构建项目
![image](https://github.com/imotoko/UE4_reinforcemmentlearning_customenv/blob/master/image/step1.png)
进入游戏
![image](https://github.com/imotoko/UE4_reinforcemmentlearning_customenv/blob/master/image/step2.png)
运行dqn_model.py
