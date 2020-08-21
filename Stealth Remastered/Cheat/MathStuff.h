#pragma once

#ifndef M_PI
#define M_PI	3.14159265358979323844f
#endif
#define DEGTORAD( Deg ) ( (Deg * M_PI) / 180.0f )
#define RADTODEG( Deg ) ( (Deg * 180.f) / M_PI )

int		near_zero(float v);
void	vect2_normalize(const float in[2], float out[2]);
float	vect2_length(const float in[2]);
int		vect2_near_zero(const float in[2]);
void	vect2_copy(const uint8_t in[2], uint8_t out[2]);
float	vect2_dist(float* point1, float* point2);
float	vect2_dist(CVector* point1, CVector* point2);
void	vect3_zero(float out[3]);
void	vect3_normalize(const float in[3], float out[3]);
void	vect3_mult(const float in[3], float m, float out[3]);
float	vect3_length(const float in[3]);
void	vect3_div(const float in[3], float m, float out[3]);
void	vect3_mult(const float in[3], float m, float out[3]);
void	vect3_invert(const float in[3], float out[3]);
void	vect3_vect3_mult(const float in1[3], const float in2[3], float out[3]);
void	vect3_vect3_add(const float in1[3], const float in2[3], float out[3]);
void	vect3_vect3_sub(const float in1[3], const float in2[3], float out[3]);
int		vect3_near_zero(const float in[3]);
void	vect3_copy(const float in[3], float out[3]);
float	vect3_dist(const float in1[3], const float in2[3]);
int		vect4_near_zero(const float in[4]);
void	vect4_copy(const uint8_t in[4], uint8_t out[4]);
void	vect4_copy(const float in[4], float out[4]);
void	matrix_copy(const float in[16], float out[16]);
float	vect3_dot_product(const float in1[3], const float in2[3]);
void	vect3_cross_product(const float in1[3], const float in2[3], float out[3]);
void	matrix_vect3_mult(const float matrix[16], const float in[3], float out[3]);
void	matrix_vect4_mult(const float matrix[16], const float in[4], float out[4]);
void	matrix_matrix_mult(const float in1[16], const float in2[16], float out[16]);
void	matrix_vect3_rotate(const float in[16], const float vect[3], float t, float out[16]);
void	matrix_vect3_switchXY(const float in[16], float out[16]);
void	matrix_identity(float out[16]);

uint32_t encode_panels(unsigned char flp, unsigned char frp, unsigned char rlp, unsigned char rrp, unsigned char windshield, unsigned char front_bumper, unsigned char rear_bumper);
uint32_t encode_doors(unsigned char bonnet, unsigned char boot, unsigned char driver_door, unsigned char passenger_door);

float random_float(float a, float b);