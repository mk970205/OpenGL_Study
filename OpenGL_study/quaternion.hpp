#pragma once
vec4 create_versor(float theta, vec3 axis);
mat4 quat_to_mat(vec4 quaternion);
vec4 slerp(vec4 q1, vec4 q2, float t);