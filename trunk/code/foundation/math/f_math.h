#if !defined(__F_MATH_H_)
#define __F_MATH_H_

#include "kernel/kernel.h"

namespace Foundation
{
    namespace Math
    {
        /** Return the highest argument passed in.
        */
        template<class myType>
        myType f_max(myType _uFirst, myType _uSecond)
        {
            if (_uFirst > _uSecond)
                return _uFirst;
            else
                return _uSecond;
        }
        /** Return the lowest argument passed in.
        */
        template<class T>
        T f_min(T _uFirst, T _uSecond)
        {
            if (_uFirst < _uSecond)
                return _uFirst;
            else
                return _uSecond;
        }
        /** Return Value within the exclusive range of LowerBound and UpperBound.
        */
        template<class T>
        T f_clamp(T _uValue, T _uLowerBound, T _uUpperBound)
        {
            if (_uValue > _uUpperBound)
                return _uUpperBound;
            else if (_uValue < _uLowerBound)
                return _uLowerBound;
            else
                return _uValue;
        }
    };
};

#endif  //__F_MATH_H_
