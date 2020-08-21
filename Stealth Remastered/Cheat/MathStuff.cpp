#include "main.h"

#define FLOAT_EPSILON 0.0001f

int near_zero(float v)
{
	if (!isfinite(v))
		return 1;
	return fabs(v) < FLOAT_EPSILON;
}

void vect2_normalize(const float in[2], float out[2])
{
	float	m;
	int		i;

	m = sqrtf(in[0] * in[0] + in[1] * in[1]);

	for (i = 0; i < 2; i++)
		out[i] = in[i] / m;
}

float vect2_length(const float in[2])
{
	return sqrtf(in[0] * in[0] + in[1] * in[1]);
}

int vect2_near_zero(const float in[2])
{
	return near_zero(in[0]) && near_zero(in[1]);
}

void vect3_zero(float out[3])
{
	for (int i = 0; i < 3; i++)
		out[i] = 0.0f;
}

void vect2_copy(const uint8_t in[2], uint8_t out[2])
{
	memcpy(out, in, sizeof(uint8_t) * 2);
}

float vect2_dist(float* point1, float* point2)
{
	return sqrt(pow(point1[0] - point2[0], 2) + pow(point1[1] - point2[1], 2));
}

float vect2_dist(CVector* point1, CVector* point2)
{
	return sqrt(pow(point1->fX - point2->fX, 2) + pow(point1->fY - point2->fY, 2));
}

void vect3_normalize(const float in[3], float out[3])
{
	float m = sqrtf(in[0] * in[0] + in[1] * in[1] + in[2] * in[2]);

	for (int i = 0; i < 3; i++)
		out[i] = in[i] / m;
}

float vect3_length(const float in[3])
{
	return sqrtf(in[0] * in[0] + in[1] * in[1] + in[2] * in[2]);
}

void vect3_div(const float in[3], float m, float out[3])
{
	for (int i = 0; i < 3; i++)
		out[i] = in[i] / m;
}

void vect3_mult(const float in[3], float m, float out[3])
{
	for (int i = 0; i < 3; i++)
		out[i] = in[i] * m;
}

void vect3_vect3_mult(const float in1[3], const float in2[3], float out[3])
{
	for (int i = 0; i < 3; i++)
		out[i] = in1[i] * in2[i];
}

void vect3_vect3_add(const float in1[3], const float in2[3], float out[3])
{
	for (int i = 0; i < 3; i++)
		out[i] = in1[i] + in2[i];
}

void vect3_vect3_sub(const float in1[3], const float in2[3], float out[3])
{
	for (int i = 0; i < 3; i++)
		out[i] = in1[i] - in2[i];
}

void vect3_invert(const float in[3], float out[3])
{
	for (int i = 0; i < 3; i++)
		out[i] = -in[i];
}

int vect3_near_zero(const float in[3])
{
	return near_zero(in[0]) && near_zero(in[1]) && near_zero(in[2]);
}

void vect3_copy(const float in[3], float out[3])
{
	memcpy(out, in, sizeof(float) * 3);
}

float vect3_dist(const float in1[3], const float in2[3])
{
	float dist[3];

	vect3_vect3_sub(in1, in2, dist);
	return vect3_length(dist);
}

int vect4_near_zero(const float in[4])
{
	return near_zero(in[0]) && near_zero(in[1]) && near_zero(in[2]) && near_zero(in[3]);
}

void vect4_copy(const uint8_t in[4], uint8_t out[4])
{
	memcpy(out, in, sizeof(uint8_t) * 4);
}

void vect4_copy(const float in[4], float out[4])
{
	memcpy(out, in, sizeof(float) * 4);
}

void matrix_copy(const float in[16], float out[16])
{
	memcpy(out, in, sizeof(float) * 16);
}

float vect3_dot_product(const float in1[3], const float in2[3])
{
	return in1[0] * in2[0] + in1[1] * in2[1] + in1[2] * in2[2];
}

void vect3_cross_product(const float in1[3], const float in2[3], float out[3])
{
	out[0] = (in1[1] * in2[2]) - (in2[1] * in1[2]);
	out[1] = (in1[2] * in2[0]) - (in2[2] * in1[0]);
	out[2] = (in1[0] * in2[1]) - (in2[0] * in1[1]);
}

void matrix_vect3_mult(const float matrix[16], const float in[3], float out[3])
{
	const float in4[4] = { in[0], in[1], in[2], 1.0f };
	float		out4[4];

	matrix_vect4_mult(matrix, in4, out4);
	vect3_copy(out4, out);
}

void matrix_vect4_mult(const float matrix[16], const float in[4], float out[4])
{
	float	res[4];

	for (int i = 0; i < 4; i++)
	{
		res[i] = in[0] * matrix[0 + i] + in[1] * matrix[4 + i] + in[2] * matrix[8 + i] + in[3] * matrix[12 + i];
	}

	vect4_copy(res, out);
}

void matrix_matrix_mult(const float in1[16], const float in2[16], float out[16])
{
	float	res[16];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			res[i * 4 + j] = in1[i * 4 + 0] *
				in2[0 + j] +
				in1[i * 4 + 1] *
				in2[4 + j] +
				in1[i * 4 + 2] *
				in2[8 + j] +
				in1[i * 4 + 3] *
				in2[12 + j];
		}
	}

	matrix_copy(res, out);
}

void matrix_vect3_rotate(const float in[16], const float vect[3], float t, float out[16])
{
	const float x = vect[0], y = vect[1], z = vect[2];
	const float sin_t = sinf(t), cos_t = cosf(t);
	float		res[16];
	const float matrix[16] =
	{
		cos_t + (1.0f - cos_t) * x * x,
		(1.0f - cos_t) * x * y - sin_t * z,
		(1.0f - cos_t) * x * z + sin_t * y,
		0.0f,
		(1.0f - cos_t) * y * x + sin_t * z,
		cos_t + (1.0f - cos_t) * y * y,
		(1.0f - cos_t) * y * z - sin_t * x,
		0.0f,
		(1.0f - cos_t) * z * x - sin_t * y,
		(1.0f - cos_t) * z * y + sin_t * x,
		cos_t + (1.0f - cos_t) * z * z,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		1.0f
	};

	matrix_matrix_mult(in, matrix, res);
	vect4_copy(&in[4 * 3], &res[4 * 3]);
	matrix_copy(res, out);
}

void matrix_vect3_switchXY(const float in[16], float out[16])
{
	float	res[16];
	vect4_copy(&in[4 * 0], &res[4 * 0]);
	vect4_copy(&in[4 * 1], &res[4 * 1]);
	vect4_copy(&in[4 * 2], &res[4 * 2]);

	vect4_copy(&in[4 * 3], &res[4 * 3]);
	matrix_copy(res, out);
}

void matrix_identity(float out[16])
{
	static const float	id[16] =
	{
		1.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		1.0f
	};

	matrix_copy(id, out);
}

uint32_t encode_panels(unsigned char flp, unsigned char frp, unsigned char rlp, unsigned char rrp, unsigned char windshield, unsigned char front_bumper, unsigned char rear_bumper)
{
	return flp | (frp << 4) | (rlp << 8) | (rrp << 12) | (windshield << 16) | (front_bumper << 20) | (rear_bumper << 24);
}

uint32_t encode_doors(unsigned char bonnet, unsigned char boot, unsigned char driver_door, unsigned char passenger_door)
{
	return bonnet | (boot << 8) | (driver_door << 16) | (passenger_door << 24);
}

float random_float(float a, float b)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}