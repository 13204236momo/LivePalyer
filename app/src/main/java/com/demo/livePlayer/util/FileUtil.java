package com.demo.livePlayer.util;

import android.content.Context;
import android.os.Environment;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;

public class FileUtil {
    public static String readRawTextFile(Context context,int rawId){
        InputStream inputStream = context.getResources().openRawResource(rawId);
        BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
        String line;
        StringBuilder stringBuilder = new StringBuilder();
        try {
            while ((line = bufferedReader.readLine())!=null){
                stringBuilder.append(line);
                stringBuilder.append("\n");
            }
        }catch (Exception e){
            e.printStackTrace();
        }
        return stringBuilder.toString();
    }

    public static void copyAssets(Context context, String path) {
        File model = new File(path);
        File file = new File(Environment.getExternalStorageDirectory(), model.getName());
        if (file.exists()) {
            file.delete();
        }
        try {
            FileOutputStream fos = new FileOutputStream(file);
            InputStream is = context.getAssets().open(path);
            int len;
            byte[] b = new byte[2048];
            while ((len = is.read(b)) != -1) {
                fos.write(b, 0, len);
            }
            fos.close();
            is.close();
        } catch (Exception e) {
            e.printStackTrace();
        }

    }
}
