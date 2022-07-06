//
// Created by pirate on 2022/7/6.
//

#include "Cam2Wrapper.h"
#include "Common.h"
#include "utils/LogUtil.h"



int Cam2Wrapper::openCamera(ACameraDevice_request_template templateId) {
    int ret = 0;
    ACameraIdList *cameraIdList = NULL;
    ACameraMetadata *cameraMetadata = NULL;
    const char *selectedCameraId = NULL;
    camera_status_t camera_status = ACAMERA_OK;
    ACameraManager *cameraManager = ACameraManager_create();
    if (cameraManager== NULL){
        ret = DSP_FAIL;
        goto EXIT;
    }
    camera_status = ACameraManager_getCameraIdList(cameraManager, &cameraIdList);
    if (camera_status != ACAMERA_OK) {
        LOGCATE("Failed to get camera id list (reason: %d)\n", camera_status);
        ret = DSP_FAIL;
        goto EXIT;
    }
    if (cameraIdList->numCameras < 1) {
        LOGCATE("No camera device detected.\n");
        ret = DSP_FAIL;
        goto EXIT;
    }
    //选择摄像头
    selectedCameraId = cameraIdList->cameraIds[0];
    LOGCATI("Trying to open Camera2 (id: %s, num of camera : %d)\n", selectedCameraId,
         cameraIdList->numCameras);

    camera_status = ACameraManager_getCameraCharacteristics(cameraManager, selectedCameraId,
                                                            &cameraMetadata);
    if (camera_status != ACAMERA_OK) {
        LOGCATE("Failed to get camera characteristics: camera_status = %d\n", camera_status);
        ret = DSP_FAIL;
        goto EXIT;
    }
    //设置回调函数
    deviceStateCallbacks.onDisconnected = camera_device_on_disconnected;
    deviceStateCallbacks.onError = camera_device_on_error;

    camera_status = ACameraManager_openCamera(cameraManager, selectedCameraId,
                                              &deviceStateCallbacks, &cameraDevice);
    if (camera_status != ACAMERA_OK) {
        LOGCATE("Failed to open camera device camera_status = %d\n", camera_status);
        ret = DSP_FAIL;
        goto EXIT;
    }
    //发起获取图像请求
    camera_status = ACameraDevice_createCaptureRequest(cameraDevice, templateId,
                                                       &captureRequest);
    if (camera_status != ACAMERA_OK) {
        LOGCATE("Failed to create preview capture request  camera_status = %d\n", camera_status);
        ret = DSP_FAIL;
        goto EXIT;
    }

    camera_status = ACaptureSessionOutputContainer_create(&captureSessionOutputContainer);
    if (camera_status != ACAMERA_OK) {
        LOGCATE("Failed to CaptureSessionOutputContainer_create camera_status = %d\n", camera_status);
        ret = DSP_FAIL;
        goto EXIT;
    }

    captureSessionStateCallbacks.onReady = capture_session_on_ready;
    captureSessionStateCallbacks.onActive = capture_session_on_active;
    captureSessionStateCallbacks.onClosed = capture_session_on_closed;
    ret = 0;
EXIT:
    if (cameraMetadata!=NULL){
        ACameraMetadata_free(cameraMetadata);
    }
    if (cameraIdList!=NULL){
        ACameraManager_deleteCameraIdList(cameraIdList);
    }
    if (cameraManager!=NULL){
        ACameraManager_delete(cameraManager);
    }
    return ret;
}




void Cam2Wrapper::closeCamera()
{
    camera_status_t camera_status = ACAMERA_OK;

    if (captureRequest != NULL) {
        ACaptureRequest_free(captureRequest);
        captureRequest = NULL;
    }

    if (cameraOutputTarget != NULL) {
        ACameraOutputTarget_free(cameraOutputTarget);
        cameraOutputTarget = NULL;
    }

    if (cameraDevice != NULL) {
        camera_status = ACameraDevice_close(cameraDevice);

        if (camera_status != ACAMERA_OK) {
            LOGCATE("Failed to close CameraDevice.\n");
        }
        cameraDevice = NULL;
    }

    if (sessionOutput != NULL) {
        ACaptureSessionOutput_free(sessionOutput);
        sessionOutput = NULL;
    }

    if (captureSessionOutputContainer != NULL) {
        ACaptureSessionOutputContainer_free(captureSessionOutputContainer);
        captureSessionOutputContainer = NULL;
    }

    LOGCATI("Close Camera\n");
}

void Cam2Wrapper::startPreview(JNIEnv *jniEnv,jobject surface){
    camera_status_t camera_status = ACAMERA_OK;
    /* Assuming that camera preview has already been started */
    aNativeWindow = ANativeWindow_fromSurface(jniEnv, surface);
    camera_status = ACameraOutputTarget_create(aNativeWindow, &cameraOutputTarget);
    if (camera_status != ACAMERA_OK) {
        LOGCATE("Failed to ACameraOutputTarget_create camera_status = %d\n", camera_status);
        return;
    }
    //将请求与输出绑定
    camera_status = ACaptureRequest_addTarget(captureRequest, cameraOutputTarget);
    if (camera_status != ACAMERA_OK) {
        LOGCATE("Failed to ACaptureRequest_addTarget camera_status = %d\n", camera_status);
        return;
    }
    //to add an output ANativeWindow to ACaptureSessionOutputContainer.
    camera_status = ACaptureSessionOutput_create(aNativeWindow, &sessionOutput);
    if (camera_status != ACAMERA_OK) {
        LOGCATE("Failed to ACaptureSessionOutput_create camera_status = %d\n", camera_status);
        return;
    }
    camera_status = ACaptureSessionOutputContainer_add(captureSessionOutputContainer, sessionOutput);
    if (camera_status != ACAMERA_OK) {
        LOGCATE("Failed to ACaptureSessionOutputContainer_add camera_status = %d\n", camera_status);
        return;
    }

    camera_status =ACameraDevice_createCaptureSession(cameraDevice, captureSessionOutputContainer,
                                                      &captureSessionStateCallbacks, &captureSession);
    if (camera_status != ACAMERA_OK) {
        LOGCATE("Failed to ACameraDevice_createCaptureSession camera_status = %d\n", camera_status);
        return;
    }

    camera_status =ACameraCaptureSession_setRepeatingRequest(captureSession, NULL, 1, &captureRequest, NULL);
    if (camera_status != ACAMERA_OK) {
        LOGCATE("Failed to ACameraCaptureSession_setRepeatingRequest (reason: %d)\n", camera_status);
        return;
    }
}


void Cam2Wrapper::stopPreview() {
    closeCamera();
    if (aNativeWindow != NULL) {
        ANativeWindow_release(aNativeWindow);
        aNativeWindow = NULL;
    }
}