package com.hikvision.mediademo;

import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.PersistableBundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.hikvision.audio.AudioRecorder;
import com.hikvision.audio.AudioTrackManager;
import com.hikvision.opengl.MyMediaRender;
import com.hikvision.util.FileUtils;

/**
 * description ： TODO:类的作用
 * author : 海賊王　私失敗しないので
 * param
 * date : 2022/7/20 14:01
 */
public class AudioActivity extends AppCompatActivity implements View.OnClickListener, AudioRecorder.AudioRecorderCallback {
    private static final String TAG = "AudioActivity";

    private AudioRecorder mAudioRecorder;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_audio);
        initView();

    }

    private void initView(){
        findViewById(R.id.tv_recordPCM).setOnClickListener(this);
        findViewById(R.id.tv_cancelRecordPCM).setOnClickListener(this);
        findViewById(R.id.tv_playPCM).setOnClickListener(this);
        findViewById(R.id.tv_camcelPlayPCM).setOnClickListener(this);
    }
    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_recordPCM:
                mAudioRecorder = new AudioRecorder(AudioActivity.this);
                mAudioRecorder.start();
                break;
            case R.id.tv_cancelRecordPCM:
                if (mAudioRecorder!=null){
                    mAudioRecorder.interrupt();
                    try {
                        mAudioRecorder.join();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    mAudioRecorder = null;
                }
                break;
            case R.id.tv_playPCM:
                AudioTrackManager.getInstance().startPlay("/sdcard/pcm1.pcm");
                break;
            case R.id.tv_camcelPlayPCM:
                AudioTrackManager.getInstance().stopPlay();
                break;
            default:
                break;
        }
    }

    @Override
    public void onAudioData(byte[] data, int dataSize) {
        Log.d(TAG, "onAudioData: dataSize=" + dataSize);
        byte[] rightData = new byte[dataSize/4];
        byte[] mic1Data = new byte[dataSize/4];
        byte[] mic2Data = new byte[dataSize/4];
        byte[] mic3Data = new byte[dataSize/4];
        stereo2mono(data, dataSize, rightData, mic1Data, mic2Data,mic3Data);
        FileUtils.saveData("/sdcard/pcm0.pcm",rightData,true);
        FileUtils.saveData("/sdcard/pcm1.pcm",mic1Data,true);
        FileUtils.saveData("/sdcard/pcm2.pcm",mic2Data,true);
        FileUtils.saveData("/sdcard/pcm3.pcm",mic3Data,true);
    }

    @Override
    public void onError(String msg) {

    }


    public int stereo2mono(byte[] stereoData, int stereoSize, byte[] rightData, byte[] mic1Data, byte[] mic2Data, byte[] mic3Data) {
        if (stereoData == null || stereoSize <= 0 || rightData == null || mic1Data == null || mic2Data == null || mic3Data == null)
            return -1;
        int mic1 = 0;
        int mic2 = 0;
        int mic3 = 0;
        int right = 0;
        int len = 0;
        for(len = 0; len < stereoSize; len += 8)
        {
            rightData[right++] = stereoData[len];
            rightData[right++] = stereoData[len + 1];
            mic1Data[mic1++] = stereoData[len + 2];
            mic1Data[mic1++] = stereoData[len + 3];
            mic2Data[mic2++] = stereoData[len + 4];
            mic2Data[mic2++] = stereoData[len + 5];
            mic3Data[mic3++] = stereoData[len + 6];
            mic3Data[mic3++] = stereoData[len + 7];
        }
        return 0;

    }
}
