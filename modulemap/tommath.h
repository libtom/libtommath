/*
* It's necessary for targets which come from package dependencies (C libraries).
* an additional header search path argument `-I /path/to/this/directory`
* will automatically added to the include directory.
*
* This allows projects to use #include <tommath.h> syntax which are not within the package.
*/

#include "../tommath.h"
