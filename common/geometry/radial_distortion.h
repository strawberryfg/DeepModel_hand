#pragma once


	// check the explanation of the distortion parameters at
	// http://www.vision.caltech.edu/bouguetj/calib_doc/htmls/parameters.html

	struct PlumbBobModel
	{
		float kc[5];

		PlumbBobModel()
		{
			kc[0] = kc[1] = kc[2] = kc[3] = kc[4] = 0; // no distortion
		}

		PlumbBobModel(const float _kc[5])
		{
			Set(_kc);
		}

		void Set(const float _kc[5])
		{
			kc[0] = _kc[0]; kc[1] = _kc[1]; kc[2] = _kc[2]; kc[3] = _kc[3]; kc[4] = _kc[4];
		}

		float K_radial(float r2) const
		{	
			return 1.0f + kc[0]*r2 + kc[1]*r2*r2 + kc[4]*r2*r2*r2;	
		}

		void tangential(float x, float y, float r2, float& dx, float& dy) const
		{
			dx = 2.0f*kc[2]*x*y + kc[3]*(r2+2.0f*x*x);
			dy = kc[2]*(r2+2.0f*y*y) + 2.0f*kc[3]*x*y;
		}

		void distort(float x, float y, float& xd, float& yd) const
		{
			float r2 = x*x + y*y;
			float k_radial = K_radial(r2);
			tangential(x, y, r2, xd, yd);
			xd += x*k_radial;
			yd += y*k_radial;
		}

		int undistort(float x, float y, float& dx, float& dy) const
		{			
			dx = x; dy = y;
			int i = 0;
			while (i < 20)
			{
				float r2 = dx*dx + dy*dy;
				float k_radial = K_radial(r2);

				float tanDistx, tanDisty;
				tangential(dx, dy, r2, tanDistx, tanDisty);

				float old_dx = dx, old_dy = dy;
				dx = (x - tanDistx) / k_radial;
				dy = (y - tanDisty) / k_radial;
				i++;

				// if change is too small, quit, usually no more than 10 iterations are needed
				if ((dx-old_dx)*(dx-old_dx)+(dy-old_dy)*(dy-old_dy) < 1e-10)
					return i;
			}
			return i;
		}
	};

	// C implementation
	inline void radial_distort(float x, float y, float kc[5], float& dx, float& dy)
	{
		float r2 = x*x + y*y;
		float k_radial = 1.0f + kc[0]*r2 + kc[1]*r2*r2 + kc[4]*r2*r2*r2;
		//dx = x * k_radial + 2.0f*kc[2]*x*y + kc[3]*(r2+2.0*x*y);  // this orginal line is wrong!
		dx = x * k_radial + 2.0f*kc[2]*x*y + kc[3]*(r2+2.0f*x*x);
		dy = y * k_radial + kc[2]*(r2+2.0f*y*y) + 2.0f*kc[3]*x*y;
	}

	//Compensates for radial and tangential distortion. Model From Oulu university.
	//For more informatino about the distortion model, check the forward projection mapping function:
	//INPUT: pt2d: distorted (normalized) point coordinates in the image plane
	//       k: Distortion coefficients (radial and tangential) (5x1 vector, member of the class)
	//RETURN Point2D: undistorted (normalized) point coordinates in the image plane (2xN matrix)
	//Method: Iterative method for compensation.
	//NOTE: This compensation should be done after the subtraction
	//      of the principal point, and division by the focal length.

	//pt_Distorted = ptInput * (K_radial) + tangential_distortion
	//so a iterative method is :
	// pt1 = ptInput
	// for ...	
	//     K_radial=radial(pt1), tangential_distortion=tangential(pt1)
	//     pt1= (ptInput-tangential_distortion)/K_radial
	// end
	inline void radial_undistort(float x, float y, float kc[5], float& dx, float& dy)
	{
		dx = x;
		dy = y;
		for (int i = 0; i < 20; ++i)
		{
			float r2 = dx*dx + dy*dy;
			float k_radial = 1.0f + kc[0]*r2 + kc[1]*r2*r2 + kc[4]*r2*r2*r2;
			float tanDistx = 2.0f*kc[2]*dx*dy + kc[3]*(r2+2.0f*dx*dx);
			float tanDisty = kc[2]*(r2+2.0f*dy*dy) + 2.0f*kc[3]*dx*dy;
			dx = (x - tanDistx) / k_radial;
			dy = (y - tanDisty) / k_radial;
		}
	}