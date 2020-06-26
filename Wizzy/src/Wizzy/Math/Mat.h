#pragma once

#include "Vec.h"

#define PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481

namespace Wizzy {

    template <typename value_t>
    value_t to_radians(value_t deg) {
        return deg * PI / (value_t)180;
    }
    template <typename value_t>
    value_t to_degrees(value_t rad) {
        return rad * (value_t)180 / PI;
    }

    template <typename value_t>
    struct mat4 {
        typedef mat4<value_t> mat_type;
        typedef vec4<value_t> vec4_type;
        typedef vec3<value_t> vec3_type;

        union {
            vec4_type rows[4];
            value_t data[4 * 4];
            value_t ptr [4 * 4];
            struct {
                vec4_type row1;
                vec4_type row2;
                vec4_type row3;
                union {
                    vec4_type row4;
                    vec3_type translation;
                };
            };
        };

        force_inline mat4() {
            memset(data, 0, 4 * 4 * sizeof(value_t));
            rows[0].x = rows[1].y = rows[2].z = rows[3].w = (value_t)1;
        }

        force_inline mat4(value_t diagonal) {
            memset(data, 0, 4 * 4 * sizeof(value_t));
            rows[0].x = rows[1].y = rows[2].z = rows[3].w = diagonal;
        }
        

        force_inline mat4& multiply(const mat4& other) {
            value_t new_data[16];
            for (u8 row = 0; row < 4; row++) {
                for (u8 col = 0; col < 4; col++) {
                    value_t sum = 0.0f;
                    for (u8 e = 0; e < 4; e++) {
                        sum += data[e + row * 4] * other.data[col + e * 4];
                    }
                    new_data[col + row * 4] = sum;
                }
            }
            memcpy(data, new_data, 4 * 4 * sizeof(value_t));
            return *this;
        }

        force_inline vec3_type multiply(const vec3_type& vec) const {
            return vec3_type(
                this->rows[0].x * vec.x + this->rows[0].y * vec.y
                + this->rows[0].z * vec.z + this->rows[0].w,

                this->rows[1].x * vec.x + this->rows[1].y * vec.y
                + this->rows[1].z * vec.z + this->rows[1].w,

                this->rows[2].x * vec.x + this->rows[2].y * vec.y
                + this->rows[2].z * vec.z + this->rows[2].w
            );
        }

        force_inline vec4_type multiply(const vec4_type& vec) const {
            return vec4_type(
                this->rows[0].x * vec.x + this->rows[0].y * vec.y
                + this->rows[0].z * vec.z + this->rows[0].w * vec.w,

                this->rows[1].x * vec.x + this->rows[1].y * vec.y
                + this->rows[1].z * vec.z + this->rows[1].w * vec.w,

                this->rows[2].x * vec.x + this->rows[2].y * vec.y
                + this->rows[2].z * vec.z + this->rows[2].w * vec.w,

                this->rows[3].x * vec.x + this->rows[3].y * vec.y
                + this->rows[3].z * vec.z + this->rows[3].w * vec.w
            );
        }

        force_inline friend mat_type operator*(mat_type left, const mat_type& right) {
            return left.multiply(right);
        }

        force_inline mat_type& operator*=(const mat_type& other) {
            return multiply(other);
        }

        force_inline friend vec3_type operator*(const mat_type& left, const vec3_type& right) {
            return left.multiply(right);
        }

        force_inline friend vec4_type operator*(const mat_type& left, const vec4_type& right) {
            return left.multiply(right);
        }

        force_inline mat_type& translate(const vec3_type& amount) {
            translation += amount;
            return *this;
        }

        force_inline mat_type& rotate(value_t angle, const vec3_type& axis) {
            value_t r = to_radians(angle);
            value_t c = cos(r);
            value_t s = sin(r);
            value_t omc = 1.0f - c;

            value_t x = axis.x;
            value_t y = axis.y;
            value_t z = axis.z;

            data[0 + 0 * 4] += x * x * omc + c;
            data[0 + 1 * 4] += y * x * omc + z * s;
            data[0 + 2 * 4] += x * z * omc - y * s;

            data[1 + 0 * 4] += x * y * omc - z * s;
            data[1 + 1 * 4] += y * y * omc + c;
            data[1 + 2 * 4] += y * z * omc + x * s;

            data[2 + 0 * 4] += x * z * omc + y * s;
            data[2 + 1 * 4] += y * z * omc - x * s;
            data[2 + 2 * 4] += z * z * omc + c;

            return *this;
        }

        force_inline mat_type& scale(const vec3_type& scale) {
            rows[0].x += scale.x;
            rows[1].y += scale.y;
            rows[2].z += scale.z;

            return *this;
        }

        force_inline mat_type& transpose() {
            rows[0] = vec4<value_t>(rows[0].x, rows[1].x, rows[2].x, rows[3].x);
            rows[1] = vec4<value_t>(rows[0].y, rows[1].y, rows[2].y, rows[3].y);
            rows[2] = vec4<value_t>(rows[0].z, rows[1].z, rows[2].z, rows[3].z);
            rows[3] = vec4<value_t>(rows[0].w, rows[1].w, rows[2].w, rows[3].w);

            return *this;
        }

        force_inline mat_type& invert() {
            value_t temp[16];

            temp[0] = data[5] * data[10] * data[15] -
                data[5] * data[11] * data[14] -
                data[9] * data[6] * data[15] +
                data[9] * data[7] * data[14] +
                data[13] * data[6] * data[11] -
                data[13] * data[7] * data[10];

            temp[4] = -data[4] * data[10] * data[15] +
                data[4] * data[11] * data[14] +
                data[8] * data[6] * data[15] -
                data[8] * data[7] * data[14] -
                data[12] * data[6] * data[11] +
                data[12] * data[7] * data[10];

            temp[8] = data[4] * data[9] * data[15] -
                data[4] * data[11] * data[13] -
                data[8] * data[5] * data[15] +
                data[8] * data[7] * data[13] +
                data[12] * data[5] * data[11] -
                data[12] * data[7] * data[9];

            temp[12] = -data[4] * data[9] * data[14] +
                data[4] * data[10] * data[13] +
                data[8] * data[5] * data[14] -
                data[8] * data[6] * data[13] -
                data[12] * data[5] * data[10] +
                data[12] * data[6] * data[9];

            temp[1] = -data[1] * data[10] * data[15] +
                data[1] * data[11] * data[14] +
                data[9] * data[2] * data[15] -
                data[9] * data[3] * data[14] -
                data[13] * data[2] * data[11] +
                data[13] * data[3] * data[10];

            temp[5] = data[0] * data[10] * data[15] -
                data[0] * data[11] * data[14] -
                data[8] * data[2] * data[15] +
                data[8] * data[3] * data[14] +
                data[12] * data[2] * data[11] -
                data[12] * data[3] * data[10];

            temp[9] = -data[0] * data[9] * data[15] +
                data[0] * data[11] * data[13] +
                data[8] * data[1] * data[15] -
                data[8] * data[3] * data[13] -
                data[12] * data[1] * data[11] +
                data[12] * data[3] * data[9];

            temp[13] = data[0] * data[9] * data[14] -
                data[0] * data[10] * data[13] -
                data[8] * data[1] * data[14] +
                data[8] * data[2] * data[13] +
                data[12] * data[1] * data[10] -
                data[12] * data[2] * data[9];

            temp[2] = data[1] * data[6] * data[15] -
                data[1] * data[7] * data[14] -
                data[5] * data[2] * data[15] +
                data[5] * data[3] * data[14] +
                data[13] * data[2] * data[7] -
                data[13] * data[3] * data[6];

            temp[6] = -data[0] * data[6] * data[15] +
                data[0] * data[7] * data[14] +
                data[4] * data[2] * data[15] -
                data[4] * data[3] * data[14] -
                data[12] * data[2] * data[7] +
                data[12] * data[3] * data[6];

            temp[10] = data[0] * data[5] * data[15] -
                data[0] * data[7] * data[13] -
                data[4] * data[1] * data[15] +
                data[4] * data[3] * data[13] +
                data[12] * data[1] * data[7] -
                data[12] * data[3] * data[5];

            temp[14] = -data[0] * data[5] * data[14] +
                data[0] * data[6] * data[13] +
                data[4] * data[1] * data[14] -
                data[4] * data[2] * data[13] -
                data[12] * data[1] * data[6] +
                data[12] * data[2] * data[5];

            temp[3] = -data[1] * data[6] * data[11] +
                data[1] * data[7] * data[10] +
                data[5] * data[2] * data[11] -
                data[5] * data[3] * data[10] -
                data[9] * data[2] * data[7] +
                data[9] * data[3] * data[6];

            temp[7] = data[0] * data[6] * data[11] -
                data[0] * data[7] * data[10] -
                data[4] * data[2] * data[11] +
                data[4] * data[3] * data[10] +
                data[8] * data[2] * data[7] -
                data[8] * data[3] * data[6];

            temp[11] = -data[0] * data[5] * data[11] +
                data[0] * data[7] * data[9] +
                data[4] * data[1] * data[11] -
                data[4] * data[3] * data[9] -
                data[8] * data[1] * data[7] +
                data[8] * data[3] * data[5];

            temp[15] = data[0] * data[5] * data[10] -
                data[0] * data[6] * data[9] -
                data[4] * data[1] * data[10] +
                data[4] * data[2] * data[9] +
                data[8] * data[1] * data[6] -
                data[8] * data[2] * data[5];

            value_t determinant = data[0] * temp[0] + data[1] * temp[4] + data[2] * temp[8] + data[3] * temp[12];
            determinant = 1.0f / determinant;

            for (u8 i = 0; i < 4 * 4; i++)
                data[i] = temp[i] * determinant;

            return *this;
        }
    };

    namespace transformation {
        template <typename value_t>
        mat4<value_t> translation(vec3<value_t> value) {
            mat4<value_t> mat(1.f);
            mat.translate(value);
            return mat;
        }

        template <typename value_t>
        mat4<value_t> rotation(value_t angle, vec3<value_t> axis) {
            mat4<value_t> mat(1.f);
            mat.rotate(angle, axis);
            return mat;
        }

        template <typename value_t>
        mat4<value_t> scale(vec3<value_t> value) {
            mat4<value_t> mat(1.f);
            mat.scale(value);
            return mat;
        }
    }

    namespace projection {
        template <typename value_t>
        mat4<value_t> ortho(value_t left, value_t right, value_t bottom, value_t top, value_t near_, value_t far_) {
            mat4 result(1.0f);

            result.data[0 + 0 * 4] = 2.0f / (right - left);

            result.data[1 + 1 * 4] = 2.0f / (top - bottom);

            result.data[2 + 2 * 4] = 2.0f / (near_ - far_);

            result.data[3 + 0 * 4] = (left + right) / (left - right);
            result.data[3 + 1 * 4] = (bottom + top) / (bottom - top);
            result.data[3 + 2 * 4] = (far_ + near_) / (far_ - near_);

            return result;
        }

        template <typename value_t>
        mat4<value_t> perspective(value_t fov, f32 aspectRatio, value_t near, value_t far) {
            mat4<value_t> result(1.0f);

            value_t q = 1.0f / tan(to_radians(0.5f * fov));
            value_t a = (value_t)((f32)q / aspectRatio);

            value_t b = (near + far) / (near - far);
            value_t c = (2.0f * near * far) / (near - far);

            result.data[0 + 0 * 4] = a;
            result.data[1 + 1 * 4] = q;
            result.data[2 + 2 * 4] = b;
            result.data[2 + 3 * 4] = -1.0f;
            result.data[3 + 2 * 4] = c;

            return result;
        }

        template <typename value_t>
        mat4<value_t> look_at(const vec3<value_t>& camera, const vec3<value_t>& object, const vec3<value_t>& up) {
            mat4<value_t> result = mat4<value_t>((value_t)0);
            vec3<value_t> f = (object - camera).normalize();
            vec3<value_t> s = f.cross(up.normalize());
            vec3<value_t> u = s.cross(f);

            result.data[0 + 0 * 4] = s.x;
            result.data[0 + 1 * 4] = s.y;
            result.data[0 + 2 * 4] = s.z;

            result.data[1 + 0 * 4] = u.x;
            result.data[1 + 1 * 4] = u.y;
            result.data[1 + 2 * 4] = u.z;

            result.data[2 + 0 * 4] = -f.x;
            result.data[2 + 1 * 4] = -f.y;
            result.data[2 + 2 * 4] = -f.z;

            return result * mat4<value_t>((value_t)1.f).translate(vec3<value_t>(-camera.x, -camera.y, -camera.z));
        }
    }

    template<typename TStream, typename value_t>
    inline TStream& operator<<(TStream& str, const mat4<value_t>& m) {
        return str << "mat4:\n"
               << m.rows[0] << "\n"
               << m.rows[1] << "\n"
               << m.rows[2] << "\n"
               << m.rows[3];
    }

    typedef mat4<f32> fmat4;
    typedef mat4<f64> dmat4;
    typedef mat4<s32> imat4;
}