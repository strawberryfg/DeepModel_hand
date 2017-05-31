# Model-based Deep Hand Pose Estimation
This repository is the modified version of the released code of our IJCAI 2016 [paper](http://xingyizhou.xyz/zhou2016model.pdf) for estimating hand pose from depth image.

(With source file and include file reconstructed and comments added. Note that the hand model layer has exactly the same semantic meaning as https://github.com/xingyizhou/DeepModel.

All the experiments have been conducted using this version of code and the results are the same as https://github.com/xingyizhou/DeepModel)

Please feel free to contact Qingfu Wan at strawberryfgalois@gmail.com or wanqingfustrawberryfg0818@alu.fudan.edu.cn.

Your kind suggestions on the code or anything else are always welcomed!

## Include
- HandDefine.h: With explanations of joint, bone, DoF, forward sequence of forward kinematics process
- deep_hand_model_layer.hpp

## Src
- deep_hand_model_layer.cpp: Hand Model Layer with more efficient realization
- deep_hand_model_dof_constraint_loss_layer: Physical Constraint Loss Layer

## Common
- Include files of matrix and vector operations

## Installation & Test & Train
- Please refer to https://github.com/xingyizhou/DeepModel for more details

## Citation

Please cite DeepModel in your publication if it helps your research:

    @inproceedings{zhou2016model,
        author = {Xingyi Zhou and Qingfu Wan and Wei Zhang and Xiangyang Xue and Yichen Wei},
        booktitle = {IJCAI},
        title = {Model-based Deep Hand Pose Estimation},
        year = {2016}
    }