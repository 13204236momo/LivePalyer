package com.demo.livePlayer.opensgl.shape;

import android.opengl.GLES20;
import android.opengl.Matrix;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.opengles.GL10;

public class Triangle {
//    opengl 操作

    //    初始化  地址
    int mProgram;
    //    渲染
    static float triangleCoords[] = {
            0.5f, 0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
    };
    private FloatBuffer vertexBuffer;
    //     native 函数 写好了
    private String vertextShaderCode = "attribute vec4 vPosition;" +
            "uniform mat4 vMatrix;\n" +
            "void main(){" +
            "gl_Position=vMatrix*vPosition;" +
            "}";
    private final String fragmentShaderCode = "precision mediump float;\n" +
            "uniform  vec4 vColor;\n" +
            "void main(){\n" +
            "gl_FragColor=vColor;\t\n" +
            "}";
    private float[] mViewMatrix=new float[16];
    private float[] mProjectMatrix=new float[16];
    private float[] mMVPMatrix=new float[16];

    public void onSurfaceChanged(GL10 gl, int width, int height) {

//        固定的写法
        //        //计算宽高比
        float ratio=(float)width/height;
//        投影矩阵
        Matrix.frustumM(mProjectMatrix, 0, -ratio, ratio, -1, 1, 3, 120);

        Matrix.setLookAtM(mViewMatrix, 0, 0, 0, 7,//摄像机的坐标
                0f, 0f, 0f,//目标物的中心坐标
                0f, 1f, 0f);//相机方向
        //计算变换矩阵
        Matrix.multiplyMM(mMVPMatrix,0,mProjectMatrix,0,mViewMatrix,0);
    }

    public Triangle() {
        ByteBuffer bb = ByteBuffer.allocateDirect(triangleCoords.length * 4);
        bb.order(ByteOrder.nativeOrder());
        vertexBuffer=bb.asFloatBuffer();
//        把这门语法  推送 给GPU
        vertexBuffer.put(triangleCoords);
        vertexBuffer.position(0);
//        创建顶点着色器  并且在GPU进行编译  98技师
        int shader=GLES20.glCreateShader(GLES20.GL_VERTEX_SHADER);
        GLES20.glShaderSource(shader, vertextShaderCode);
        GLES20.glCompileShader(shader);

//  创建片元着色器  并且在GPU进行编译           99技师
        int fragmentShader=GLES20.glCreateShader(GLES20.GL_FRAGMENT_SHADER);
        GLES20.glShaderSource(fragmentShader, fragmentShaderCode);
        GLES20.glCompileShader(fragmentShader);
//     将片元着色器 和顶点着色器 放到统一程序今次那个管理   前台
        mProgram = GLES20.glCreateProgram();
        GLES20.glAttachShader(mProgram, shader);
        GLES20.glAttachShader(mProgram, fragmentShader);
        //连接到着色器程序
        GLES20.glLinkProgram(mProgram);
    }
    float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    //    gifAddr
    public void onDrawFrame(GL10 gl) {
//        渲染
        GLES20.glUseProgram(mProgram);
        int mMatrixHandler = GLES20.glGetUniformLocation(mProgram, "vMatrix");
        GLES20.glUniformMatrix4fv(mMatrixHandler,1,false,mMVPMatrix,0);
//        指针     native的指针      gpu的某个内存区域  vPosition
        int mPositionHandle = GLES20.glGetAttribLocation(mProgram, "vPosition");
//        打开 允许对变量读写
        GLES20.glEnableVertexAttribArray(mPositionHandle);
        GLES20.glVertexAttribPointer(mPositionHandle, 3, GLES20.GL_FLOAT, false,
                3 * 4, vertexBuffer);
        int mColorHandle = GLES20.glGetUniformLocation(mProgram, "vColor");
        GLES20.glUniform4fv(mColorHandle, 1, color, 0);
        GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, 3);
        GLES20.glDisableVertexAttribArray(mPositionHandle);
    }
}
