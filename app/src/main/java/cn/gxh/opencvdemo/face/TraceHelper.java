package cn.gxh.opencvdemo.face;

import android.view.Surface;

public class TraceHelper {

    static {
        System.loadLibrary("native-lib");
    }

    /**
     * 初始化追踪器
     * @param model
     */
    native void init(String model);

    /**
     * 释放追踪器
     */
    native void release();

    /**
     * 处理摄像头数据
     * @param data
     * @param w
     * @param h
     * @param cameraId
     */
    native void postData(byte[] data,int w,int h,int cameraId);

    /**
     * 设置画布
     * @param surface
     */
    native void setSurface(Surface surface);

}
