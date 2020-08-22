#pragma once

#ifndef M_PI
#define M_PI	3.14159265358979323844f
#endif
#define DEGTORAD( Deg ) ( (Deg * M_PI) / 180.0f )
#define RADTODEG( Deg ) ( (Deg * 180.f) / M_PI )
#define FLOAT_EPSILON 0.0001f

namespace Math
{
	static int near_zero(float v)
	{
		if (!isfinite(v))
			return 1;
		return fabs(v) < FLOAT_EPSILON;
	}

	static void vect2_normalize(const float in[2], float out[2])
	{
		float	m;
		int		i;

		m = sqrtf(in[0] * in[0] + in[1] * in[1]);

		for (i = 0; i < 2; i++)
			out[i] = in[i] / m;
	}

	static float vect2_length(const float in[2])
	{
		return sqrtf(in[0] * in[0] + in[1] * in[1]);
	}

	static int vect2_near_zero(const float in[2])
	{
		return near_zero(in[0]) && near_zero(in[1]);
	}

	static void vect3_zero(float out[3])
	{
		for (int i = 0; i < 3; i++)
			out[i] = 0.0f;
	}

	static void vect2_copy(const uint8_t in[2], uint8_t out[2])
	{
		memcpy(out, in, sizeof(uint8_t) * 2);
	}

	static float vect2_dist(float* point1, float* point2)
	{
		return sqrt(pow(point1[0] - point2[0], 2) + pow(point1[1] - point2[1], 2));
	}

	static float vect2_dist(CVector* point1, CVector* point2)
	{
		return sqrt(pow(point1->fX - point2->fX, 2) + pow(point1->fY - point2->fY, 2));
	}

	static void vect3_normalize(const float in[3], float out[3])
	{
		float m = sqrtf(in[0] * in[0] + in[1] * in[1] + in[2] * in[2]);

		for (int i = 0; i < 3; i++)
			out[i] = in[i] / m;
	}

	static float vect3_length(const float in[3])
	{
		return sqrtf(in[0] * in[0] + in[1] * in[1] + in[2] * in[2]);
	}

	static void vect3_div(const float in[3], float m, float out[3])
	{
		for (int i = 0; i < 3; i++)
			out[i] = in[i] / m;
	}

	static void vect3_mult(const float in[3], float m, float out[3])
	{
		for (int i = 0; i < 3; i++)
			out[i] = in[i] * m;
	}

	static void vect3_vect3_mult(const float in1[3], const float in2[3], float out[3])
	{
		for (int i = 0; i < 3; i++)
			out[i] = in1[i] * in2[i];
	}

	static void vect3_vect3_add(const float in1[3], const float in2[3], float out[3])
	{
		for (int i = 0; i < 3; i++)
			out[i] = in1[i] + in2[i];
	}

	static void vect3_vect3_sub(const float in1[3], const float in2[3], float out[3])
	{
		for (int i = 0; i < 3; i++)
			out[i] = in1[i] - in2[i];
	}

	static void vect3_invert(const float in[3], float out[3])
	{
		for (int i = 0; i < 3; i++)
			out[i] = -in[i];
	}

	static int vect3_near_zero(const float in[3])
	{
		return near_zero(in[0]) && near_zero(in[1]) && near_zero(in[2]);
	}

	static void vect3_copy(const float in[3], float out[3])
	{
		memcpy(out, in, sizeof(float) * 3);
	}

	static float vect3_dist(const float in1[3], const float in2[3])
	{
		float dist[3];

		vect3_vect3_sub(in1, in2, dist);
		return vect3_length(dist);
	}

	static float vect3_dot_product(const float in1[3], const float in2[3])
	{
		return in1[0] * in2[0] + in1[1] * in2[1] + in1[2] * in2[2];
	}

	static void vect3_cross_product(const float in1[3], const float in2[3], float out[3])
	{
		out[0] = (in1[1] * in2[2]) - (in2[1] * in1[2]);
		out[1] = (in1[2] * in2[0]) - (in2[2] * in1[0]);
		out[2] = (in1[0] * in2[1]) - (in2[0] * in1[1]);
	}
}