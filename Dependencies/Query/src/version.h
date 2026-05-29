#ifndef VERSION_H
#define VERSION_H

#ifndef VER_REVISION
#define VER_REVISION 1
#endif
#ifndef VER_REVISION_STR
#define VER_REVISION_STR "1"
#endif

#define VER_FILEVERSION             1,0,0,VER_REVISION
#define VER_FILEVERSION_STR         "1.0.0." VER_REVISION_STR "\0"

#define VER_PRODUCTVERSION          1,0,0,VER_REVISION
#define VER_PRODUCTVERSION_STR      "1.0.0." VER_REVISION_STR "\0"

#define VER_COMPANYNAME_STR         "Azimuth Software"
#define VER_FILEDESCRIPTION_STR     "Query library"
#define VER_INTERNALNAME_STR        "Http Query library"
#define VER_LEGALCOPYRIGHT_STR      "Copyright © 2023 Azimuth"
#define VER_LEGALTRADEMARKS1_STR    "All Rights Reserved"
#define VER_LEGALTRADEMARKS2_STR    VER_LEGALTRADEMARKS1_STR
#define VER_ORIGINALFILENAME_STR    "lib_Query.dll"
#define VER_PRODUCTNAME_STR         "HttpQuery library"

#define VER_COMPANYDOMAIN_STR       "bramtech.ru"

#ifndef WIN32
    constexpr volatile const char _libQueryVersionStrData[] __attribute__((section(".bram.version"))) = VER_FILEVERSION_STR;
#endif

#endif // VERSION_H
