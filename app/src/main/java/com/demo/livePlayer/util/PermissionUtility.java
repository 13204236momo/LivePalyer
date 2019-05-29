package com.demo.livePlayer.util;

import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.net.Uri;
import android.os.Build;
import android.provider.Settings;
import android.support.v4.content.ContextCompat;
import android.util.Log;

import com.tbruyelle.rxpermissions2.RxPermissions;

import java.util.List;

public class PermissionUtility {
    public static final String TAG = "PermissionUtility";

    public static RxPermissions getRxPermission(Activity activity) {
        RxPermissions rxPermissions = new RxPermissions(activity);
        rxPermissions.setLogging(true);
        return rxPermissions;
    }

    public static void goToSetting(Context activity) {
        String name = Build.MANUFACTURER;
        Log.d(TAG, "当前手机平台：" + name);
        /**
         * HUAWEI，vivo，OPPO......手机机型标注不可以改变
         */
        if ("HUAWEI".equals(name)) {
            goHuaWeiMainager(activity);
        } else if ("vivo".equals(name)) {
            goVivoMainager(activity);
        } else if ("OPPO".equals(name)) {
            goOppoMainager(activity);
        } else if ("Coolpad".equals(name)) {
            goCoolpadMainager(activity);
        } else if ("Meizu".equals(name)) {
            goMeizuMainager(activity);
        } else if ("Xiaomi".equals(name)) {
            goXiaoMiMainager(activity);
        } else if ("samsung".equals(name)) {
            goSangXinMainager(activity);
        } else {
            goIntentSetting(activity);
        }
    }

    private static void goHuaWeiMainager(Context activity) {
//        try {
//            Intent intent = new Intent("demo.vincent.com.tiaozhuan");
//            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
//            ComponentName comp = new ComponentName("com.huawei.systemmanager", "com.huawei.permissionmanager.ui.MainActivity");
//            intent.setComponent(comp);
//            activity.startActivity(intent);
//        } catch (Exception e) {
//            Log.d(TAG, "跳转失败，转到应用详情页");
//            e.printStackTrace();
        goIntentSetting(activity);
//        }
    }

    private static void goXiaoMiMainager(Context activity) {
        try {
            Intent localIntent = new Intent(
                    "miui.intent.action.APP_PERM_EDITOR");
            if (isMIUIv5v6(activity)) {
                localIntent.setClassName("com.miui.securitycenter",
                        "com.miui.permcenter.permissions.AppPermissionsEditorActivity");
            } else {
                localIntent.setClassName("com.miui.securitycenter",
                        "com.miui.permcenter.permissions.PermissionsEditorActivity");
            }
            localIntent.putExtra("extra_pkgname", activity.getPackageName());
            activity.startActivity(localIntent);
        } catch (ActivityNotFoundException localActivityNotFoundException) {
            Log.d(TAG, "跳转失败，转到应用详情页");
            goIntentSetting(activity);
        }
    }

    private static void goMeizuMainager(Context activity) {
        try {
            Intent intent = new Intent("com.meizu.safe.security.SHOW_APPSEC");
            intent.addCategory(Intent.CATEGORY_DEFAULT);
            intent.putExtra("packageName", "xiang.settingpression");
            activity.startActivity(intent);
        } catch (ActivityNotFoundException localActivityNotFoundException) {
            Log.d(TAG, "跳转失败，转到应用详情页");
            localActivityNotFoundException.printStackTrace();
            goIntentSetting(activity);
        }
    }

    private static void goSangXinMainager(Context activity) {
        //三星4.3可以直接跳转
        goIntentSetting(activity);
    }

    private static void goIntentSetting(Context activity) {
        Intent intent = new Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS);
        Uri uri = Uri.fromParts("package", activity.getPackageName(), null);
        intent.setData(uri);
        try {
            activity.startActivity(intent);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static void goOppoMainager(Context activity) {
        doStartApplicationWithPackageName(activity, "com.coloros.safecenter");
    }

    /**
     * doStartApplicationWithPackageName("com.yulong.android.security:remote")
     * 和Intent open = getPackageManager().getLaunchIntentForPackage("com.yulong.android.security:remote");
     * startActivity(open);
     * 本质上没有什么区别，通过Intent open...打开比调用doStartApplicationWithPackageName方法更快，也是android本身提供的方法
     *
     * @param activity
     */
    private static void goCoolpadMainager(Context activity) {
        doStartApplicationWithPackageName(activity, "com.yulong.android.security:remote");
      /*  Intent openQQ = getPackageManager().getLaunchIntentForPackage("com.yulong.android.security:remote");
        startActivity(openQQ);*/
    }

    //vivo
    private static void goVivoMainager(Context activity) {
        doStartApplicationWithPackageName(activity, "com.bairenkeji.icaller");
     /*   Intent openQQ = getPackageManager().getLaunchIntentForPackage("com.vivo.securedaemonservice");
        startActivity(openQQ);*/
    }

    private static void doStartApplicationWithPackageName(Context activity, String packagename) {

        // 通过包名获取此APP详细信息，包括Activities、services、versioncode、name等等
        PackageInfo packageinfo = null;
        try {
            packageinfo = activity.getPackageManager().getPackageInfo(packagename, 0);
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        if (packageinfo == null) {
            return;
        }
        // 创建一个类别为CATEGORY_LAUNCHER的该包名的Intent
        Intent resolveIntent = new Intent(Intent.ACTION_MAIN, null);
        resolveIntent.addCategory(Intent.CATEGORY_LAUNCHER);
        resolveIntent.setPackage(packageinfo.packageName);
        // 通过getPackageManager()的queryIntentActivities方法遍历
        List<ResolveInfo> resolveinfoList = activity.getPackageManager()
                .queryIntentActivities(resolveIntent, 0);
        Log.i("MainActivity", "resolveinfoList" + resolveinfoList.size());
        for (int i = 0; i < resolveinfoList.size(); i++) {
            Log.i("MainActivity", resolveinfoList.get(i).activityInfo.packageName + resolveinfoList.get(i).activityInfo.name);
        }
        ResolveInfo resolveinfo = resolveinfoList.iterator().next();
        if (resolveinfo != null) {
            // packagename = 参数packname
            String packageName = resolveinfo.activityInfo.packageName;
            // 这个就是我们要找的该APP的LAUNCHER的Activity[组织形式：packagename.mainActivityname]
            String className = resolveinfo.activityInfo.name;
            // LAUNCHER Intent
            Intent intent = new Intent(Intent.ACTION_MAIN);
            intent.addCategory(Intent.CATEGORY_LAUNCHER);
            // 设置ComponentName参数1:packagename参数2:MainActivity路径
            ComponentName cn = new ComponentName(packageName, className);
            intent.setComponent(cn);
            try {
                activity.startActivity(intent);
            } catch (Exception e) {
                Log.d(TAG, "跳转失败，转到应用详情页");
                goIntentSetting(activity);
                e.printStackTrace();
            }
        }
    }

    /**
     * 判断是否是miui V5/V6,miui8的权限Activity是PermissionsEditorActivity，这里要注意了
     *
     * @param context
     * @return
     */
    public static boolean isMIUIv5v6(Context context) {
        boolean result = false;
        Intent localIntent = new Intent("miui.intent.action.APP_PERM_EDITOR");
        localIntent.setClassName("com.miui.securitycenter", "com.miui.permcenter.permissions.AppPermissionsEditorActivity");
        if (isIntentAvailable(context, localIntent)) {
            result = true;
        }
        return result;
    }

    /**
     * 检查是否有这个activity
     *
     * @param context
     * @param intent
     * @return
     */
    private static boolean isIntentAvailable(Context context, Intent intent) {
        PackageManager packageManager = context.getPackageManager();
        List<ResolveInfo> list = packageManager.queryIntentActivities(
                intent, PackageManager.GET_ACTIVITIES);
        return list.size() > 0;
    }

    public static boolean isPermissionGranted(Context activity, String... permissions) {
        if (permissions != null && permissions.length > 0) {
            for (String permission : permissions) {
                int hasPermission = ContextCompat.checkSelfPermission(activity, permission);
                if (hasPermission != PackageManager.PERMISSION_GRANTED) {
                    Log.i(TAG, permission + "没有授权");
                    return false;
                }
            }
        }
        return true;
    }

}

