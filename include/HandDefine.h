#pragma once
#define pb push_back
#define mp std::make_pair
enum basic_settings
{
	JointNum = 31, 
	ParamNum = 47,/*While Our hand Model fixes the seven joints namely "Bone_1_MCP, Bone_2_MCP, Bone_3_MCP, Bone_4_MCP, wrist_left, wrist_middle, thumb_MCP" on the palm,
	              the following dimensions : 6-14, 19-30 are fixed in order to keep the relative spatial relationships of the seven joints.
				  Therefore our hand model actually has 26 tunable DoFs: dimension 0-5, 15-18, 31-46*/
	BoneNum = 30,
	ConstMatrNum = JointNum,
	EachFingerBoneNum = 5,
	EachMCPDoFNum = 3,
	EachFingerDoFNum = 4
};

enum joint //31
{
	//little finger:
	little_finger_tip, little_finger_dip, little_finger_pip_second, little_finger_pip_first, little_finger_base,  
	//ring finger:
	ring_finger_tip, ring_finger_dip, ring_finger_pip_second, ring_finger_pip_first, ring_finger_base,
	//middle finger:
	middle_finger_tip, middle_finger_dip, middle_finger_pip_second, middle_finger_pip_first, middle_finger_base,
	//index finger:
	index_finger_tip, index_finger_dip, index_finger_pip_second, index_finger_pip_first, index_finger_base,
	//4 MCP:
	little_finger_mcp, ring_finger_mcp, middle_finger_mcp, index_finger_mcp,
	palm_center,
	//the left 3 fixed joints:
	wrist_left, wrist_middle, thumb_mcp,
	//thumb finger:
	thumb_pip, thumb_dip, thumb_tip
};

enum bone //30
{
	//little finger:
	bone_little_finger_tip_connect_dip, bone_little_finger_dip_connect_pip_second, bone_little_finger_pip_second_connect_pip_first, bone_little_finger_pip_first_connect_finger_base, bone_little_finger_base_connect_mcp,
	//ring finger:
	bone_ring_finger_tip_connect_dip, bone_ring_finger_dip_connect_pip_second, bone_ring_finger_pip_second_connect_pip_first, bone_ring_finger_pip_first_connect_finger_base, bone_ring_finger_base_connect_mcp,
	//middle finger:
	bone_middle_finger_tip_connect_dip, bone_middle_finger_dip_connect_pip_second, bone_middle_finger_pip_second_connect_pip_first, bone_middle_finger_pip_first_connect_finger_base, bone_middle_finger_base_connect_mcp,
	//index finger:
	bone_index_finger_tip_connect_dip, bone_index_finger_dip_connect_pip_second, bone_index_finger_pip_second_connect_pip_first, bone_index_finger_pip_first_connect_finger_base, bone_index_finger_base_connect_mcp,
	//4 MCP connect palm center
	bone_little_finger_mcp_connect_palm_center, bone_ring_finger_mcp_connect_palm_center, bone_middle_finger_mcp_connect_palm_center, bone_index_finger_mcp_connect_palm_center,
	bone_palm_center_connect_wrist_left, bone_palm_center_connect_wrist_middle, bone_palm_center_connect_thumb_mcp, 
	//thumb finger:
	bone_thumb_mcp_connect_pip, bone_thumb_pip_connect_dip, bone_thumb_dip_connect_tip
};

enum dof //47(only 26 are tunable, while the left 21 are for fixing seven joints on the palm)
{
	//0-5: tunable
	global_trans_x, global_trans_y, global_trans_z, global_rot_x, global_rot_y, global_rot_z,
	//6-14 are fixed constants to fix wrist_left, wrist_middle and thumb_mcp
	wrist_left_const_rot_x, wrist_left_const_rot_y, wrist_left_const_rot_z, wrist_middle_const_rot_x, wrist_middle_const_rot_y, wrist_middle_const_rot_z, thumb_mcp_const_rot_x, thumb_mcp_const_rot_y, thumb_mcp_const_rot_z,
	//15-18: tunable
	thumb_pip_rot_y, thumb_pip_rot_z, thumb_dip_rot_z, thumb_tip_rot_z,
	//19-30 are fixed constants to fix 4 MCP joints of the four fingers(expect thumb finger)
	little_finger_mcp_const_rot_x, little_finger_mcp_const_rot_y, little_finger_mcp_const_rot_z,
	ring_finger_mcp_const_rot_x, ring_finger_mcp_const_rot_y, ring_finger_mcp_const_rot_z,
	middle_finger_mcp_const_rot_x, middle_finger_mcp_const_rot_y, middle_finger_mcp_const_rot_z,
	index_finger_mcp_const_rot_x, index_finger_mcp_const_rot_y, index_finger_mcp_const_rot_z,
	//31-46: tunable
	little_finger_base_rot_x, little_finger_base_rot_z, little_finger_pip_rot_x, little_finger_dip_rot_x,
	ring_finger_base_rot_x, ring_finger_base_rot_z, ring_finger_pip_rot_x, ring_finger_dip_rot_x,
	middle_finger_base_rot_x, middle_finger_base_rot_z, middle_finger_pip_rot_x, middle_finger_dip_rot_x,
	index_finger_base_rot_x, index_finger_base_rot_z, index_finger_pip_rot_x, index_finger_dip_rot_x
};

enum finger_joint_start_id
{
	finger_tip_start = 0, //e.g. little_finger_tip = 0, ring_finger_tip = 5, middle_finger_tip = 10, index_finger_tip = 15(cycle is num of bones each finger : 5)
	finger_dip_start = 1, finger_pip_second_start = 2, finger_pip_first_start = 3, finger_base_start = 4,
	finger_mcp_start = 20, 	
};

enum bone_start_id
{
	bone_finger_tip_connect_dip_start = 0, bone_finger_dip_connect_pip_second_start = 1, bone_finger_pip_second_connect_pip_first_start = 2, bone_finger_pip_first_connect_finger_base_start = 3, bone_finger_base_connect_finger_mcp_start = 4,
	bone_finger_mcp_connect_palm_center_start = 20, 	
};

enum dof_start_id
{
	finger_mcp_rot_x_start = 19, finger_mcp_rot_y_start = 20, finger_mcp_rot_z_start = 21,
	finger_base_rot_x_start = 31, finger_base_rot_z_start = 32, finger_pip_rot_x_start = 33, finger_dip_rot_x_start = 34
};

const int forward_seq[JointNum] = { palm_center, wrist_left, wrist_middle, thumb_mcp, thumb_pip, thumb_dip, thumb_tip, little_finger_mcp, ring_finger_mcp, middle_finger_mcp, index_finger_mcp,
									little_finger_base, little_finger_pip_first, little_finger_pip_second, little_finger_dip, little_finger_tip,
									ring_finger_base, ring_finger_pip_first, ring_finger_pip_second, ring_finger_dip, ring_finger_tip,
									middle_finger_base, middle_finger_pip_first, middle_finger_pip_second, middle_finger_dip, middle_finger_tip,
									index_finger_base, index_finger_pip_first, index_finger_pip_second, index_finger_dip, index_finger_tip
								  };
const int prev_seq[JointNum] =    { -1, palm_center, palm_center, palm_center, thumb_mcp, thumb_pip, thumb_dip, palm_center, palm_center, palm_center, palm_center,
								    little_finger_mcp, little_finger_base, little_finger_pip_first, little_finger_pip_second, little_finger_dip,
									ring_finger_mcp, ring_finger_base, ring_finger_pip_first, ring_finger_pip_second, ring_finger_dip,
									middle_finger_mcp, middle_finger_base, middle_finger_pip_first, middle_finger_pip_second, middle_finger_dip,
									index_finger_mcp, index_finger_base, index_finger_pip_first, index_finger_pip_second, index_finger_dip };
