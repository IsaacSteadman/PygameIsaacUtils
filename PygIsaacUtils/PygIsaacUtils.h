// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PYGISAACUTILS_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PYGISAACUTILS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef PYGISAACUTILS_EXPORTS
#define PYGISAACUTILS_API __declspec(dllexport)
#else
#define PYGISAACUTILS_API __declspec(dllimport)
#endif

#include <IsaacUtils.h>
#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG 1
#else
#include <Python.h>
#endif
#include <SDL.h>
#include <SDL_mixer.h>

PyMODINIT_FUNC initPygIsaacUtils(void);
