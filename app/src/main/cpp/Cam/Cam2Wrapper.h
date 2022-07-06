//
// Created by pirate on 2022/7/6.
//

#ifndef MEDIADEMO_CAM2WRAPPER_H
#define MEDIADEMO_CAM2WRAPPER_H


#include <pthread.h>
#include <android/native_window_jni.h>
#include <camera/NdkCameraDevice.h>
#include <camera/NdkCameraManager.h>
#include "utils/LogUtil.h"

class Cam2Wrapper {
public:
    Cam2Wrapper()
        :cameraDevice(NULL)
        ,captureRequest(NULL)
        ,captureSessionOutputContainer(NULL)
        ,aNativeWindow(NULL)
        ,cameraOutputTarget(NULL)
        ,sessionOutput(NULL)
        ,captureSession(NULL){}
    int openCamera(ACameraDevice_request_template templateId);
    void closeCamera();
    void startPreview(JNIEnv *jniEnv,jobject surface);
    void stopPreview();
public:
    static void camera_device_on_disconnected(void *context, ACameraDevice *device) {
        LOGCATI("Camera(id: %s) is diconnected.\n", ACameraDevice_getId(device));
    }

    static void camera_device_on_error(void *context, ACameraDevice *device, int error) {
        LOGCATE("Error(code: %d) on Camera(id: %s).\n", error, ACameraDevice_getId(device));
    }

    static void capture_session_on_ready(void *context, ACameraCaptureSession *session) {
        LOGCATI("Session is ready. %p\n", session);
    }

    static void capture_session_on_active(void *context, ACameraCaptureSession *session) {
        LOGCATI("Session is activated. %p\n", session);
    }

    static void capture_session_on_closed(void *context, ACameraCaptureSession *session) {
        LOGCATI("Session is closed. %p\n", session);
    }
private:
    ACameraDevice_StateCallbacks deviceStateCallbacks;
    ACameraCaptureSession_stateCallbacks captureSessionStateCallbacks;
    ACameraDevice *cameraDevice;
    ACaptureRequest *captureRequest;
    ACaptureSessionOutputContainer *captureSessionOutputContainer;
    ANativeWindow *aNativeWindow;
    ACameraOutputTarget *cameraOutputTarget;
    ACaptureSessionOutput *sessionOutput;
    ACameraCaptureSession *captureSession;
};



#endif //MEDIADEMO_CAM2WRAPPER_H
