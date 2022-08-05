package com.hikvision.mediademo;


import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import com.hikvision.adapter.SelectItem;
import com.hikvision.adapter.SelectItemAdapter;

import java.util.ArrayList;
import java.util.List;


public class MainActivity extends AppCompatActivity {
    private static String TAG = "MainActivity";
    private List<SelectItem> selectItemList = new ArrayList<>();
    private static final int CAM_OPEATION = 0;
    private static final int Audio_OPEATION = 1;
    private static final int DE_MEDIACODEC = 2;
    private static final int Audio_EXAMPLE = 3;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initItems();
        RecyclerView recyclerView = (RecyclerView) findViewById(R.id.recycler_view);
        LinearLayoutManager layoutManager = new LinearLayoutManager(this);
        recyclerView.setLayoutManager(layoutManager);
        final SelectItemAdapter adapter = new SelectItemAdapter(selectItemList);
        recyclerView.setAdapter(adapter);
        adapter.addOnItemClickListener(new SelectItemAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(View view, int position) {
                int selectIndex = adapter.getSelectIndex();
                adapter.setSelectIndex(position);
                adapter.safeNotifyItemChanged(selectIndex);
                adapter.safeNotifyItemChanged(position);
                Log.d(TAG, "onItemClick: " + position);
                switch (position) {
                    case CAM_OPEATION:
                        startActivity(new Intent(MainActivity.this, CamActivity.class));
                        break;
                    case Audio_OPEATION:
                        startActivity(new Intent(MainActivity.this, AudioActivity.class));
                        break;
                    default:
                        break;
                }
            }
        });
    }

    private void initItems() {
        SelectItem cam = new SelectItem("摄像头预览 camera2 + opengl", R.drawable.ic_camera);
        selectItemList.add(cam);
        SelectItem speaker = new SelectItem("speaker", R.drawable.ic_aspect);
        selectItemList.add(speaker);
    }
}