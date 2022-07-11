//
// Created by pirate on 2022/7/5.
//
#include "jni.h"
#include "utils/LogUtil.h"
#include "Cam2Wrapper.h"
#include "MyGLRenderContext.h"
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
 * Method:    previewFrame
 * Signature: ()V
 */
JNIEXPORT void JNICALL setImageData(JNIEnv *env,
									jobject instance,
									jint format,
									jbyteArray data,
									jint width,
									jint height)
{
	LOGCATE("setImageData");
	int len = env->GetArrayLength (data);
	unsigned char* buf = new unsigned char[len];
	env->GetByteArrayRegion(data, 0, len, reinterpret_cast<jbyte*>(buf));
	MyGLRenderContext::GetInstance()->SetImageData(format, width, height, buf);
    MyGLRenderContext::GetInstance()->OnDrawFrame();
	delete[] buf;
	env->DeleteLocalRef(data);
}


/*
 * Class:     com_hikvision_jni_MyCam
 * Method:    previewFrame
 * Signature: ()V
 */
JNIEXPORT void JNICALL surfaceCreated(JNIEnv *env, jclass instance)
{
	MyGLRenderContext::GetInstance()->OnSurfaceCreated();
}

/*
 * Class:     com_hikvision_jni_MyCam
 * Method:    previewFrame
 * Signature: ()V
 */
JNIEXPORT void JNICALL surfaceChanged(JNIEnv *env, jclass instance,jint width,jint height)
{
	MyGLRenderContext::GetInstance()->OnSurfaceChanged(width, height);
}

/*
 * Class:     com_hikvision_jni_MyCam
 * Method:    previewFrame
 * Signature: ()V
 */
JNIEXPORT void JNICALL drawFrame(JNIEnv *env, jclass instance)
{
	MyGLRenderContext::GetInstance()->OnDrawFrame();
}


#ifdef __cplusplus
}
#endif

static JNINativeMethod g_RenderMethods[] = {
		{"native_startPreview",                      "(Landroid/view/Surface;)V",       (void *)(startPreview)},
		{"native_stopPreview",                       "()V",                             (void *)(stopPreview)},
		{"native_setImageData",                       "(I[BII)V",                       (void *)(setImageData)},
		{"native_surfaceCreated",                       "()V",                        (void *)(surfaceCreated)},
		{"native_surfaceChanged",                       "(II)V",                        (void *)(surfaceChanged)},
		{"native_drawFrame",                             "()V",                        (void *)(drawFrame)},

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
