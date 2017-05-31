#include <algorithm>
#include "caffe/layer.hpp"
#include "caffe/HandModel/deep_hand_model_layer.hpp"

namespace caffe 
{
	template <typename Dtype>
	void DeepHandModelLayer<Dtype>::SetupConstantMatrices()
	{	
		//finger 5: thumb
		const_matr[wrist_left] = Matr(trans_y, -bonelen[bone_palm_center_connect_wrist_left], false);		
		const_matr[wrist_middle] = Matr(trans_y, -bonelen[bone_palm_center_connect_wrist_middle], false);	
		const_matr[thumb_mcp] = Matr(trans_y, -bonelen[bone_palm_center_connect_thumb_mcp], false);
		const_matr[thumb_pip] = Matr(trans_x, bonelen[bone_thumb_mcp_connect_pip], false);		
		const_matr[thumb_dip] = Matr(trans_x, bonelen[bone_thumb_pip_connect_dip], false);
		const_matr[thumb_tip] = Matr(trans_x, bonelen[bone_thumb_dip_connect_tip], false);
		for (int k = 0; k < 4; k++) //finger 1 - finger 4 (little, ring, middle, index)
		{
			const_matr[finger_mcp_start + k] = Matr(trans_y, bonelen[bone_finger_mcp_connect_palm_center_start + k], false);
			const_matr[finger_base_start + EachFingerBoneNum * k] = Matr(trans_y, bonelen[bone_finger_base_connect_finger_mcp_start + EachFingerBoneNum * k], false);
			const_matr[finger_pip_first_start + EachFingerBoneNum * k] = Matr(trans_y, bonelen[bone_finger_pip_first_connect_finger_base_start + EachFingerBoneNum * k], false);
			const_matr[finger_pip_second_start + EachFingerBoneNum * k] = Matr(trans_y, bonelen[bone_finger_pip_second_connect_pip_first_start + EachFingerBoneNum * k], false);
			//Actually there are two points for DIP in each finger (NYU dataset) and two points for TIP in each finger(but here we only use 1 for DIP and TIP each)
			const_matr[finger_dip_start + EachFingerBoneNum * k] = Matr(trans_y, bonelen[bone_finger_dip_connect_pip_second_start + EachFingerBoneNum * k], false);
			const_matr[finger_tip_start + EachFingerBoneNum * k] = Matr(trans_y, bonelen[bone_finger_tip_connect_dip_start + EachFingerBoneNum * k], false);		
		}		
	}

	template <typename Dtype>
	void DeepHandModelLayer<Dtype>::SetupTransformation()
	{
		//palm center
		Homo_mat[palm_center].pb(mp(trans_x, global_trans_x));
		Homo_mat[palm_center].pb(mp(trans_y, global_trans_y));
		Homo_mat[palm_center].pb(mp(trans_z, global_trans_z));
		Homo_mat[palm_center].pb(mp(rot_z, global_rot_z));
		Homo_mat[palm_center].pb(mp(rot_x, global_rot_x));
		Homo_mat[palm_center].pb(mp(rot_y, global_rot_y));		
		//wrist left
		for (int i = 0; i < Homo_mat[palm_center].size(); i++)
			Homo_mat[wrist_left].pb(Homo_mat[palm_center][i]);
	
		Homo_mat[wrist_left].pb(mp(rot_z, wrist_left_const_rot_z));
		Homo_mat[wrist_left].pb(mp(rot_x, wrist_left_const_rot_x));
		Homo_mat[wrist_left].pb(mp(rot_y, wrist_left_const_rot_y));
		Homo_mat[wrist_left].pb(mp(Const_Matr, wrist_left));
		//wrist middle(carpals)
		for (int i = 0; i < Homo_mat[palm_center].size(); i++)
			Homo_mat[wrist_middle].pb(Homo_mat[palm_center][i]);
	
		Homo_mat[wrist_middle].pb(mp(rot_z, wrist_middle_const_rot_z));
		Homo_mat[wrist_middle].pb(mp(rot_x, wrist_middle_const_rot_x));
		Homo_mat[wrist_middle].pb(mp(rot_y, wrist_middle_const_rot_y));
		Homo_mat[wrist_middle].pb(mp(Const_Matr, wrist_middle));		
		//thumb MCP (wrist right metacarpals)
		for (int i = 0; i < Homo_mat[palm_center].size(); i++)
			Homo_mat[thumb_mcp].pb(Homo_mat[palm_center][i]);

		Homo_mat[thumb_mcp].pb(mp(rot_z, thumb_mcp_const_rot_z));
		Homo_mat[thumb_mcp].pb(mp(rot_x, thumb_mcp_const_rot_x));
		Homo_mat[thumb_mcp].pb(mp(rot_y, thumb_mcp_const_rot_y));
		Homo_mat[thumb_mcp].pb(mp(Const_Matr, thumb_mcp));		
		//thumb PIP
		for (int i = 0; i < Homo_mat[thumb_mcp].size(); i++)
			Homo_mat[thumb_pip].pb(Homo_mat[thumb_mcp][i]);
		Homo_mat[thumb_pip].pb(mp(rot_z, thumb_pip_rot_z));
		Homo_mat[thumb_pip].pb(mp(rot_y, thumb_pip_rot_y));
		Homo_mat[thumb_pip].pb(mp(Const_Matr, thumb_pip));		
		//thumb DIP
		for (int i = 0; i < Homo_mat[thumb_pip].size(); i++)
			Homo_mat[thumb_dip].pb(Homo_mat[thumb_pip][i]);
		Homo_mat[thumb_dip].pb(mp(rot_z, thumb_dip_rot_z));
		Homo_mat[thumb_dip].pb(mp(Const_Matr, thumb_dip));		
		//thumb TIP
		for (int i = 0; i < Homo_mat[thumb_dip].size(); i++)
			Homo_mat[thumb_tip].pb(Homo_mat[thumb_dip][i]);
		Homo_mat[thumb_tip].pb(mp(rot_z, thumb_tip_rot_z));
		Homo_mat[thumb_tip].pb(mp(Const_Matr, thumb_tip));		
		//Finger 1-4
		for (int k = 0; k < 4; k++)
		{
			//finger mcp
			for (int i = 0; i < Homo_mat[palm_center].size(); i++)
				Homo_mat[finger_mcp_start + k].pb(Homo_mat[palm_center][i]);
		
			Homo_mat[finger_mcp_start + k].pb(mp(rot_z, finger_mcp_rot_z_start + EachMCPDoFNum * k));
			Homo_mat[finger_mcp_start + k].pb(mp(rot_x, finger_mcp_rot_x_start + EachMCPDoFNum * k));
			Homo_mat[finger_mcp_start + k].pb(mp(rot_y, finger_mcp_rot_y_start + EachMCPDoFNum * k));
			Homo_mat[finger_mcp_start + k].pb(mp(Const_Matr, finger_mcp_start + k));			
			//finger base
			for (int i = 0; i < Homo_mat[finger_mcp_start + k].size(); i++)
				Homo_mat[finger_base_start + EachFingerBoneNum * k].pb(Homo_mat[finger_mcp_start + k][i]);
			Homo_mat[finger_base_start + EachFingerBoneNum * k].pb(mp(rot_z, finger_base_rot_z_start + EachFingerDoFNum * k));
			Homo_mat[finger_base_start + EachFingerBoneNum * k].pb(mp(rot_x, finger_base_rot_x_start + EachFingerDoFNum * k));
			Homo_mat[finger_base_start + EachFingerBoneNum * k].pb(mp(Const_Matr, finger_base_start + EachFingerBoneNum * k));			
			//finger pip first
			for (int i = 0; i < Homo_mat[finger_base_start + EachFingerBoneNum * k].size(); i++)
				Homo_mat[finger_pip_first_start + EachFingerBoneNum * k].pb(Homo_mat[finger_base_start + EachFingerBoneNum * k][i]);
			Homo_mat[finger_pip_first_start + EachFingerBoneNum * k].pb(mp(rot_x, finger_pip_rot_x_start + EachFingerDoFNum * k));
			Homo_mat[finger_pip_first_start + EachFingerBoneNum * k].pb(mp(Const_Matr, finger_pip_first_start + EachFingerBoneNum * k));
			//finger pip second
			for (int i = 0; i < Homo_mat[finger_pip_first_start + EachFingerBoneNum * k].size(); i++)
				Homo_mat[finger_pip_second_start + EachFingerBoneNum * k].pb(Homo_mat[finger_pip_first_start + EachFingerBoneNum * k][i]);
			Homo_mat[finger_pip_second_start + EachFingerBoneNum * k].pb(mp(Const_Matr, finger_pip_second_start + EachFingerBoneNum * k));
			//finger dip
			for (int i = 0; i < Homo_mat[finger_pip_second_start + EachFingerBoneNum * k].size(); i++)
				Homo_mat[finger_dip_start + EachFingerBoneNum * k].pb(Homo_mat[finger_pip_second_start + EachFingerBoneNum * k][i]);
			Homo_mat[finger_dip_start + EachFingerBoneNum * k].pb(mp(rot_x, finger_dip_rot_x_start + EachFingerDoFNum * k));
			Homo_mat[finger_dip_start + EachFingerBoneNum * k].pb(mp(Const_Matr, finger_dip_start + EachFingerBoneNum * k));
			//finger tip
			for (int i = 0; i < Homo_mat[finger_dip_start + EachFingerBoneNum * k].size(); i++)
				Homo_mat[finger_tip_start + EachFingerBoneNum * k].pb(Homo_mat[finger_dip_start + EachFingerBoneNum * k][i]);
			Homo_mat[finger_tip_start + EachFingerBoneNum * k].pb(mp(Const_Matr, finger_tip_start + EachFingerBoneNum * k));
		}
	}

	template <typename Dtype>
	void DeepHandModelLayer<Dtype>::LayerSetUp(const vector<Blob<Dtype>*>& bottom,
		  const vector<Blob<Dtype>*>& top) 
	{
		int n;
		FILE *fin = fopen("configuration/DofConstraintId.in", "r");
		fscanf(fin, "%d", &n);
		for (int i = 0; i < ParamNum; i++) isFixed[i] = 0;
		for (int i = 0; i < n; i++) { int id; fscanf(fin, "%d", &id); isFixed[id] = 1; }
		fclose(fin);
		fin = fopen("configuration/InitialParameters.in", "r");
		for (int i = 0; i < ParamNum; i++)
		{
			fscanf(fin, "%lf", &initparam[i]);		
		}
		fclose(fin);   
		//load initial bone length(fixed number)
		fin = fopen("configuration/BoneLength.in", "r");
		for (int i = 0; i < BoneNum; i++)
		{
			fscanf(fin, "%lf", &bonelen[i]);
		}
		fclose(fin);
		SetupConstantMatrices();
		SetupTransformation();
	}


	template <typename Dtype>
	void DeepHandModelLayer<Dtype>::Reshape(const vector<Blob<Dtype>*>& bottom,
		  const vector<Blob<Dtype>*>& top) 
	{
	  const int axis = bottom[0]->CanonicalAxisIndex(
		  this->layer_param_.inner_product_param().axis());
	  vector<int> top_shape = bottom[0]->shape();
	  top_shape.resize(axis + 1);
	  top_shape[axis] = JointNum * 3;
	  top[0]->Reshape(top_shape);
	}

	template <typename Dtype>
	Matr DeepHandModelLayer<Dtype>::GetMatrix(matrix_operation opt, int bottom_id, int image_id, int param_id, bool is_gradient, const Dtype *bottom_data)
	{		
		return opt == Const_Matr ? const_matr[param_id] : Matr(opt, isFixed[param_id] ? initparam[param_id] : bottom_data[bottom_id + param_id] + initparam[param_id], is_gradient);
	}

	template <typename Dtype>
	void DeepHandModelLayer<Dtype>::Forward(Matr mat, int bottom_id, int image_id, int joint_id, int prev_size, const Dtype *bottom_data)
	{
		for (int r = prev_size; r < Homo_mat[joint_id].size(); r++)	mat = mat * GetMatrix(Homo_mat[joint_id][r].first, bottom_id, image_id, Homo_mat[joint_id][r].second, false, bottom_data);
		prev_mat[joint_id] = mat;
		t_joint[joint_id] = prev_mat[joint_id] * Vec(0.0, 0.0, 0.0, 1.0);
	}

	template <typename Dtype>
	void DeepHandModelLayer<Dtype>::Forward_cpu(const vector<Blob<Dtype>*>& bottom,
		const vector<Blob<Dtype>*>& top) 
	{
	  const Dtype* bottom_data = bottom[0]->cpu_data();
	  Dtype* top_data = top[0]->mutable_cpu_data();
	  const int batSize = (bottom[0]->shape())[0];  
	  for (int t = 0; t < batSize; t++) 
	  {
		int bottom_id = t * ParamNum;    
		int top_id = t * JointNum * 3;
		for (int i = 0; i < JointNum; i++) //in the order of "forward_seq"
		{			
			int id = forward_seq[i];
			Matr mat;
			if (prev_seq[i] != -1) mat = prev_mat[prev_seq[i]];
			Forward(mat, bottom_id, t, id, prev_seq[i] == -1 ? 0 : Homo_mat[prev_seq[i]].size(), bottom_data);
		}
		for (int i = 0; i < JointNum; i++) for (int j = 0; j < 3; j++) top_data[top_id + i * 3 + j] = t_joint[i][j];		
	  }
	}

	template <typename Dtype>
	void DeepHandModelLayer<Dtype>::Backward(int bottom_id, int image_id, int joint_id, const Dtype *bottom_data)
	{
		std::vector<std::pair<matrix_operation, int> > mat = Homo_mat[joint_id];
		Matr m_left[ParamNum * 3];
		Vec v_right[ParamNum * 3];
		v_right[mat.size() - 1] = Vec(0.0, 0.0, 0.0, 1.0);
		for (int r = mat.size() - 2; r >= 0; r--) v_right[r] = GetMatrix(mat[r + 1].first, bottom_id, image_id, mat[r + 1].second, false, bottom_data) * v_right[r + 1];
		m_left[0] = Matr(); //Identity matrix
		for (int r = 1; r < mat.size(); r++) m_left[r] = m_left[r - 1] * GetMatrix(mat[r - 1].first, bottom_id, image_id, mat[r - 1].second, false, bottom_data);
		for (int r = 0; r < mat.size(); r++) if (mat[r].first != Const_Matr) Jacobian[joint_id][mat[r].second] = isFixed[mat[r].second] ? Vec(0.0, 0.0, 0.0, 1.0) : m_left[r] * GetMatrix(mat[r].first, bottom_id, image_id, mat[r].second, true, bottom_data) * v_right[r];		
	}

	//Core idea: (ABCD)'=A'(BCD)+A(BCD)'    (BCD)'=B'(CD)+B(CD)'   (CD)'=C'D+CD'
	//Jacobian[i][j][0] : \frac{\partial x[i][0]}{\partial d[j]}  partial of x coordinate value of t_joint i with regard to the j-th DoF
	//Jacobian[i][j][1] : \frac{\partial x[i][1]}{\partial d[j]}  partial of y coordinate value of t_joint i with regard to the j-th DoF
	//Jacobian[i][j][2] : \frac{\partial x[i][2]}{\partial d[j]}  partial of z coordinate value of t_joint i with regard to the j-th DoF

	template <typename Dtype>
	void DeepHandModelLayer<Dtype>::Backward_cpu(const vector<Blob<Dtype>*>& top,
		const vector<bool>& propagate_down,
		const vector<Blob<Dtype>*>& bottom) 
	{
		if (propagate_down[0]) 
		{
			const Dtype* bottom_data = bottom[0]->cpu_data();
			const Dtype* top_diff = top[0]->cpu_diff();
			Dtype* bottom_diff = bottom[0]->mutable_cpu_diff();
			const int batSize = (bottom[0]->shape())[0];

			for (int t = 0; t < batSize; t++)
			{
				int bottom_id = t * ParamNum;
				for (int i = 0; i < JointNum; i++)
				{
					for (int j = 0; j < ParamNum; j++)
					{
						for (int k = 0; k < 4; k++)
							Jacobian[i][j].V[k] = 0.0;
					}
					Backward(bottom_id, t, i, bottom_data);
				}
				for (int j = 0; j < ParamNum; j++)
				{
					bottom_diff[bottom_id + j] = 0.0;
					for (int i = 0; i < JointNum; i++)
					{
						int top_id = t * JointNum * 3 + i * 3;
						for (int k = 0; k < 3; k++) bottom_diff[bottom_id + j] += Jacobian[i][j][k] * top_diff[top_id + k];
					}
				}
			}
		}		
	}

	#ifdef CPU_ONLY
	STUB_GPU(DeepHandModelLayer);
	#endif

	INSTANTIATE_CLASS(DeepHandModelLayer);
	REGISTER_LAYER_CLASS(DeepHandModel);
}  // namespace caffe