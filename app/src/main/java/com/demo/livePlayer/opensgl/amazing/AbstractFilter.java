package com.demo.livePlayer.opensgl.amazing;

import android.content.Context;
import android.opengl.GLES20;

import com.demo.livePlayer.R;
import com.demo.livePlayer.util.GLUtil;
import com.demo.livePlayer.util.OpenFileUtil;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.opengles.GL10;

public abstract class AbstractFilter {

    //顶点着色器
    protected int vertexShaderId;
    //片元着色器
    protected int fragmentShaderId;
    protected FloatBuffer mVertexBuffer;
    protected FloatBuffer mFragmentBuffer;
    protected Context context;
    protected int mProgram;
    protected int vTexture;
    protected int vMatrix;
    protected int vCoord;
    protected int vPosition;
    protected int mWidth;
    protected int mHeight;

    public AbstractFilter(Context context, int vertexShaderId, int fragmentShaderId) {
        this.vertexShaderId = vertexShaderId;
        this.fragmentShaderId = fragmentShaderId;


        mVertexBuffer = ByteBuffer.allocateDirect(4 * 2 * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        mVertexBuffer.clear();
        float[] VERTEX = {
                -0.1f, -1.0f,
                1.0f, -1.0f,
                -1.0f, 1.0f,
                1.0f, 1.0f
        };
        mVertexBuffer.put(VERTEX);

        mFragmentBuffer = ByteBuffer.allocateDirect(4 * 2 * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        mFragmentBuffer.clear();
        float[] FRAGMENT = {
                0.0f, 1.0f,
                1.0f, 1.0f,
                0.0f, 0.0f,
                1.0f, 1.0f
        };
        mFragmentBuffer.put(FRAGMENT);
        initialize(context);
        initCoordinate();
    }

    protected abstract void initCoordinate();

    protected void initialize(Context context) {
        String vertexShader = OpenFileUtil.readRawTextFile(context, R.raw.base_vertex);
        String fragmentShader = OpenFileUtil.readRawTextFile(context,R.raw.base_frag);
        mProgram = GLUtil.loadProgram(vertexShader,fragmentShader);
        //获取vPosition
        vPosition = GLES20.glGetAttribLocation(mProgram,"vPosition");
        vCoord = GLES20.glGetAttribLocation(mProgram,"vCoord");

        vMatrix = GLES20.glGetUniformLocation(mProgram,"vMatrix");
        //获取Uniform变量的索引值
        vTexture = GLES20.glGetUniformLocation(mProgram,"vTexture");

    }

    public int onDrawFrame(int textureId){
        //设置显示窗口
        GLES20.glViewport(0, 0, mWidth, mHeight);

        //使用着色器
        GLES20.glUseProgram(mProgram);
        mVertexBuffer.position(0);
        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 0, mVertexBuffer);
        GLES20.glEnableVertexAttribArray(vPosition);

        mFragmentBuffer.position(0);
        GLES20.glVertexAttribPointer(vCoord, 2, GLES20.GL_FLOAT, false, 0, mFragmentBuffer);
        GLES20.glEnableVertexAttribArray(vCoord);

//        不一样的地方
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D,textureId);

        GLES20.glUniform1i(vTexture, 0);
//        绘制
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
        return textureId;
    }
}
