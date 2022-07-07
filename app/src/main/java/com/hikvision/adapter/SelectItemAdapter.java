package com.hikvision.adapter;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.hikvision.mediademo.R;

import java.util.List;

public class SelectItemAdapter extends RecyclerView.Adapter<SelectItemAdapter.ViewHolder> {
    private static String TAG = "SelectItemAdapter";
    private List<SelectItem> mSelectItemList;
    private OnItemClickListener mOnItemClickListener = null;
    private int mSelectIndex =0;

    public SelectItemAdapter(List<SelectItem> selectItemList) {
        mSelectItemList = selectItemList;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.select_item, parent, false);
        ViewHolder holder = new ViewHolder(view);
        return holder;
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        SelectItem mSelectItem = mSelectItemList.get(position);
        holder.selectItemImage.setImageResource(mSelectItem.getImageId());
        holder.selectItemName.setText(mSelectItem.getName());

        holder.itemView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mOnItemClickListener!=null){
                    mOnItemClickListener.onItemClick(v, position);
                }
            }
        });
    }

    @Override
    public int getItemCount() {
        return mSelectItemList.size();
    }

    public class ViewHolder extends RecyclerView.ViewHolder {
        ImageView selectItemImage;
        TextView selectItemName;
        public ViewHolder(View itemView) {
            super(itemView);
            selectItemImage = (ImageView) itemView.findViewById(R.id.select_image);
            selectItemName = (TextView) itemView.findViewById(R.id.select_name);
        }
    }

    //添加点击事件
    public void addOnItemClickListener(OnItemClickListener onItemClickListener) {
        Log.d(TAG, "addOnItemClickListener: ");
        mOnItemClickListener = onItemClickListener;
    }

    public interface OnItemClickListener {
        void onItemClick(View view, int position);
    }

//    @Override
//    public void onClick(View v) {
//        Log.d(TAG, "onClick: ");
//        if (mOnItemClickListener != null) {
//            mOnItemClickListener.onItemClick(v, (Integer) v.getTag());
//        }
//    }

    public void setSelectIndex(int index) {
        mSelectIndex = index;
    }

    public int getSelectIndex() {
        return mSelectIndex;
    }

    public void safeNotifyItemChanged(int index) {
        if(index > 0)
            notifyItemChanged(index);
    }
}
