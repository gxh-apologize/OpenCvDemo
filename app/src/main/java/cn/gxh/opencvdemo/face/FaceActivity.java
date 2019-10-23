package cn.gxh.opencvdemo.face;

import android.hardware.Camera;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.File;

import cn.gxh.opencvdemo.R;

public class FaceActivity extends AppCompatActivity implements Camera.PreviewCallback,SurfaceHolder.Callback {

    CameraHelper cameraHelper;
    TraceHelper traceHelper;
    int cameraId=1;
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_face);
        traceHelper=new TraceHelper();
        Utils.copyAssets(this, "lbpcascade_frontalface.xml");

        SurfaceView surfaceView=findViewById(R.id.surfaceView);
        surfaceView.getHolder().addCallback(this);

        cameraHelper=new CameraHelper(cameraId);
        cameraHelper.setPreviewCallback(this);
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        traceHelper.postData(data,CameraHelper.WIDTH,CameraHelper.HEIGHT,cameraId);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {

        if (event.getAction() == MotionEvent.ACTION_UP) {
            cameraHelper.switchCamera();
            cameraId = cameraHelper.getCameraId();
        }
        return super.onTouchEvent(event);
    }

    @Override
    protected void onResume() {
        super.onResume();
        traceHelper.init(Environment.getExternalStorageDirectory()+File.separator+
                        "lbpcascade_frontalface.xml");
        cameraHelper.startPreview();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        traceHelper.setSurface(holder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        traceHelper.release();
        cameraHelper.stopPreview();
    }
}
