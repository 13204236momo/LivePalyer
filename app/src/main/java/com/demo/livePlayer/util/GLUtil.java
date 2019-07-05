package com.demo.livePlayer.util;

import android.opengl.GLES20;

public class GLUtil {
    public static int loadProgram(String vSource,String fSource){
        /**
         * 顶点着色器
         */
        int vShader = GLES20.glCreateShader(GLES20.GL_VERTEX_SHADER);
        //加载着色器代码
        GLES20.glShaderSource(vShader,vSource);
        //编译（配置）
        GLES20.glCompileShader(vShader);

        //查看配置 是否成功
        int[] status = new int[1];
        GLES20.glGetShaderiv(vShader,GLES20.GL_COMPILE_STATUS,status,0);
        if(status[0] != GLES20.GL_TRUE){
            //失败
            throw new IllegalStateException("load vertex shader:"+GLES20.glGetShaderInfoLog(vShader));
        }

        /**
         *  片元着色器
         *  流程和上面一样
         */
        int fShader = GLES20.glCreateShader(GLES20.GL_FRAGMENT_SHADER);
        //加载着色器代码
        GLES20.glShaderSource(fShader,fSource);
        //编译（配置）
        GLES20.glCompileShader(fShader);

        //查看配置 是否成功
        GLES20.glGetShaderiv(fShader,GLES20.GL_COMPILE_STATUS,status,0);
        if(status[0] != GLES20.GL_TRUE){
            //失败
            throw new IllegalStateException("load fragment shader:"+GLES20.glGetShaderInfoLog(vShader));
        }


        /**
         * 创建着色器程序
         */
        int program = GLES20.glCreateProgram();
        //绑定顶点和片元
        GLES20.glAttachShader(program,vShader);
        GLES20.glAttachShader(program,fShader);
        //链接着色器程序
        GLES20.glLinkProgram(program);
        //获得状态
        GLES20.glGetProgramiv(program,GLES20.GL_LINK_STATUS,status,0);
        if(status[0] != GLES20.GL_TRUE){
            throw new IllegalStateException("link program:"+GLES20.glGetProgramInfoLog(program));
        }
        GLES20.glDeleteShader(vShader);
        GLES20.glDeleteShader(fShader);
        return program;
    }
}