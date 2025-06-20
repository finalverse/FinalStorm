#pragma once

// Platform-agnostic math types for FinalStorm
// Automatically uses simd on Apple platforms, GLM on others

#ifdef __APPLE__
    #include <simd/simd.h>
    
    namespace FinalStorm {
        // Vector types
        using vec2 = simd_float2;
        using vec3 = simd_float3;
        using vec4 = simd_float4;
        
        using ivec2 = simd_int2;
        using ivec3 = simd_int3;
        using ivec4 = simd_int4;
        
        using uvec2 = simd_uint2;
        using uvec3 = simd_uint3;
        using uvec4 = simd_uint4;
        
        // Matrix types
        using mat2 = simd_float2x2;
        using mat3 = simd_float3x3;
        using mat4 = simd_float4x4;
        
        // Quaternion
        using quat = simd_quatf;
        
        // Helper functions to match GLM interface
        inline vec3 make_vec3(float x, float y, float z) {
            return simd_make_float3(x, y, z);
        }
        
        inline vec4 make_vec4(float x, float y, float z, float w) {
            return simd_make_float4(x, y, z, w);
        }
        
        inline vec4 make_vec4(const vec3& v, float w) {
            return simd_make_float4(v.x, v.y, v.z, w);
        }
        
        inline mat4 make_mat4(float diagonal = 1.0f) {
            return matrix_identity_float4x4;
        }
        
        // Math operations
        inline float dot(const vec3& a, const vec3& b) {
            return simd_dot(a, b);
        }
        
        inline vec3 cross(const vec3& a, const vec3& b) {
            return simd_cross(a, b);
        }
        
        inline vec3 normalize(const vec3& v) {
            return simd_normalize(v);
        }
        
        inline float length(const vec3& v) {
            return simd_length(v);
        }
        
        inline mat4 transpose(const mat4& m) {
            return simd_transpose(m);
        }
        
        inline mat4 inverse(const mat4& m) {
            return simd_inverse(m);
        }
        
        // Transformation matrices
        inline mat4 translate(const mat4& m, const vec3& v) {
            mat4 result = m;
            result.columns[3] = m.columns[0] * v.x + m.columns[1] * v.y + m.columns[2] * v.z + m.columns[3];
            return result;
        }
        
        inline mat4 scale(const mat4& m, const vec3& v) {
            mat4 result;
            result.columns[0] = m.columns[0] * v.x;
            result.columns[1] = m.columns[1] * v.y;
            result.columns[2] = m.columns[2] * v.z;
            result.columns[3] = m.columns[3];
            return result;
        }
        
        inline mat4 rotate(const mat4& m, float angle, const vec3& axis) {
            float c = cos(angle);
            float s = sin(angle);
            vec3 a = normalize(axis);
            vec3 temp = a * (1.0f - c);
            
            mat4 rot;
            rot.columns[0].x = c + temp.x * a.x;
            rot.columns[0].y = temp.x * a.y + s * a.z;
            rot.columns[0].z = temp.x * a.z - s * a.y;
            rot.columns[0].w = 0.0f;
            
            rot.columns[1].x = temp.y * a.x - s * a.z;
            rot.columns[1].y = c + temp.y * a.y;
            rot.columns[1].z = temp.y * a.z + s * a.x;
            rot.columns[1].w = 0.0f;
            
            rot.columns[2].x = temp.z * a.x + s * a.y;
            rot.columns[2].y = temp.z * a.y - s * a.x;
            rot.columns[2].z = c + temp.z * a.z;
            rot.columns[2].w = 0.0f;
            
            rot.columns[3] = simd_make_float4(0.0f, 0.0f, 0.0f, 1.0f);
            
            return simd_mul(m, rot);
        }
        
        inline mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up) {
            vec3 f = normalize(center - eye);
            vec3 s = normalize(cross(f, up));
            vec3 u = cross(s, f);
            
            mat4 result = matrix_identity_float4x4;
            result.columns[0].x = s.x;
            result.columns[1].x = s.y;
            result.columns[2].x = s.z;
            result.columns[0].y = u.x;
            result.columns[1].y = u.y;
            result.columns[2].y = u.z;
            result.columns[0].z = -f.x;
            result.columns[1].z = -f.y;
            result.columns[2].z = -f.z;
            result.columns[3].x = -dot(s, eye);
            result.columns[3].y = -dot(u, eye);
            result.columns[3].z = dot(f, eye);
            return result;
        }
        
        inline mat4 perspective(float fovy, float aspect, float near, float far) {
            float f = 1.0f / tan(fovy * 0.5f);
            mat4 result = simd_diagonal_matrix(simd_make_float4(f / aspect, f, (far + near) / (near - far), 0.0f));
            result.columns[2].w = -1.0f;
            result.columns[3].z = (2.0f * far * near) / (near - far);
            return result;
        }
        
        inline mat4 ortho(float left, float right, float bottom, float top, float near, float far) {
            mat4 result = matrix_identity_float4x4;
            result.columns[0].x = 2.0f / (right - left);
            result.columns[1].y = 2.0f / (top - bottom);
            result.columns[2].z = -2.0f / (far - near);
            result.columns[3].x = -(right + left) / (right - left);
            result.columns[3].y = -(top + bottom) / (top - bottom);
            result.columns[3].z = -(far + near) / (far - near);
            return result;
        }
        
        // Constants
        inline vec3 vec3_zero() { return simd_make_float3(0.0f, 0.0f, 0.0f); }
        inline vec3 vec3_one() { return simd_make_float3(1.0f, 1.0f, 1.0f); }
        inline vec3 vec3_up() { return simd_make_float3(0.0f, 1.0f, 0.0f); }
        inline vec3 vec3_forward() { return simd_make_float3(0.0f, 0.0f, -1.0f); }
        inline vec3 vec3_right() { return simd_make_float3(1.0f, 0.0f, 0.0f); }
    }
    
#else
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glm/gtc/quaternion.hpp>
    #include <glm/gtc/type_ptr.hpp>
    
    namespace FinalStorm {
        // Vector types
        using vec2 = glm::vec2;
        using vec3 = glm::vec3;
        using vec4 = glm::vec4;
        
        using ivec2 = glm::ivec2;
        using ivec3 = glm::ivec3;
        using ivec4 = glm::ivec4;
        
        using uvec2 = glm::uvec2;
        using uvec3 = glm::uvec3;
        using uvec4 = glm::uvec4;
        
        // Matrix types
        using mat2 = glm::mat2;
        using mat3 = glm::mat3;
        using mat4 = glm::mat4;
        
        // Quaternion
        using quat = glm::quat;
        
        // Helper functions to match simd interface
        inline vec3 make_vec3(float x, float y, float z) {
            return vec3(x, y, z);
        }
        
        inline vec4 make_vec4(float x, float y, float z, float w) {
            return vec4(x, y, z, w);
        }
        
        inline vec4 make_vec4(const vec3& v, float w) {
            return vec4(v, w);
        }
        
        inline mat4 make_mat4(float diagonal = 1.0f) {
            return mat4(diagonal);
        }
        
        // Math operations (just forward to GLM)
        using glm::dot;
        using glm::cross;
        using glm::normalize;
        using glm::length;
        using glm::transpose;
        using glm::inverse;
        using glm::translate;
        using glm::scale;
        using glm::rotate;
        using glm::lookAt;
        using glm::perspective;
        using glm::ortho;
        
        // Constants
        inline vec3 vec3_zero() { return vec3(0.0f); }
        inline vec3 vec3_one() { return vec3(1.0f); }
        inline vec3 vec3_up() { return vec3(0.0f, 1.0f, 0.0f); }
        inline vec3 vec3_forward() { return vec3(0.0f, 0.0f, -1.0f); }
        inline vec3 vec3_right() { return vec3(1.0f, 0.0f, 0.0f); }
    }
#endif

// Common math utilities
namespace FinalStorm {
    constexpr float PI = 3.14159265358979323846f;
    constexpr float TWO_PI = 2.0f * PI;
    constexpr float HALF_PI = 0.5f * PI;
    constexpr float DEG_TO_RAD = PI / 180.0f;
    constexpr float RAD_TO_DEG = 180.0f / PI;
    
    inline float radians(float degrees) {
        return degrees * DEG_TO_RAD;
    }
    
    inline float degrees(float radians) {
        return radians * RAD_TO_DEG;
    }
    
    inline float clamp(float value, float min, float max) {
        return value < min ? min : (value > max ? max : value);
    }
    
    inline float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }
    
    inline vec3 lerp(const vec3& a, const vec3& b, float t) {
        return a + (b - a) * t;
    }
}