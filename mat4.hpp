#pragma once

//A *small* matrix math library for 4x4 matrices only

#include <array>
#include <cmath>
#include <cstdint>

//NOTE: column-major storage order (like in OpenGl /GLSL):
using mat4 = std::array< float, 16>;
static_assert(sizeof(mat4) == 16*4, "mat4 is exactly 16 32-bit floats.");

using vec4 = std::array< float, 4>;
static_assert(sizeof(vec4) == 4*4, "vec4 is exactly 4 32-bit floats.");

using vec3 = std::array< float, 3>;
static_assert(sizeof(vec3) == 3*4, "vec3 is exactly 3 32-bit floats.");

inline vec4 operator*(mat4 const &A, vec4 const &b) {
    vec4 ret;
    //compute ret = A * b:
    for (uint32_t r = 0; r < 4; r++) {
        ret[r] = A[0 * 4 + r] * b[0];
        for(uint32_t k = 1; k < 4; k++){
            ret[r] += A[k * 4 + r] * b[k];
        } 
    }
    return ret;
}

inline mat4 operator*(mat4 const &A, mat4 const &B) {
	mat4 ret;
	//compute ret = A * B:
	for (uint32_t c = 0; c < 4; c++) {
		for (uint32_t r = 0; r < 4; r++) {
			ret[c * 4 + r] = A[0 * 4 + r] * B[c * 4 + 0];
			for (uint32_t k = 1; k < 4; k++) {
				ret[c * 4 + r] += A[k * 4 + r] * B[c * 4 + k];
			}
		}
	}
	return ret;
}

inline vec4 operator*(vec4 const &v, float const &c) {
    vec4 ret;
    //compute ret = v * c :
    for (uint32_t r = 0; r < 4; r++) {
        ret[r] = v[r] * c;
    }
    return ret;
}
inline vec4 operator*(float const &c, vec4 const &v) {
    return v * c;
}

inline vec4 operator*(vec4 const &v0, vec4 const &v1){
    vec4 v;
    for(int i = 0; i < 4; i++){
        v[i] = v0[i] * v1[i];
    }
    return v;
}

inline vec4 operator+(vec4 const &v0, vec4 const &v1){
    vec4 v;
    for(int i = 0; i < 4; i++){
        v[i] = v0[i] + v1[i];
    }
    return v;
}


//perspective projection matrix.
// - vfov is fov *in radians*
// - near maps to 0, far maps to 1
// looks down -z with +y up and +x right
inline mat4 perspective(float vfov, float aspect, float near, float far){
    //as per https://www.terathon.com/gdc07_lengyel.pdf
	// (with modifications for Vulkan-style coordinate system)
	//  notably: flip y (vulkan device coords are y-down)
	//       and rescale z (vulkan device coords are z-[0,1])
    const float e = 1.0f / std::tan(vfov / 2.0f);
    const float a = aspect;
    const float n = near;
    const float f = far;
    return mat4{ //note: column-major storage order!
        e/a,    0.f,                    0.f,    0.f,
        0.f,    -e,                     0.f,    0.f,
        0.f,    0.f, -0.5f - 0.5f * (f+n)/(f-n),-1.f,
        0.f,    0.f,            -(f*n)/(f-n), 0.f,
    };
}

inline mat4 quaternianToMatrix(vec4 quaternian){
    // float x = quaternian[0];
    // float y = quaternian[1];
    // float z = quaternian[2];
    // float w = quaternian[3];

    // float m_00 = 1.0f - 2.0f * (y * y + z * z);
    // float m_01 = 2.0f * (x * y - w * z);
    // float m_02 = 2.0f * (x * z + w * y);

    // float m_10 = 2.0f * (x * y + w * z);
    // float m_11 = 1.0f - 2.0f * (x * x + z * z);
    // float m_12 = 2.0f * (y * z - w * x);

    // float m_20 = 2.0f * (x * z - w * y);
    // float m_21 = 2.0f * (y * z + w * x);
    // float m_22 = 1.0f - 2.0f * (x * x + y * y);

    // return mat4{m_00, m_10, m_20, 0.f,
    //             m_01, m_11, m_21, 0.f,
    //             m_02, m_12, m_22, 0.f,
    //             0.f,  0.f,  0.f,  1.f};

    float x = quaternian[0];
    float y = quaternian[1];
    float z = quaternian[2];
    float w = quaternian[3];

    float m_00 = 2.0f * (w * w + x * x) - 1.0f;
    float m_01 = 2.0f * (x * y - w * z);
    float m_02 = 2.0f * (x * z + w * y);

    float m_10 = 2.0f * (x * y + w * z);
    float m_11 = 2.0f * (w * w + y * y) - 1.0f;
    float m_12 = 2.0f * (y * z - w * x);

    float m_20 = 2.0f * (x * z - w * y);
    float m_21 = 2.0f * (y * z + w * x);
    float m_22 = 2.0f * (w * w + z * z) - 1.0f;

    return mat4{m_00, m_10, m_20, 0.f,
                m_01, m_11, m_21, 0.f,
                m_02, m_12, m_22, 0.f,
                0.f,  0.f,  0.f,  1.f};
}

//look at matrix:
// makes a camera-space-from-world matrix for a camera at eye looking toward
// target with up-vector pointing (as-close-as-possible) along up.
// That is, it maps:
//  - eye_xyz to the origin
//  - the unit length vector from eye_xyz to target_xyz to -z
//  - an as-close-as-possible unit-length vector to up to +y
inline mat4 look_at(
    float eye_x, float eye_y, float eye_z,
    float target_x, float target_y, float target_z,
    float up_x, float up_y, float up_z) {
    
    //NOTE: this would be a lot cleaner witha vec3 type and some overloads!

    //compute vector from eye to target:
	float in_x = target_x - eye_x;
	float in_y = target_y - eye_y;
	float in_z = target_z - eye_z;
    
    //normalize 'in' vector:
	float inv_in_len = 1.0f / std::sqrt(in_x*in_x + in_y*in_y + in_z*in_z);
	in_x *= inv_in_len;
	in_y *= inv_in_len;
	in_z *= inv_in_len;

    //make 'up' orthogonal to 'in':
	float in_dot_up = in_x*up_x + in_y*up_y +in_z*up_z;
	up_x -= in_dot_up * in_x;
	up_y -= in_dot_up * in_y;
	up_z -= in_dot_up * in_z;

    //normalize 'up' vector:
	float inv_up_len = 1.0f / std::sqrt(up_x*up_x + up_y*up_y + up_z*up_z);
	up_x *= inv_up_len;
	up_y *= inv_up_len;
	up_z *= inv_up_len;

    //compute 'right' vector as 'in' x 'up'
	float right_x = in_y*up_z - in_z*up_y;
	float right_y = in_z*up_x - in_x*up_z;
	float right_z = in_x*up_y - in_y*up_x;

    //compute dot products of right, in, up with eye:
	float right_dot_eye = right_x*eye_x + right_y*eye_y + right_z*eye_z;
	float up_dot_eye = up_x*eye_x + up_y*eye_y + up_z*eye_z;
	float in_dot_eye = in_x*eye_x + in_y*eye_y + in_z*eye_z;

    //final matrix: computes (right . (v - eye), up . (v - eye), -in . (v - eye), v.w)
    return mat4{ //note: column-major storage order
        right_x, up_x, -in_x, 0.f,
        right_y, up_y, -in_y, 0.f,
        right_z, up_z, -in_z, 0.f,
        -right_dot_eye, -up_dot_eye, in_dot_eye, 1.f,
    };
}

inline vec4 inverse(vec4 v){
    vec4 inv;
    for(int i = 0; i < 4; i++){
        if(v[i] == 0.f){
            inv[i] = 0.f;
        } else {
            inv[i] = 1.f / v[i];
        }
    }
    return inv;
}

inline float dot(vec4 v0, vec4 v1){
    float ret = 0.f;
    for(int i = 0; i < 4; i++){
        ret += v0[i] * v1[i];
    }
    return ret;
}

inline vec4 pow(vec4 v, float u){
    vec4 ret;
    for(int i = 0; i < 4; i++){
        ret[i] = std::pow(v[i], u);
    }
    return ret;
}



