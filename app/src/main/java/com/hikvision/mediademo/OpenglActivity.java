package com.hikvision.mediademo;

import android.app.AlertDialog;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.Button;
import android.widget.RelativeLayout;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.hikvision.adapter.OpenglRecyclerViewAdapter;
import com.hikvision.opengl.MyMediaRender;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.util.Arrays;

import static android.opengl.GLSurfaceView.RENDERMODE_CONTINUOUSLY;
import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;
import static com.hikvision.jni.MyCam.IMAGE_FORMAT_RGBA;
import static com.hikvision.jni.MyCam.SAMPLE_TYPE;
import static com.hikvision.jni.MyCam.SAMPLE_TYPE_KEY_BEATING_HEART;
import static com.hikvision.jni.MyCam.SAMPLE_TYPE_KEY_MULTI_THREAD_RENDER;
import static com.hikvision.jni.MyCam.SAMPLE_TYPE_TEXTURE_MAP;
import static com.hikvision.jni.MyCam.SAMPLE_TYPE_TRIANGLE;
import static com.hikvision.opengl.MyMediaRender.RECORDER_TYPE_SINGLE_VIDEO;

/**
 * description ： TODO:类的作用
 * author : 海賊王　私失敗しないので
 * param
 * date : 2022/9/2 10:10
 */
public class OpenglActivity extends AppCompatActivity {
    private static final String TAG  = "OpenglActivity";

    private   MyMediaRender myMediaRender;
    protected GLSurfaceView mGLSurfaceView;

    private int mSampleSelectedIndex = SAMPLE_TYPE_KEY_BEATING_HEART - SAMPLE_TYPE;

    private static final String[] SAMPLE_TITLES = {
            "DrawTriangle",
            "TextureMap",
            "YUV Rendering",
            "VAO&VBO",
            "FBO Offscreen Rendering",
            "EGL Background Rendering",
            "FBO Stretching",
            "Coordinate System",
            "Basic Lighting",
            "Transform Feedback",
            "Complex Lighting",
            "Depth Testing",
            "Instancing",
            "Stencil Testing",
            "Blending",
            "Particles",
            "SkyBox",
            "Assimp Load 3D Model",
            "PBO",
            "Beating Heart",
            "Cloud",
            "Time Tunnel",
            "Bezier Curve",
            "Big Eyes",
            "Face Slender",
            "Big Head",
            "Rotary Head",
            "Visualize Audio",
            "Scratch Card",
            "3D Avatar",
            "Shock Wave",
            "MRT",
            "FBO Blit",
            "Texture Buffer",
            "Uniform Buffer",
            "RGB to YUYV",
            "Multi-Thread Render",
            "Text Render",
            "Portrait stay color",
            "GL Transitions_1",
            "GL Transitions_2",
            "GL Transitions_3",
            "GL Transitions_4",
            "RGB to NV21",
            "RGB to I420",
    };
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_opengltest);
        init();
    }

    private void init(){
        myMediaRender = new MyMediaRender();
        mGLSurfaceView = (GLSurfaceView)findViewById(R.id.glSurfaceViewTest);
        myMediaRender.init(mGLSurfaceView,1);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        myMediaRender.unInit();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_opengl, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_change_sample) {
            showGLSampleDialog();
        }
        return true;
    }

    private void showGLSampleDialog() {
        final AlertDialog.Builder builder = new AlertDialog.Builder(this);
        LayoutInflater inflater = LayoutInflater.from(this);
        final View rootView = inflater.inflate(R.layout.opengl_sample_selected_layout, null);

        final AlertDialog dialog = builder.create();

        Button confirmBtn = rootView.findViewById(R.id.confirm_btn);
        confirmBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialog.cancel();
            }
        });

        final RecyclerView resolutionsListView = rootView.findViewById(R.id.resolution_list_view);

        final OpenglRecyclerViewAdapter myPreviewSizeViewAdapter = new OpenglRecyclerViewAdapter(this, Arrays.asList(SAMPLE_TITLES));
        myPreviewSizeViewAdapter.setSelectIndex(mSampleSelectedIndex);
        myPreviewSizeViewAdapter.addOnItemClickListener(new OpenglRecyclerViewAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(View view, int position) {
                int selectIndex = myPreviewSizeViewAdapter.getSelectIndex();
                myPreviewSizeViewAdapter.setSelectIndex(position);
                myPreviewSizeViewAdapter.notifyItemChanged(selectIndex);
                myPreviewSizeViewAdapter.notifyItemChanged(position);
                mSampleSelectedIndex = position;
                mGLSurfaceView.setRenderMode(RENDERMODE_WHEN_DIRTY);

                myMediaRender.setParamsInt(SAMPLE_TYPE, position + SAMPLE_TYPE, 0);

                int sampleType = position + SAMPLE_TYPE;
                Bitmap tmp;
                switch (sampleType) {
                    case SAMPLE_TYPE_TRIANGLE:
                        break;
                    case SAMPLE_TYPE_TEXTURE_MAP:
                        Log.d(TAG, "onClick: glTxtMap");
                        loadRGBAImage(R.drawable.dzzz);
                    case SAMPLE_TYPE_KEY_MULTI_THREAD_RENDER:
                        Log.d(TAG, "MultiThreadRender");
                        loadRGBAImage(R.drawable.lye);
                        mGLSurfaceView.setRenderMode(RENDERMODE_CONTINUOUSLY);
//                        int frameWidth = 524;
//                        int frameHeight = 790;
//                        int fps = 25;
//                        int bitRate = (int) (frameWidth * frameHeight * fps * 0.25);
//                        myMediaRender.startRecord(RECORDER_TYPE_SINGLE_VIDEO, "/sdcard/record.mp4", frameWidth, frameHeight, bitRate, fps);
                    default:
                        break;
                }
                mGLSurfaceView.requestRender();
                dialog.cancel();
            }
        });

        LinearLayoutManager manager = new LinearLayoutManager(this);
        manager.setOrientation(LinearLayoutManager.VERTICAL);
        resolutionsListView.setLayoutManager(manager);

        resolutionsListView.setAdapter(myPreviewSizeViewAdapter);
        resolutionsListView.scrollToPosition(mSampleSelectedIndex);

        dialog.show();
        dialog.getWindow().setContentView(rootView);

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
