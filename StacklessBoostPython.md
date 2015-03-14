# Stackless Python with Boost Bindings #
## Setting Up ##
### Boost C++ Libraries ###
Download the latest version from: http://www.boost.org/

### Stackless Python ###
Download the latest appropriate version from: http://www.stackless.com/

## Building Under MSVC ##
Pull the Boost Python sources into your build (e.g. boost\_1\_38\_0/libs/python/src)

Include the Python include directory (e.g. Python25\include)

Include the Stackless include directory (e.g. Python25\include\Stackless)

Add: "#define BOOST\_PYTHON\_STATIC\_LIB" to boost\python\detail\config.hpp at line 62

define BOOST\_ALL\_NO\_LIB under boost\config\user.hpp: #define BOOST\_ALL\_NO\_LIB