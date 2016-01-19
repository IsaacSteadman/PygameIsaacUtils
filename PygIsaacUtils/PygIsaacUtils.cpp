// PygIsaacUtils.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "PygIsaacUtils.h"
#pragma comment(lib, "IsaacUtils.lib")
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDL_mixer.lib")

static int RwKyFl_seek(SDL_RWops *context, int offset, int whence) {
	try {
		if (offset != 0 || whence != Utils::fs::SK_CUR)
			((Utils::fs::FileBase *)context->hidden.unknown.data1)->Seek(offset, whence);
		return ((Utils::fs::FileBase *)context->hidden.unknown.data1)->Tell();
	}
	catch (...) {
		return -1;
	}
}
static int RwKyFl_read(SDL_RWops* context, void* ptr, int size, int maxnum) {
	Utils::ByteArray Rtn;
	Rtn.Take((Utils::Byte *)ptr, size * maxnum);
	unsigned long NumRead = 0;
	try{
		NumRead = ((Utils::fs::FileBase *)context->hidden.unknown.data1)->Read(Rtn);
	}
	catch (...) {
		Utils::Byte *Back;
		SizeL Len;
		Rtn.Give(Back, Len);
		return -1;
	}
	/*SizeL End = NumRead / size;
	End *= size;
	SizeL FastSz = End / sizeof(SizeL);
	SizeL *RtnDat = (SizeL *)Rtn.GetData();
	SizeL c = 0;
	for (c = 0; c < FastSz; c += sizeof(SizeL)) {
		RtnDat[c] = ((SizeL *)ptr)[c];
	}
	c *= sizeof(SizeL);
	for (SizeL c = 0; c < End; ++c) {//Added SizeL c = 0
		((Utils::Byte *)ptr)[c] = Rtn[c];
	}*/
	Utils::Byte *Back;
	SizeL Len;
	Rtn.Give(Back, Len);
	return NumRead - NumRead % size;
}
static int RwKyFl_write(SDL_RWops* context, const void* ptr, int size, int num) {
	Utils::ByteArray Tmp;
	Tmp.Take((Utils::Byte *)ptr, SizeL(size) * num);
	int Rtn = 0;
	try{
		SizeL RtnTmp = ((Utils::fs::FileBase *)context->hidden.unknown.data1)->Write(Tmp);
		Rtn = RtnTmp / size;
	}
	catch (...) {
		Rtn = -1;
	}
	Utils::Byte *Back;
	SizeL Len;
	Tmp.Give(Back, Len);
	return Rtn;
}
static int RwKyFl_close(SDL_RWops *context) {
	((Utils::fs::FileBase *)context->hidden.unknown.data1)->Close();
	if (!DelObj(context->hidden.unknown.data1))
		delete (Utils::fs::FileBase *)context->hidden.unknown.data1;
	SDL_FreeRW(context);
	return 0;
}

static SDL_RWops *RwKyFl_new(Utils::fs::FileBase *KyFl) {
	SDL_RWops &Rtn = *SDL_AllocRW();
	Rtn.hidden.unknown.data1 = KyFl;
	Rtn.read = RwKyFl_read;
	Rtn.write = RwKyFl_write;
	Rtn.seek = RwKyFl_seek;
	Rtn.close = RwKyFl_close;
	return &Rtn;
}
Mix_Music **PygCurMus = 0, **PygQueMus = 0;
static PyObject *music_IsaacLoad(PyObject *self, PyObject *args) {
	PyObject *ret = NULL;
	assert(args);
	Py_INCREF(args);

	if (!PygCurMus || !PygQueMus)
	{
		PyErr_SetString(PyExc_RuntimeError, "Module not initialized or pygame not present");
		goto except;
	}
	if (*PygCurMus) Mix_FreeMusic(*PygCurMus);
	if (*PygQueMus) Mix_FreeMusic(*PygCurMus);
	*PygCurMus = 0;
	*PygQueMus = 0;
	void *Mine = PyLong_AsVoidPtr(args);
	if (PyErr_Occurred()) return NULL;
	if (!File_tell(Mine) && UlLastError())
	{
		PyErr_SetString(PyExc_ValueError, "First argument must be a file object from IsaacUtils");
		goto except;
	}
	*PygCurMus = Mix_LoadMUS_RW(RwKyFl_new((Utils::fs::FileBase *)Mine));
	if (!*PygCurMus)
	{
		PyErr_SetString(PyExc_Exception, Mix_GetError());
		goto except;
	}


	Py_INCREF(Py_None);
	ret = Py_None;
	assert(!PyErr_Occurred());
	assert(ret);
	goto endfunc;
except:
	Py_XDECREF(args);
	ret = NULL;
endfunc:
	Py_DECREF(args);
	return ret;
}
static PyMethodDef _IsaacMethods[] =
{
	{ "KyMusLoad", music_IsaacLoad, METH_O,"KyMusLoad(void *FlObj) -> None\nLoads a IsaacUtils File object into pygame.mixer.music" },
	{ NULL, NULL, 0, NULL }
};
extern "C" {
	void initPygIsaacUtils(void) {
		PyObject *Module = Py_InitModule("PygIsaacUtils", _IsaacMethods);
		if (!Module) return;
		PyObject *music = PyImport_ImportModule("pygame.mixer_music");
		if (!music) goto except;
		PyObject *_dict = PyModule_GetDict(music);
		PyObject *ptr = PyDict_GetItemString(_dict, "_MUSIC_POINTER");
		PygCurMus = (Mix_Music **)PyCapsule_GetPointer(ptr, "pygame.music_mixer._MUSIC_POINTER");
		if (!PygCurMus) goto except1;
		ptr = PyDict_GetItemString(_dict, "_QUEUE_POINTER");
		PygQueMus = (Mix_Music **)PyCapsule_GetPointer(ptr, "pygame.music_mixer._QUEUE_POINTER");
		if (!PygQueMus) goto except1;
		Init();
		return;
	except1:
		Py_DECREF(music);
	except:
		Py_DECREF(Module);
	}
	
}
