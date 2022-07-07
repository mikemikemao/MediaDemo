package com.hikvision.adapter;

public class SelectItem {

    private String name;
    private int imageId;

    public SelectItem(String name, int imageId){
        this.name = name;
        this.imageId = imageId;

    }

    public String getName() {
        return name;
    }

    public int getImageId() {
        return imageId;
    }
}