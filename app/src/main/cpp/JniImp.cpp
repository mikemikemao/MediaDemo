//
// Created by pirate on 2022/7/5.
//
#include "jni.h"
#include "utils/LogUtil.h"
#include "Cam2Wrapper.h"
#include "MediaRecorderContext.h"
#define NATIVE_CAM_CLASS_NAME "com/hikvision/jni/MyCam"

#ifdef __cplusplus
extern "C" {
#endif

Cam2Wrapper cam2Wrapper;

/*
  * Class:     com_hikvision_jni_MyCam
  * Method:    startPreview
  * Signature: (Landroid/view/SurfaceHolder;)V
  */
 JNIEXPORT void JNICALL startPreview(JNIEnv *env, jobject instance,jclass surfaceObj)
 {
     LOGCATE("startPreview");
     int ret = 0;
	 ret = cam2Wrapper.openCamera(TEMPLATE_PREVIEW);
	 if (ret!=0){
		 LOGCATE("openCamera failed ret =%d\n",ret);
		 return;
	 }
	 cam2Wrapper.startPreview(env,surfaceObj);

 }

 /*
  * Class:     com_hikvision_jni_MyCam
  * Method:    stopPreview
  * Signature: ()V
  */
 JNIEXPORT void JNICALL stopPreview(JNIEnv *env, jobject instance)
 {
     LOGCATE("stopPreview");
	 cam2Wrapper.closeCamera();
 }


/*
* Class:     com_hikvision_jni_MyCam
* Method:    native_CreateContext
* Signature: ()V
*/
JNIEXPORT void JNICALL native_CreateContext(JNIEnv *env, jobject instance)
{
    MediaRecorderContext::CreateContext(env, instance);
}



/*
* Class:     com_hikvision_jni_MyCam
* Method:    native_DestroyContext
* Signature: ()V
*/
JNIEXPORT void JNICALL native_DestroyContext(JNIEnv *env, jobject instance)
{
	MediaRecorderContext::DeleteContext(env, instance);
}

/*
* Class:     com_hikvision_jni_MyCam
* Method:    native_Init
* Signature: ()V
*/
JNIEXPORT int JNICALL native_Init(JNIEnv *env, jobject instance)
{
	MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, instance);
	if(pContext) return pContext->Init();
	return 0;
}


/*
* Class:     com_hikvision_jni_MyCam
* Method:    native_UnInit
* Signature: ()V
*/
JNIEXPORT int JNICALL native_UnInit(JNIEnv *env, jobject instance)
{
	MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, instance);
	if(pContext) return pContext->UnInit();
	return 0;
}

/*
* Class:     com_hikvision_jni_MyCam
* Method:    native_OnPreviewFrame
* Signature: ()V
*/
JNIEXPORT void JNICALL native_OnPreviewFrame(JNIEnv *env,
											 jobject instance,
											 jint format,
											 jbyteArray data,
											 jint width,
											 jint height) {
	int len = env->GetArrayLength (data);
	unsigned char* buf = new unsigned char[len];
	env->GetByteArrayRegion(data, 0, len, reinterpret_cast<jbyte*>(buf));
	MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, instance);
	if(pContext) pContext->OnPreviewFrame(format, buf, width, height);
	delete[] buf;
}


/*
* Class:     com_hikvision_jni_MyCam
* Method:    native_OnSurfaceCreated
* Signature: ()V
*/
JNIEXPORT void JNICALL native_OnSurfaceCreated(JNIEnv *env, jobject instance) {
	MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, instance);
	if(pContext) pContext->OnSurfaceCreated();
}


/*
* Class:     com_hikvision_jni_MyCam
* Method:    native_OnSurfaceChanged
* Signature: ()V
*/
JNIEXPORT void JNICALL native_OnSurfaceChanged(JNIEnv *env, jobject instance,jint width,jint height) {
	MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, instance);
	if(pContext) pContext->OnSurfaceChanged(width, height);
}


/*
* Class:     com_hikvision_jni_MyCam
* Method:    native_OnDrawFrame
* Signature: ()V
*/
JNIEXPORT void JNICALL native_OnDrawFrame(JNIEnv *env, jobject instance) {
	MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, instance);
	if(pContext) pContext->OnDrawFrame();
}




#ifdef __cplusplus
}
#endif

static JNINativeMethod g_RenderMethods[] = {
		{"native_startPreview",                      "(Landroid/view/Surface;)V",       (void *)(startPreview)},
		{"native_stopPreview",                       "()V",                             (void *)(stopPreview)},
		{"native_CreateContext",                     "()V",                             (void *)(native_CreateContext)},
		{"native_DestroyContext",                    "()V",                             (void *)(native_DestroyContext)},
		{"native_Init",                              "()I",                             (void *)(native_Init)},
		{"native_UnInit",                            "()I",                             (void *)(native_UnInit)},
		{"native_OnPreviewFrame",                    "(I[BII)V",                        (void *)(native_OnPreviewFrame)},
		{"native_OnSurfaceCreated",                  "()V",                             (void *)(native_OnSurfaceCreated)},
		{"native_OnSurfaceChanged",                  "(II)V",                           (void *)(native_OnSurfaceChanged)},
		{"native_OnDrawFrame",                       "()V",                             (void *)(native_OnDrawFrame)},

};


static int RegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *methods, int methodNum)
{
	LOGCATE("RegisterNativeMethods");
	jclass clazz = env->FindClass(className);
	if (clazz == NULL)
	{
		LOGCATE("RegisterNativeMethods fail. clazz == NULL");
		return JNI_FALSE;
	}
	if (env->RegisterNatives(clazz, methods, methodNum) < 0)
	{
		LOGCATE("RegisterNativeMethods fail");
		return JNI_FALSE;
	}
	return JNI_TRUE;
}

static void UnregisterNativeMethods(JNIEnv *env, const char *className)
{
	LOGCATE("UnregisterNativeMethods");
	jclass clazz = env->FindClass(className);
	if (clazz == NULL)
	{
		LOGCATE("UnregisterNativeMethods fail. clazz == NULL");
		return;
	}
	if (env != NULL)
	{
		env->UnregisterNatives(clazz);
	}
}

// call this func when loading lib
extern "C" jint JNI_OnLoad(JavaVM *jvm, void *p)
{
	LOGCATE("===== JNI_OnLoad =====");
	jint jniRet = JNI_ERR;
	JNIEnv *env = NULL;
	if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK)
	{
		return jniRet;
	}

	jint regRet = RegisterNativeMethods(env, NATIVE_CAM_CLASS_NAME, g_RenderMethods,
										sizeof(g_RenderMethods) /
										sizeof(g_RenderMethods[0]));
	if (regRet != JNI_TRUE)
	{
		return JNI_ERR;
	}
	return JNI_VERSION_1_6;
}

extern "C" void JNI_OnUnload(JavaVM *jvm, void *p)
{
	JNIEnv *env = NULL;
	if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK)
	{
		return;
	}
	UnregisterNativeMethods(env, NATIVE_CAM_CLASS_NAME);
}
