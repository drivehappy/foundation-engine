#include "py_gmtl.h"

// Pickle Vector
tuple vector_pickle_suite::getinitargs(const gmtlVector3_PythonWrapper& vector)
{
    return make_tuple(vector[gmtl::Xelt], vector[gmtl::Yelt], vector[gmtl::Zelt]);
}

tuple vector_pickle_suite::getstate(const gmtlVector3_PythonWrapper& vector)
{
    return make_tuple(vector[gmtl::Xelt], vector[gmtl::Yelt], vector[gmtl::Zelt]);
}

void vector_pickle_suite::setstate(gmtlVector3_PythonWrapper& vector, tuple state)
{
    if (len(state) != 3) {
        PyErr_SetObject(PyExc_ValueError,
                        ("expected 3-item tuple in call to __setstate__; got %s"
                        % state).ptr()
            );
        throw_error_already_set();
    }

    float nX = float(extract<float>(state[0]));
    float nY = float(extract<float>(state[1]));
    float nZ = float(extract<float>(state[2]));

    vector[gmtl::Xelt] = nX;
    vector[gmtl::Yelt] = nY;
    vector[gmtl::Zelt] = nZ;
}

//
gmtlVector2_PythonWrapper::gmtlVector2_PythonWrapper()
    : gmtl::Vec2f() 
{
    // do nothing
}

gmtlVector2_PythonWrapper::gmtlVector2_PythonWrapper(const gmtl::Vec2f & _Vec)
    : gmtl::Vec2f(_Vec)
{
    // do nothing
}

gmtlVector2_PythonWrapper::gmtlVector2_PythonWrapper(float x, float y)
    : gmtl::Vec2f(x, y)
{
    // do nothing
}

float gmtlVector2_PythonWrapper::get(int key)
{
    return operator[](key);
}

float gmtlVector2_PythonWrapper::length()
{
    return gmtl::length((*this));
}

void gmtlVector2_PythonWrapper::normalize()
{
    gmtl::normalize((*this));
}

void gmtlVector2_PythonWrapper::set(int key, float value)
{
    operator[](key) = value;
}

gmtlVector2_PythonWrapper gmtlVector2_PythonWrapper::operator-(gmtlVector2_PythonWrapper _uRHS)
{
    return gmtlVector2_PythonWrapper((*(gmtl::Vec2f *)this) - (gmtl::Vec2f)_uRHS);
}

gmtlVector2_PythonWrapper gmtlVector2_PythonWrapper::operator+(gmtlVector2_PythonWrapper _uRHS)
{
    return gmtlVector2_PythonWrapper((*(gmtl::Vec2f *)this) + (gmtl::Vec2f)_uRHS);
}

gmtlVector2_PythonWrapper gmtlVector2_PythonWrapper::operator/(float _uRHS)
{
    return gmtlVector2_PythonWrapper((*(gmtl::Vec2f *)this) / _uRHS);
}

gmtlVector2_PythonWrapper gmtlVector2_PythonWrapper::operator*(float _uRHS)
{
    return gmtlVector2_PythonWrapper((*(gmtl::Vec2f *)this) * _uRHS);
}

//
gmtlVector3_PythonWrapper::gmtlVector3_PythonWrapper()
    : gmtl::Vec3f() 
{
    // do nothing
}

gmtlVector3_PythonWrapper::gmtlVector3_PythonWrapper(const gmtl::Vec3f & _Vec)
    : gmtl::Vec3f(_Vec)
{
    // do nothing
}

gmtlVector3_PythonWrapper::gmtlVector3_PythonWrapper(float x, float y, float z)
    : gmtl::Vec3f(x, y, z)
{
    // do nothing
}

float gmtlVector3_PythonWrapper::get(/*const gmtlVector_PythonWrapper& self, */int key)
{
    return operator[](key);
}

float gmtlVector3_PythonWrapper::length()
{
    return gmtl::length((*this));
}

void gmtlVector3_PythonWrapper::normalize()
{
    gmtl::normalize((*this));
}

void gmtlVector3_PythonWrapper::set(int key, float value)
{
    operator[](key) = value;
}


gmtlVector3_PythonWrapper gmtlVector3_PythonWrapper::operator-(gmtlVector3_PythonWrapper _uRHS)
{
    return gmtlVector3_PythonWrapper((*(gmtl::Vec3f *)this) - (gmtl::Vec3f)_uRHS);
}

gmtlVector3_PythonWrapper gmtlVector3_PythonWrapper::operator+(gmtlVector3_PythonWrapper _uRHS)
{
    return gmtlVector3_PythonWrapper((*(gmtl::Vec3f *)this) + (gmtl::Vec3f)_uRHS);
}

gmtlVector3_PythonWrapper gmtlVector3_PythonWrapper::operator/(float _uRHS)
{
    return gmtlVector3_PythonWrapper((*(gmtl::Vec3f *)this) / _uRHS);
}

gmtlVector3_PythonWrapper gmtlVector3_PythonWrapper::operator*(float _uRHS)
{
    return gmtlVector3_PythonWrapper((*(gmtl::Vec3f *)this) * _uRHS);
}

//
gmtlVector4_PythonWrapper::gmtlVector4_PythonWrapper()
    : gmtl::Vec4f() 
{
    // do nothing
}

gmtlVector4_PythonWrapper::gmtlVector4_PythonWrapper(const gmtl::Vec4f & _Vec)
    : gmtl::Vec4f(_Vec)
{
    // do nothing
}

gmtlVector4_PythonWrapper::gmtlVector4_PythonWrapper(float x, float y, float z, float w)
    : gmtl::Vec4f(x, y, z, w)
{
    // do nothing
}

float gmtlVector4_PythonWrapper::get(/*const gmtlVector_PythonWrapper& self, */int key)
{
    return operator[](key);
}

float gmtlVector4_PythonWrapper::length()
{
    return gmtl::length((*this));
}

void gmtlVector4_PythonWrapper::normalize()
{
    gmtl::normalize((*this));
}

void gmtlVector4_PythonWrapper::set(int key, float value)
{
    operator[](key) = value;
}

gmtlVector4_PythonWrapper gmtlVector4_PythonWrapper::operator-(gmtlVector4_PythonWrapper _uRHS)
{
    return gmtlVector4_PythonWrapper((*(gmtl::Vec4f *)this) - (gmtl::Vec4f)_uRHS);
}

gmtlVector4_PythonWrapper gmtlVector4_PythonWrapper::operator+(gmtlVector4_PythonWrapper _uRHS)
{
    return gmtlVector4_PythonWrapper((*(gmtl::Vec4f *)this) + (gmtl::Vec4f)_uRHS);
}

gmtlVector4_PythonWrapper gmtlVector4_PythonWrapper::operator/(float _uRHS)
{
    return gmtlVector4_PythonWrapper((*(gmtl::Vec4f *)this) / _uRHS);
}

gmtlVector4_PythonWrapper gmtlVector4_PythonWrapper::operator*(float _uRHS)
{
    return gmtlVector4_PythonWrapper((*(gmtl::Vec4f *)this) * _uRHS);
}

// Pickle Quaternion
tuple quaternion_pickle_suite::getinitargs(const gmtlQuaternion_PythonWrapper& quaternion)
{
    return make_tuple(quaternion[gmtl::Welt], quaternion[gmtl::Xelt], quaternion[gmtl::Yelt], quaternion[gmtl::Zelt]);
}

tuple quaternion_pickle_suite::getstate(const gmtlQuaternion_PythonWrapper& quaternion)
{
    return make_tuple(quaternion[gmtl::Welt], quaternion[gmtl::Xelt], quaternion[gmtl::Yelt], quaternion[gmtl::Zelt]);
}

void quaternion_pickle_suite::setstate(gmtlQuaternion_PythonWrapper& quaternion, tuple state)
{
    if (len(state) != 4) {
        PyErr_SetObject(PyExc_ValueError,
                        ("expected 4-item tuple in call to __setstate__; got %s"
                        % state).ptr()
            );
        throw_error_already_set();
    }

    float nW = float(extract<float>(state[0]));
    float nX = float(extract<float>(state[1]));
    float nY = float(extract<float>(state[2]));
    float nZ = float(extract<float>(state[3]));

    quaternion[gmtl::Welt] = nW;
    quaternion[gmtl::Xelt] = nX;
    quaternion[gmtl::Yelt] = nY;
    quaternion[gmtl::Zelt] = nZ;
}

//
gmtlQuaternion_PythonWrapper::gmtlQuaternion_PythonWrapper()
    : gmtl::Quatf()
{
    // do nothing
}

gmtlQuaternion_PythonWrapper::gmtlQuaternion_PythonWrapper(const gmtl::Quatf & _Vec)
    : gmtl::Quatf(_Vec)
{
    // do nothing
}

gmtlQuaternion_PythonWrapper::gmtlQuaternion_PythonWrapper(float x, float y, float z, float w)
    : gmtl::Quatf(x, y, z, w)
{
    // do nothing
}

float gmtlQuaternion_PythonWrapper::get(int key)
{
    return operator[](key);
}

float gmtlQuaternion_PythonWrapper::getAngleY()
{
    float angleq;

    angleq = acos(this->mData[3]) * 2.0f;                 //quaternion to axis to angle converion
    angleq = gmtl::Math::rad2Deg(angleq);

    if (this->mData[1] < 0.0f)
        angleq = 360.0f - angleq;

    return angleq;
}

gmtlQuaternion_PythonWrapper gmtlQuaternion_PythonWrapper::rotate(gmtlVector3_PythonWrapper & _nEulerAngle)
{
    gmtl::EulerAngleXYZf myEuler(_nEulerAngle[gmtl::Xelt], _nEulerAngle[gmtl::Yelt], _nEulerAngle[gmtl::Zelt]);
    gmtl::Quatf myQuat;

    myQuat = gmtl::make<gmtl::Quatf>( myEuler );
    
    return gmtl::Quatf(*this) * myQuat;
}

gmtlQuaternion_PythonWrapper gmtlQuaternion_PythonWrapper::normalize()
{
	return gmtl::normalize((*this));
}
            
gmtlQuaternion_PythonWrapper gmtlQuaternion_PythonWrapper::operator*(gmtlQuaternion_PythonWrapper _uRHS)
{
    return gmtl::Quatf(*this) * _uRHS;
}

gmtlQuaternion_PythonWrapper gmtlQuaternion_PythonWrapper::operator+(gmtlQuaternion_PythonWrapper _uRHS)
{
    return gmtl::Quatf(*this) + _uRHS;
}

gmtlQuaternion_PythonWrapper gmtlQuaternion_PythonWrapper::operator-(gmtlQuaternion_PythonWrapper _uRHS)
{
    return gmtl::Quatf(*this) - _uRHS;
}
