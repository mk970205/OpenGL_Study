#include "quaternion.hpp"
#include <glm/ext/matrix_transform.hpp>
using namespace glm;
vec4 create_versor(float theta, vec3 axis) {
	return vec4(
		cos(theta / 2),
		sin(theta / 2) * axis.x,
		sin(theta / 2) * axis.y,
		sin(theta / 2) * axis.z
	);
}

mat4 quat_to_mat(vec4 quaternion) {
	vec4 quat_norm = normalize(quaternion);
	float w = quat_norm.w;
	float x = quat_norm.x;
	float y = quat_norm.y;
	float z = quat_norm.z;

	return mat4(
		1 - 2 * y*y - 2 * z*z, 2 * x*y + 2 * w*z, 2 * x*z - 2 * w*y, 0,
		2 * x*y - 2 * w*z, 1 - 2 * x*x - 2 * z*z, 2 * y*z + 2 * w*x, 0,
		2 * x*z + 2 * w*y, 2 * y*z - 2 * w*x, 1 - 2 * x*x - 2 * y*y, 0,
		0, 0, 0, 1
	);
}

vec4 multiply_quat(vec4 q1, vec4 q2) {
	return vec4(
		q2.w * q1.w - q2.x * q1.x - q2.y * q1.y - q2.z * q1.z,
		q2.w * q1.x + q2.x * q1.w - q2.y * q1.z + q2.z * q1.y,
		q2.w * q1.y + q2.x * q1.z + q2.y * q1.w - q2.z * q1.x,
		q2.w * q1.z - q2.x * q1.y + q2.y * q1.x + q2.z * q1.w
	);
}

vec4 slerp(vec4 q1, vec4 q2, float t) {
	float quaternion_dot = dot(q1, q2);
	if (quaternion_dot >= 1.0f) { // extremely small angle
		return q1;
	}
	float ohm = acos(quaternion_dot);
	return (q1 * sin((1.0f - t) * ohm) + q2 * sin(t * ohm)) / sin(ohm);
}