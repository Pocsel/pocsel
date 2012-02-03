#ifndef __TOOLS_MATRIX4_HPP__
#define __TOOLS_MATRIX4_HPP__

#include <cmath>
#include "tools/sqrt.hpp"
#include "tools/Vector3.hpp"

namespace Tools {

    template<typename T>
    struct Matrix4
    {
        union
        {
            T m[4][4];
            T mm[16];
        };

        static const Matrix4<T> identity;

        static Matrix4<T> CreateTranslation(Vector3<T> const& v);
        static Matrix4<T> CreateTranslation(T tx, T ty, T tz);
        static Matrix4<T> CreateScale(Vector3<T> const& v);
        static Matrix4<T> CreateScale(T sx, T sy, T sz );
        static Matrix4<T> CreateYawPitchRollRotation(T yaw, T pitch, T roll);

        static Matrix4<T> CreateOrthographic(T left, T right, T bottom, T top, T zNearPlane, T zFarPlane);
        static Matrix4<T> CreatePerspective(T fov, T ratio, T zNearPlane, T zFarPlane);
        static Matrix4<T> CreatePerspective2(T horizontalFov, T verticalFov, T zNearPlane, T zFarPlane);
        static Matrix4<T> CreateLookAt(Vector3<T> const& eye, Vector3<T> const& center, Vector3<T> const& up);

        Matrix4() {}
        template<typename Targ>
        explicit Matrix4(Matrix4<Targ> const& matrix);
        Matrix4(T m00, T m01, T m02, T m03,
                T m10, T m11, T m12, T m13,
                T m20, T m21, T m22, T m23,
                T m30, T m31, T m32, T m33);

        Matrix4<T> operator *(Matrix4<T> const& m2) const;
        Matrix4<T> operator *(T scalar) const;
        Matrix4<T> operator +(Matrix4<T> const& m2) const;
        Matrix4<T> operator -(Matrix4<T> const& m2) const;
        bool operator ==(Matrix4<T> const& m2) const;
        bool operator !=(Matrix4<T> const& m2) const;

        Matrix4<T> Transpose(void) const;
        void Translate(Vector3<T> const& v);
        void Scale(Vector3<T> const& v);
    };

    // Implementation
    template<class T>
    const Matrix4<T> Matrix4<T>::identity(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);

    template<class T>
    Matrix4<T> Matrix4<T>::CreateTranslation(Vector3<T> const& v)
    {
        return Matrix4<T>::CreateTranslation(v.x, v.y, v.z);
    }

    template<class T>
    Matrix4<T> Matrix4<T>::CreateTranslation(T tx, T ty, T tz)
    {
        Matrix4<T> r = Matrix4<T>::identity;
        r.m[3][0] = tx;
        r.m[3][1] = ty;
        r.m[3][2] = tz;
        return r;
    }

    template<class T>
    Matrix4<T> Matrix4<T>::CreateScale(Vector3<T> const& v)
    {
        return Matrix4<T>::CreateScale(v.x, v.y, v.z);
    }

    template<class T>
    Matrix4<T> Matrix4<T>::CreateScale(T sx, T sy, T sz )
    {
        Matrix4<T> r = Matrix4<T>::identity;
        r.m[0][0] = sx;
        r.m[1][1] = sy;
        r.m[2][2] = sz;
        return r;
    }

    template<class T>
    Matrix4<T> Matrix4<T>::CreateYawPitchRollRotation(T yaw, T pitch, T roll)
    {
        T num2(std::sin(roll / 2));
        T num3(std::cos(roll / 2));

        T num5(std::sin(pitch / 2));
        T num6(std::cos(pitch / 2));

        T num8(std::sin(yaw / 2));
        T num9(std::cos(yaw / 2));

        T x = num9 * num5 * num3 + num8 * num6 * num2;
        T y = num8 * num6 * num3 - num9 * num5 * num2;
        T z = num9 * num6 * num2 - num8 * num5 * num3;
        T w = num9 * num6 * num3 + num8 * num5 * num2;

        T xx = x * x;
        T yy = y * y;
        T zz = z * z;
        T xy = x * y;
        T zw = z * w;
        T zx = z * x;
        T yw = y * w;
        T yz = y * z;
        T xw = x * w;

        Matrix4<T> r;
        r.m[0][0] = 1 - 2 * (yy + zz);
        r.m[0][1] = 2 * (xy + zw);
        r.m[0][2] = 2 * (zx - yw);
        r.m[0][3] = 0;
        r.m[1][0] = 2 * (xy - zw);
        r.m[1][1] = 1 - 2 * (zz + xx);
        r.m[1][2] = 2 * (yz + xw);
        r.m[1][3] = 0;
        r.m[2][0] = 2 * (zx + yw);
        r.m[2][1] = 2 * (yz - xw);
        r.m[2][2] = 1 - 2 * (yy + xx);
        r.m[2][3] = 0;
        r.m[3][0] = 0;
        r.m[3][1] = 0;
        r.m[3][2] = 0;
        r.m[3][3] = 1;
        return r;
    }

    template<class T>
    Matrix4<T> Matrix4<T>::CreateOrthographic(T left, T right, T bottom, T top, T zNearPlane, T zFarPlane)
    {
        Matrix4<T> r = Matrix4<T>::identity;
        r.m[0][0] = 2 / (right - left);
        r.m[1][1] = 2 / (top - bottom);
        r.m[2][2] = 1 / (zNearPlane - zFarPlane);
        r.m[3][0] = -(left + right) / (right - left);
        r.m[3][1] = -(top + bottom) / (top - bottom);
        r.m[3][2] = (zFarPlane - zNearPlane) / (zFarPlane - zNearPlane);
        return r;
    }

    template<class T>
    Matrix4<T> Matrix4<T>::CreatePerspective(T fov, T ratio, T zNearPlane, T zFarPlane)
    {
        T rad = T(fov * 0.01745329251994329576923690768489);
        T h = T(1.0 / std::tan(T(0.5) * rad));
        T w = h / ratio;

        Matrix4<T> r;
        r.m[0][0] = w;
        r.m[0][1] = 0;
        r.m[0][2] = 0;
        r.m[0][3] = 0;

        r.m[1][0] = 0;
        r.m[1][1] = h;
        r.m[1][2] = 0;
        r.m[1][3] = 0;

        r.m[2][0] = 0;
        r.m[2][1] = 0;
        r.m[2][2] = zFarPlane / (zNearPlane - zFarPlane);
        r.m[2][3] = -T(1);

        r.m[3][0] = 0;
        r.m[3][1] = 0;
        r.m[3][2] = zFarPlane * zNearPlane / (zNearPlane - zFarPlane);
        r.m[3][3] = 0;
        return r;
    }

    template<class T>
    Matrix4<T> Matrix4<T>::CreatePerspective2(T horizontalFov, T verticalFov, T zNearPlane, T zFarPlane)
    {
        T rad = T(verticalFov * 0.01745329251994329576923690768489);
        T h = T(1.0 / std::tan(rad * T(0.5)));
        rad = T(horizontalFov * 0.01745329251994329576923690768489);
        T w = T(1.0 / std::tan(rad * T(0.5)));

        Matrix4<T> r(Matrix4<T>::identity);
        r.m[0][0] = w;
        r.m[1][1] = h;
        r.m[2][2] = (zFarPlane + zNearPlane) / (zFarPlane - zNearPlane);
        r.m[2][3] = T(1);
        r.m[3][2] = -(T(2) * zFarPlane * zNearPlane) / (zFarPlane - zNearPlane);
        return r;
    }

    template<class T>
    Matrix4<T> Matrix4<T>::CreateLookAt(Vector3<T> const& eye, Vector3<T> const& target, Vector3<T> const& upDir)
    {
        auto forward = eye - target; // vecteur qui part devant moi (avec un moins mais ça marche, ne demandez pas pourquoi x))
        forward.Normalize();
        auto left = Vector3<T>::Cross(upDir, forward); // vecteur d'un coté (ou de l'autre)
        left.Normalize();
        auto up = Vector3<T>::Cross(forward, left); // le haut de la caméra
        up.Normalize();

        Matrix4<T> r;
        r.m[0][0] = left.x;
        r.m[0][1] = up.x;
        r.m[0][2] = forward.x;
        r.m[0][3] = 0;
        r.m[1][0] = left.y;
        r.m[1][1] = up.y;
        r.m[1][2] = forward.y;
        r.m[1][3] = 0;
        r.m[2][0] = left.z;
        r.m[2][1] = up.z;
        r.m[2][2] = forward.z;
        r.m[2][3] = 0;
        //r.m[3][0] = -Vector3<T>::Dot(left, eye);
        //r.m[3][1] = -Vector3<T>::Dot(up, eye);
        //r.m[3][2] = -Vector3<T>::Dot(forward, eye);
        r.m[3][0] = 0;
        r.m[3][1] = 0;
        r.m[3][2] = 0;
        r.m[3][3] = 1;

        return r * Matrix4<T>::CreateTranslation(eye);
    }

    template<class T>
    template<typename Targ>
    Matrix4<T>::Matrix4(Matrix4<Targ> const& matrix)
    {
        m[0][0] = T(matrix.m[0][0]);
        m[0][1] = T(matrix.m[0][1]);
        m[0][2] = T(matrix.m[0][2]);
        m[0][3] = T(matrix.m[0][3]);
        m[1][0] = T(matrix.m[1][0]);
        m[1][1] = T(matrix.m[1][1]);
        m[1][2] = T(matrix.m[1][2]);
        m[1][3] = T(matrix.m[1][3]);
        m[2][0] = T(matrix.m[2][0]);
        m[2][1] = T(matrix.m[2][1]);
        m[2][2] = T(matrix.m[2][2]);
        m[2][3] = T(matrix.m[2][3]);
        m[3][0] = T(matrix.m[3][0]);
        m[3][1] = T(matrix.m[3][1]);
        m[3][2] = T(matrix.m[3][2]);
        m[3][3] = T(matrix.m[3][3]);
    }

    template<class T>
    Matrix4<T>::Matrix4(T m00, T m01, T m02, T m03,
                T m10, T m11, T m12, T m13,
                T m20, T m21, T m22, T m23,
                T m30, T m31, T m32, T m33)
    {
        m[0][0] = m00;
        m[0][1] = m01;
        m[0][2] = m02;
        m[0][3] = m03;
        m[1][0] = m10;
        m[1][1] = m11;
        m[1][2] = m12;
        m[1][3] = m13;
        m[2][0] = m20;
        m[2][1] = m21;
        m[2][2] = m22;
        m[2][3] = m23;
        m[3][0] = m30;
        m[3][1] = m31;
        m[3][2] = m32;
        m[3][3] = m33;
    }

    template<class T>
    Matrix4<T> Matrix4<T>::operator *(Matrix4<T> const& m2) const
    {
        Matrix4<T> r;

        r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
        r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
        r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
        r.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

        r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
        r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
        r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
        r.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

        r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
        r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
        r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
        r.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

        r.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
        r.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
        r.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
        r.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];

        return r;
    }

    template<class T>
    Matrix4<T> Matrix4<T>::operator *(T scalar) const
    {
        return Matrix4<T>(
            scalar * m[0][0], scalar * m[0][1], scalar * m[0][2], scalar * m[0][3],
            scalar * m[1][0], scalar * m[1][1], scalar * m[1][2], scalar * m[1][3],
            scalar * m[2][0], scalar * m[2][1], scalar * m[2][2], scalar * m[2][3],
            scalar * m[3][0], scalar * m[3][1], scalar * m[3][2], scalar * m[3][3]);
    }

    template<class T>
    Matrix4<T> Matrix4<T>::operator +(Matrix4<T> const& m2) const
    {
        Matrix4<T> r;

        r.m[0][0] = m[0][0] + m2.m[0][0];
        r.m[0][1] = m[0][1] + m2.m[0][1];
        r.m[0][2] = m[0][2] + m2.m[0][2];
        r.m[0][3] = m[0][3] + m2.m[0][3];

        r.m[1][0] = m[1][0] + m2.m[1][0];
        r.m[1][1] = m[1][1] + m2.m[1][1];
        r.m[1][2] = m[1][2] + m2.m[1][2];
        r.m[1][3] = m[1][3] + m2.m[1][3];

        r.m[2][0] = m[2][0] + m2.m[2][0];
        r.m[2][1] = m[2][1] + m2.m[2][1];
        r.m[2][2] = m[2][2] + m2.m[2][2];
        r.m[2][3] = m[2][3] + m2.m[2][3];

        r.m[3][0] = m[3][0] + m2.m[3][0];
        r.m[3][1] = m[3][1] + m2.m[3][1];
        r.m[3][2] = m[3][2] + m2.m[3][2];
        r.m[3][3] = m[3][3] + m2.m[3][3];

        return r;
    }

    template<class T>
    Matrix4<T> Matrix4<T>::operator -(Matrix4<T> const& m2) const
    {
        Matrix4<T> r;

        r.m[0][0] = m[0][0] - m2.m[0][0];
        r.m[0][1] = m[0][1] - m2.m[0][1];
        r.m[0][2] = m[0][2] - m2.m[0][2];
        r.m[0][3] = m[0][3] - m2.m[0][3];

        r.m[1][0] = m[1][0] - m2.m[1][0];
        r.m[1][1] = m[1][1] - m2.m[1][1];
        r.m[1][2] = m[1][2] - m2.m[1][2];
        r.m[1][3] = m[1][3] - m2.m[1][3];

        r.m[2][0] = m[2][0] - m2.m[2][0];
        r.m[2][1] = m[2][1] - m2.m[2][1];
        r.m[2][2] = m[2][2] - m2.m[2][2];
        r.m[2][3] = m[2][3] - m2.m[2][3];

        r.m[3][0] = m[3][0] - m2.m[3][0];
        r.m[3][1] = m[3][1] - m2.m[3][1];
        r.m[3][2] = m[3][2] - m2.m[3][2];
        r.m[3][3] = m[3][3] - m2.m[3][3];

        return r;
    }

    template<class T>
    bool Matrix4<T>::operator ==(Matrix4<T> const& m2) const
    {
        if (m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
            m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
            m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
            m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3])
            return false;
        return true;
    }

    template<class T>
    bool Matrix4<T>::operator !=(Matrix4<T> const& m2) const
    {
        if (m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
            m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
            m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
            m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3])
            return true;
        return false;
    }

    template<class T>
    Matrix4<T> Matrix4<T>::Transpose(void) const
    {
        return Matrix4<T>(
            m[0][0], m[1][0], m[2][0], m[3][0],
            m[0][1], m[1][1], m[2][1], m[3][1],
            m[0][2], m[1][2], m[2][2], m[3][2],
            m[0][3], m[1][3], m[2][3], m[3][3]);
    }

    template<class T>
    void Matrix4<T>::Translate(Vector3<T> const& v)
    {
        //slow ><
        *this = *this * Matrix4<T>::CreateTranslation(v);
        /*
        this->m[0][3] = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3];
        this->m[1][3] = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3];
        this->m[2][3] = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3];
        this->m[3][3] = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3];
        */
    }

    template<class T>
    void Matrix4<T>::Scale(Vector3<T> const& v)
    {
        this->m[0][0] *= v.x;
        this->m[0][1] *= v.x;
        this->m[0][2] *= v.z;

        this->m[1][0] *= v.x;
        this->m[1][1] *= v.x;
        this->m[1][2] *= v.z;

        this->m[2][0] *= v.x;
        this->m[2][1] *= v.x;
        this->m[2][2] *= v.z;
    }

}

#endif
