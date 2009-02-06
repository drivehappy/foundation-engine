#if !defined(_PY__F_GMTL_H_)
#define _PY__F_GMTL_H_

// Foundation
#include "kernel/kernel.h"

// Boost
#include <boost/python.hpp>
using namespace boost::python;

namespace Foundation {

    /**
     */
    class gmtlVector2_PythonWrapper : public gmtl::Vec2f {
        public:
            gmtlVector2_PythonWrapper();
            gmtlVector2_PythonWrapper(const gmtl::Vec2f & _Vec);
            gmtlVector2_PythonWrapper(float x, float y);

            float get(int key);
            void set(int key, float value);

            float length();
            void normalize();

            gmtlVector2_PythonWrapper operator-(gmtlVector2_PythonWrapper _uRHS);
            gmtlVector2_PythonWrapper operator+(gmtlVector2_PythonWrapper _uRHS);
            gmtlVector2_PythonWrapper operator/(float _uRHS);
            gmtlVector2_PythonWrapper operator*(float _uRHS);
    };
    /**
     */
    class gmtlVector3_PythonWrapper : public gmtl::Vec3f {
        public:
            gmtlVector3_PythonWrapper();
            gmtlVector3_PythonWrapper(const gmtl::Vec3f & _Vec);
            gmtlVector3_PythonWrapper(float x, float y, float z);

            float get(int key);
            void set(int key, float value);

            float length();
            void normalize();

            gmtlVector3_PythonWrapper operator-(gmtlVector3_PythonWrapper _uRHS);
            gmtlVector3_PythonWrapper operator+(gmtlVector3_PythonWrapper _uRHS);
            gmtlVector3_PythonWrapper operator/(float _uRHS);
            gmtlVector3_PythonWrapper operator*(float _uRHS);
    };
    /**
     */
    class gmtlVector4_PythonWrapper : public gmtl::Vec4f {
        public:
            gmtlVector4_PythonWrapper();
            gmtlVector4_PythonWrapper(const gmtl::Vec4f & _Vec);
            gmtlVector4_PythonWrapper(float x, float y, float z, float w);

            float get(int key);
            void set(int key, float value);

            float length();
            void normalize();

            gmtlVector4_PythonWrapper operator-(gmtlVector4_PythonWrapper _uRHS);
            gmtlVector4_PythonWrapper operator+(gmtlVector4_PythonWrapper _uRHS);
            gmtlVector4_PythonWrapper operator/(float _uRHS);
            gmtlVector4_PythonWrapper operator*(float _uRHS);
    };

    /**
     */
    struct vector_pickle_suite : public pickle_suite {
        static tuple getinitargs(const gmtlVector3_PythonWrapper& vector);
        static tuple getstate(const gmtlVector3_PythonWrapper& vector);
        static void setstate(gmtlVector3_PythonWrapper& vector, tuple state);
    };

    /**
     */
    class gmtlQuaternion_PythonWrapper : public gmtl::Quatf {
        public:
            gmtlQuaternion_PythonWrapper();
            gmtlQuaternion_PythonWrapper(const gmtl::Quatf & _Quat);
            gmtlQuaternion_PythonWrapper(float x, float y, float z, float w);

            float get(int key);
            float getAngleY();

            gmtlQuaternion_PythonWrapper rotate(gmtlVector3_PythonWrapper & _nEulerAngle);
			gmtlQuaternion_PythonWrapper normalize();
            
            gmtlQuaternion_PythonWrapper operator*(gmtlQuaternion_PythonWrapper _uRHS);
            gmtlQuaternion_PythonWrapper operator+(gmtlQuaternion_PythonWrapper _uRHS);
            gmtlQuaternion_PythonWrapper operator-(gmtlQuaternion_PythonWrapper _uRHS);
    };

    /**
     */
    struct quaternion_pickle_suite : public pickle_suite {
        static tuple getinitargs(const gmtlQuaternion_PythonWrapper& quaternion);
        static tuple getstate(const gmtlQuaternion_PythonWrapper& quaternion);
        static void setstate(gmtlQuaternion_PythonWrapper& quaternion, tuple state);
    };

};

#endif  // _PY__F_GMTL_H_
