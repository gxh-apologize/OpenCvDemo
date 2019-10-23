#include <jni.h>
#include <string>

#include <opencv2/opencv.hpp>
#include <android/native_window_jni.h>

#define LOG(FORMAT,...) printf(FORMAT,__VA_ARGS__);
#define LOG_I(FORMAT,...) printf("INFO:"); printf(FORMAT,__VA_ARGS__);

using namespace cv;
class CascadeDetectorAdapter : public DetectionBasedTracker::IDetector {
public:
    CascadeDetectorAdapter(cv::Ptr<cv::CascadeClassifier> detector) :
            IDetector(),
            Detector(detector) {
        CV_Assert(detector);
    }

    void detect(const cv::Mat &Image, std::vector<cv::Rect> &objects) {
        Detector->detectMultiScale(Image, objects, scaleFactor, minNeighbours, 0, minObjSize,
                                   maxObjSize);
    }

    virtual ~CascadeDetectorAdapter() {
    }

private:
    CascadeDetectorAdapter();
    cv::Ptr<cv::CascadeClassifier> Detector;
};


ANativeWindow *window = 0;
extern "C" JNIEXPORT void JNICALL
Java_cn_gxh_opencvdemo_face_TraceHelper_setSurface(
        JNIEnv* env,
        jobject /* this */,jobject surface) {
    if (window) {
        ANativeWindow_release(window);
        window = 0;
    }
    window = ANativeWindow_fromSurface(env, surface);
}

DetectionBasedTracker *tracker = 0;
extern "C" JNIEXPORT void JNICALL
Java_cn_gxh_opencvdemo_face_TraceHelper_init(
        JNIEnv* env,
        jobject /* this */,jstring model_) {
    if(tracker){
        tracker->stop();
        delete tracker;
        tracker=0;
    }

    const char *model=env->GetStringUTFChars(model_,0);
    //智能指针
    Ptr<CascadeClassifier> classifier = makePtr<CascadeClassifier>(model);
    //创建一个跟踪适配器
    Ptr<CascadeDetectorAdapter> mainDetector = makePtr<CascadeDetectorAdapter>(classifier);
    Ptr<CascadeClassifier> classifier1 = makePtr<CascadeClassifier>(model);
    //创建一个跟踪适配器
    Ptr<CascadeDetectorAdapter> trackingDetector = makePtr<CascadeDetectorAdapter>(classifier1);
    //拿去用的跟踪器
    DetectionBasedTracker::Parameters DetectorParams;
    tracker = new DetectionBasedTracker(mainDetector, trackingDetector, DetectorParams);
    //开启跟踪器
    tracker->run();
    env->ReleaseStringUTFChars(model_,model);
}


extern "C" JNIEXPORT void JNICALL
Java_cn_gxh_opencvdemo_face_TraceHelper_release(
        JNIEnv* env,
        jobject /* this */) {
    if(tracker){
        tracker->stop();
        delete tracker;
        tracker=0;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_cn_gxh_opencvdemo_face_TraceHelper_postData(
        JNIEnv* env,
        jobject /* this */,jbyteArray data_,jint w,jint h,jint cameraId) {

    //nv21
    jbyte *data=env->GetByteArrayElements(data_,NULL);
    //参数:高、宽
    Mat src(h+h/2,w,CV_8UC1,data);
    //颜色格式转换 nv21->rgba
    cvtColor(src,src,COLOR_YUV2RGBA_NV21);
    /**
     * imwrite可以保存图片到本地，我们可以来看下大致的图片
     * 使用imwrite时需要cvtColor用COLOR_YUV2BGR_NV21
     * 因为我们在java层没有对回调的数据流调整方向
     */
    //imwrite("path",src);
    if(cameraId==1){
        //前置摄像头需要逆时针旋转90
        rotate(src,src,ROTATE_90_COUNTERCLOCKWISE);
        //镜像处理  1为水平翻转
        flip(src,src,1);
    } else{
        rotate(src,src,ROTATE_90_CLOCKWISE);
    }

    Mat gray;
    cvtColor(src,gray,COLOR_RGBA2GRAY);
    //增强对比度
    equalizeHist(gray,gray);
    std::vector<Rect> faces;
    tracker->process(gray);
    tracker->getObjects(faces);

    for(Rect face : faces){
        LOG_I("gxh:%s","find face");
        rectangle(src,face,Scalar(255,0,255));
    }

    if(window){
        //通过设置宽高限制缓冲区中的像素数量
        //如果缓冲区与物理屏幕的显示尺寸不相符，则实际显示可能会是拉伸，或者被压缩的图像
        //上面旋转了，所以这里w、h需要交换，也可以用src的cols、rows
        ANativeWindow_setBuffersGeometry(window,src.cols,src.rows,WINDOW_FORMAT_RGBA_8888);
        //视频缓冲区
        ANativeWindow_Buffer buffer;
        do{
            if(ANativeWindow_lock(window,&buffer,0)!=0){//失败
                ANativeWindow_release(window);
                window=0;
                break;
            }
            /**
             * 一行有多少像素  buffer.stride*4
             */
//            memcpy(buffer.bits,src.data,buffer.stride*buffer.height*4);


            uint8_t *dst_data = static_cast<uint8_t *>(buffer.bits);
            int dst_linesize = buffer.stride * 4;

            //一行一行拷贝
            for (int i = 0; i < buffer.height; ++i) {
                memcpy(dst_data + i * dst_linesize, src.data + i * src.cols * 4, dst_linesize);
            }

            ANativeWindow_unlockAndPost(window);

        }while (0);
    }

    //释放MAT
    src.release();
    gray.release();

    env->ReleaseByteArrayElements(data_,data,0);
}
