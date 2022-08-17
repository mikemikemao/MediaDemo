/**
 *
 * Created by 公众号：字节流动 on 2021/3/16.
 * https://github.com/githubhaohao/LearnFFmpeg
 * 最新文章首发于公众号：字节流动，有疑问或者技术交流可以添加微信 Byte-Flow ,领取视频教程, 拉你进技术交流群
 *
 * */


#ifndef LEARNFFMPEG_BASEGLRENDER_H
#define LEARNFFMPEG_BASEGLRENDER_H

#include "ImageDef.h"

class BaseGLRender {
public:
    virtual ~BaseGLRender(){}
    virtual void OnSurfaceCreated() = 0;
    virtual void OnSurfaceChanged(int w, int h) = 0;
    virtual void OnDrawFrame() = 0;
};


#endif //LEARNFFMPEG_BASEGLRENDER_H
