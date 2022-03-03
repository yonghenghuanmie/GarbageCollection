#pragma once

#ifdef Export

#ifdef __cplusplus
#define declaration extern "C" __declspec(dllexport)
#else
#define declaration __declspec(dllexport)
#endif // __cplusplus

#else

#ifdef __cplusplus
#define declaration extern "C" __declspec(dllimport)
#else
#define declaration __declspec(dllimport)
#endif // __cplusplus

#endif // Export