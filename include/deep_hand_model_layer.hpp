#ifndef CAFFE_CUSTOM_LAYERS_HPP_
#define CAFFE_CUSTOM_LAYERS_HPP_

#include <string>
#include <utility>
#include <vector>

#include "caffe/blob.hpp"
#include "caffe/common.hpp"
#include "caffe/layer.hpp"
#include "caffe/proto/caffe.pb.h"
#include "caffe/layers/loss_layer.hpp"
#include "caffe/numeric/Matrix.h"
#include "caffe/numeric/Vector.h"
#include "caffe/HandModel/HandDefine.h"
using namespace numeric;
namespace caffe 
{
	
	template <typename Dtype>
	class DeepHandModelDofConstraintLossLayer : public LossLayer<Dtype> 
	{
	  public:
		explicit DeepHandModelDofConstraintLossLayer(const LayerParameter& param)
		  : LossLayer<Dtype>(param) {}
		virtual void LayerSetUp(const vector<Blob<Dtype>*>& bottom,
		  const vector<Blob<Dtype>*>& top);
		virtual void Reshape(const vector<Blob<Dtype>*>& bottom,
		  const vector<Blob<Dtype>*>& top);
		virtual inline const char* type() const { return "DeepHandModelDofConstraintLoss"; }
		virtual inline int ExactNumBottomBlobs() const { return 1; }
		virtual inline int ExactNumTopBlobs() const { return 1; }

	  protected:
		virtual void Forward_cpu(const vector<Blob<Dtype>*>& bottom,
		  const vector<Blob<Dtype>*>& top);	  
		virtual void Backward_cpu(const vector<Blob<Dtype>*>& top,
		  const vector<bool>& propagate_down, const vector<Blob<Dtype>*>& bottom);	 
		
		const float PI = 3.1415926535897932384626;  
		float DoFConstraintLowerBound[ParamNum];
		float DoFConstraintUpperBound[ParamNum];
		int isConstrained[ParamNum]; //whether to take account into the objective function
	};

	template <typename Dtype>
	class DeepHandModelLayer : public Layer<Dtype> 
	{
	  public:
		explicit DeepHandModelLayer(const LayerParameter& param)
			: Layer<Dtype>(param) {}
		virtual void LayerSetUp(const vector<Blob<Dtype>*>& bottom,
			const vector<Blob<Dtype>*>& top);
	
		virtual void Reshape(const vector<Blob<Dtype>*>& bottom,
			const vector<Blob<Dtype>*>& top);

		virtual inline const char* type() const { return "DeepHandModel"; }
		virtual inline int ExactNumBottomBlobs() const { return 1; }
		virtual inline int ExactNumTopBlobs() const { return 1; }
		

	  protected:
		virtual void Forward_cpu(const vector<Blob<Dtype>*>& bottom,
			const vector<Blob<Dtype>*>& top); 
		virtual void Backward_cpu(const vector<Blob<Dtype>*>& top,
			const vector<bool>& propagate_down, const vector<Blob<Dtype>*>& bottom);    
    
	  private:      
			

			//1. Related to parameter:
			int isFixed[ParamNum];
			double initparam[ParamNum]; //InitialRotationDegree
	
			//2. Related to shape parameters(bone length)
			double bonelen[BoneNum];

			//3. Related to transformation
			Matr const_matr[ConstMatrNum];
			std::vector<std::pair<matrix_operation, int> > Homo_mat[JointNum]; //Homogenous matrices (represent transformation for each joint)
			Matr prev_mat[JointNum];  //prev_mat * resttransformation

			//4. Related to joint locations
			Vec t_joint[JointNum];

			//5. Related to back propagated gradient
			Vec Jacobian[JointNum][ParamNum]; //partial derivative of joint with respect to parameter
			
			//6. Main functions
			Matr GetMatrix(matrix_operation opt, int bottom_id, int image_id, int param_id, bool is_gradient, const Dtype *bottom_data);			
			void Forward(Matr mat, int bottom_id, int image_id, int joint_id, int prev_size, const Dtype *bottom_data);
			void Backward(int bottom_id, int image_id, int joint_id, const Dtype *bottom_data);			
			void SetupConstantMatrices();
			void SetupTransformation();		
	  };
}  // namespace caffe

#endif  // CAFFE_COMMON_LAYERS_HPP_