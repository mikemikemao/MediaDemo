package com.hikvision.mediademo;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.Window;

import androidx.appcompat.app.AppCompatActivity;

import com.hikvision.opengl.MyMediaRender;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

import static com.hikvision.jni.MyCam.IMAGE_FORMAT_I420;
import static com.hikvision.jni.MyCam.IMAGE_FORMAT_RGBA;

/**
 * description ： TODO:类的作用
 * author : 海賊王　私失敗しないので
 * param
 * date : 2022/9/2 10:10
 */
public class OpenglActivity extends AppCompatActivity {
    private   MyMediaRender myMediaRender;
    protected GLSurfaceView mGLSurfaceView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_cam);
        init();
    }


    private void init(){
        myMediaRender = new MyMediaRender();
        mGLSurfaceView = (GLSurfaceView)findViewById(R.id.glSurfaceView);
        myMediaRender.init(mGLSurfaceView,1);
        Bitmap b3 = loadRGBAImage(R.drawable.lye);
    }

    private Bitmap loadRGBAImage(int resId) {
        InputStream is = this.getResources().openRawResource(resId);
        Bitmap bitmap;
        try {
            bitmap = BitmapFactory.decodeStream(is);
            if (bitmap != null) {
                int bytes = bitmap.getByteCount();
                ByteBuffer buf = ByteBuffer.allocate(bytes);
                bitmap.copyPixelsToBuffer(buf);
                byte[] byteArray = buf.array();
                myMediaRender.onPreviewFrame(IMAGE_FORMAT_RGBA, byteArray, bitmap.getWidth(),bitmap.getHeight());
            }
        }
        finally
        {
            try
            {
                is.close();
            }
            catch(IOException e)
            {
                e.printStackTrace();
            }
        }
        return bitmap;
    }
}
