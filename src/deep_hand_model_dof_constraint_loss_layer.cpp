#include <vector>

#include "caffe/layer.hpp"
#include "caffe/util/io.hpp"
#include "caffe/util/math_functions.hpp"
#include "caffe/HandModel/deep_hand_model_layer.hpp"

namespace caffe {

	template <typename Dtype>
	void DeepHandModelDofConstraintLossLayer<Dtype>::LayerSetUp(
	  const vector<Blob<Dtype>*>& bottom, const vector<Blob<Dtype>*>& top) 
	{
	  if (this->layer_param_.loss_weight_size() == 0) {
		this->layer_param_.add_loss_weight(Dtype(1));
	  }
	
	  FILE *fin = fopen("configuration/DofConstraintLowerBound.in", "r");
	  for (int i = 0; i < ParamNum; i++)
	  {
		  fscanf(fin, "%f", &DoFConstraintLowerBound[i]);
	  }
	  fclose(fin);

	  fin = fopen("configuration/DofConstraintUpperBound.in", "r");
	  for (int i = 0; i < ParamNum; i++)
	  {
		  fscanf(fin, "%f", &DoFConstraintUpperBound[i]);
	  }
	  fclose(fin);

	  fin = fopen("configuration/DofConstraintId.in", "r");
	  for (int i = 0; i < ParamNum; i++) isConstrained[i] = 0;
	  int n;
	  fscanf(fin, "%d", &n);
	  for (int i = 0; i < n; i++)
	  {
		  int id;
		  fscanf(fin, "%d", &id);
		  isConstrained[id] = 1;
	  }
	  fclose(fin);
	}


	template <typename Dtype>
	void DeepHandModelDofConstraintLossLayer<Dtype>::Reshape(
	  const vector<Blob<Dtype>*>& bottom, const vector<Blob<Dtype>*>& top) 
	{
	  vector<int> loss_shape(0);
	  top[0]->Reshape(loss_shape);
	}

	template <typename Dtype>
	void DeepHandModelDofConstraintLossLayer<Dtype>::Forward_cpu(const vector<Blob<Dtype>*>& bottom,
		const vector<Blob<Dtype>*>& top) 
	{
	  int batSize = (bottom[0]->shape())[0];
	  const Dtype* bottom_data = bottom[0]->cpu_data();
	  Dtype loss = 0;
	  for (int t = 0; t < batSize; t++) 
	  {
		int bottom_id = t * ParamNum;
		for (int i = 0; i < ParamNum; i++)
		{
			if (isConstrained[i]) continue;
			if (bottom_data[bottom_id + i] > DoFConstraintUpperBound[i]) loss += (bottom_data[bottom_id + i] - DoFConstraintUpperBound[i]) * (bottom_data[bottom_id + i] - DoFConstraintUpperBound[i]);
			else if (bottom_data[bottom_id + i] < DoFConstraintLowerBound[i]) loss += (bottom_data[bottom_id + i] - DoFConstraintLowerBound[i]) * (bottom_data[bottom_id + i] - DoFConstraintLowerBound[i]);
		}      
	  }
	  top[0]->mutable_cpu_data()[0] = loss / batSize;
	}

	template <typename Dtype>
	void DeepHandModelDofConstraintLossLayer<Dtype>::Backward_cpu(const vector<Blob<Dtype>*>& top,
		const vector<bool>& propagate_down, const vector<Blob<Dtype>*>& bottom) 
	{
	  int batSize = (bottom[0]->shape())[0];
	  const Dtype* bottom_data = bottom[0]->cpu_data();
	  float top_diff = top[0]->cpu_diff()[0] / batSize;
	  Dtype* bottom_diff = bottom[0]->mutable_cpu_diff();
  
	  if (propagate_down[0]) {
		for (int t = 0; t < batSize; t++) {
		  int bottom_id = t * ParamNum;
		  for (int i = 0; i < ParamNum; i++)
		  {
			  if (isConstrained[i]) continue;
			  if (bottom_data[bottom_id + i] > DoFConstraintUpperBound[i]) bottom_diff[bottom_id + i] = top_diff * 2 * (bottom_data[bottom_id + i] - DoFConstraintUpperBound[i]);
			  else if (bottom_data[bottom_id + i] < DoFConstraintLowerBound[i]) bottom_diff[bottom_id + i] = top_diff * 2 * (bottom_data[bottom_id + i] - DoFConstraintLowerBound[i]);
			  else bottom_diff[bottom_id + i] = 0;
		  }        
		}
	  }
	}

	#ifdef CPU_ONLY
	STUB_GPU(DeepHandModelDofConstraintLossLayer);
	#endif

	INSTANTIATE_CLASS(DeepHandModelDofConstraintLossLayer);
	REGISTER_LAYER_CLASS(DeepHandModelDofConstraintLoss);

	}  // namespace caffe