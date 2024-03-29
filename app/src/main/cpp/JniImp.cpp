//
// Created by pirate on 2022/7/5.
//
#include <egl/EglCore.h>
#include <render/samples/MyRender.h>
#include "jni.h"
#include "utils/LogUtil.h"
#include "Cam2Wrapper.h"
#include "MediaRecorderContext.h"
#define NATIVE_CAM_CLASS_NAME "com/hikvision/jni/MyCam"
#define NATIVE_AUDIO_CLASS_NAME "com/hikvision/jni/MyAudio"

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
JNIEXPORT void JNICALL native_CreateContext(JNIEnv *env, jobject instance,jint renderType)
{
    MediaRecorderContext::CreateContext(env, instance,(GL_RENDER_TYPE)renderType);
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
	env->DeleteLocalRef(data);
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

/*
* Class:     com_hikvision_jni_MyCam
* Method:    native_StartRecord
* Signature: ()V
*/
JNIEXPORT int JNICALL native_StartRecord(JNIEnv *env,
                                          jobject instance,
                                          jint recorder_type,
                                          jstring out_url,
                                          jint frame_width,
                                          jint frame_height,
                                          jlong video_bit_rate,
                                          jint fps) {
	int ret =0;
    const char* url = env->GetStringUTFChars(out_url, nullptr);
    MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, instance);
    if(pContext){
    	LOGCATD("url = %s",url);
		ret = pContext->StartRecord(recorder_type, url, frame_width, frame_height, video_bit_rate, fps);
    }
	env->ReleaseStringUTFChars(out_url, url);
    return ret;
}


/*
* Class:     com_hikvision_jni_MyCam
* Method:    native_StartRecord
* Signature: ()V
*/
JNIEXPORT void JNICALL native_SetSurface(JNIEnv *env,jobject instance,jclass surfaceObj) {
	MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, instance);
	//if(pContext)  pContext->SetSurface(env,surfaceObj);
}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_SetParamsInt
 * Signature: (III)V
 */
JNIEXPORT void JNICALL native_SetParamsInt(JNIEnv *env, jobject instance, jint paramType, jint value0, jint value1)
{
	MediaRecorderContext *pContext = MediaRecorderContext::GetContext(env, instance);
	if(pContext) return pContext->SetParamsInt(paramType, value0, value1);
}


#define YUV_WIDTH   1920
#define YUV_HEIGHT  1080
/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_SetParamsInt
 * Signature: (III)V
 */
JNIEXPORT void JNICALL native_EncodeTest(JNIEnv *env, jobject instance)
{
    EglCore* eglEnv = new EglCore();

    eglEnv->init(EGL_NO_CONTEXT, FLAG_TRY_GLES3);
    usleep(5000000);

	SingleVideoRecorder * pcEnc = new SingleVideoRecorder("/sdcard/test.h264",YUV_WIDTH, YUV_HEIGHT, 3000000, 20);
	pcEnc->StartRecord();

	ANativeWindow* anw = pcEnc->getInputSurface();

	EGLSurface windSurface = eglEnv->createWindowSurface(anw);
    GO_CHECK_GL_ERROR();
	eglEnv->makeCurrent(windSurface, windSurface);
	GO_CHECK_GL_ERROR();

    MyRender* render = new MyRender();
    render->Init();
    int frameid = 0;


	while (true)
	{
		usleep(200000);
		LOGCATD("frame id = %d", frameid++);
		render->Draw(YUV_WIDTH, YUV_HEIGHT);
		//eglEnv->setPresentationTime(windSurface, (EGLnsecsANDROID)systemTime(SYSTEM_TIME_MONOTONIC));
		eglEnv->swapBuffers(windSurface);
		//pcEnc->EncOutputProc();
	}

}



#ifdef __cplusplus
}
#endif

//视频预览相关
static JNINativeMethod g_RenderMethods[] = {
		{"native_startPreview",                      "(Landroid/view/Surface;)V",       (void *)(startPreview)},
		{"native_stopPreview",                       "()V",                             (void *)(stopPreview)},
		{"native_CreateContext",                     "(I)V",                            (void *)(native_CreateContext)},
		{"native_DestroyContext",                    "()V",                             (void *)(native_DestroyContext)},
		{"native_Init",                              "()I",                             (void *)(native_Init)},
		{"native_UnInit",                            "()I",                             (void *)(native_UnInit)},
		{"native_OnPreviewFrame",                    "(I[BII)V",                        (void *)(native_OnPreviewFrame)},
		{"native_OnSurfaceCreated",                  "()V",                             (void *)(native_OnSurfaceCreated)},
		{"native_OnSurfaceChanged",                  "(II)V",                           (void *)(native_OnSurfaceChanged)},
		{"native_OnDrawFrame",                       "()V",                             (void *)(native_OnDrawFrame)},
		{"native_StartRecord",                       "(ILjava/lang/String;IIJI)I",      (void *)(native_StartRecord)},
		{"native_SetSurface",                        "(Landroid/view/Surface;)V",       (void *)(native_SetSurface)},
		{"native_SetParamsInt",                      "(III)V",                          (void *)(native_SetParamsInt)},
		{"native_EncodeTest",                        "()V",                             (void *)(native_EncodeTest)},

};

//mediacodec相关
static JNINativeMethod g_AudioMethods[] = {



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
	//视频预览相关操作
	jint regRet = RegisterNativeMethods(env, NATIVE_CAM_CLASS_NAME, g_RenderMethods,
										sizeof(g_RenderMethods) /
										sizeof(g_RenderMethods[0]));
	if (regRet != JNI_TRUE)
	{
		return JNI_ERR;
	}
	//mediacodec相关操作
	regRet = RegisterNativeMethods(env, NATIVE_AUDIO_CLASS_NAME, g_AudioMethods,
										sizeof(g_AudioMethods) /
										sizeof(g_AudioMethods[0]));
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
	UnregisterNativeMethods(env, NATIVE_AUDIO_CLASS_NAME);
}
