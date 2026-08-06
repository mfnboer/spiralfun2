// Copyright (C) 2025 Michel de Boer
// License: GPLv3

package com.gmail.mfnboer;

import com.gmail.mfnboer.SpiralFunApplication;

import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.os.Build;
import android.os.PowerManager;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;
import android.view.WindowInsetsController;

public class ScreenUtils {
    private static final String LOGTAG = "ScreenUtils";
    private static Activity sActivity;
    private static PowerManager.WakeLock sWakeLock = null;

    public static void init(Activity activity) {
        sActivity = activity;
    }

    public static boolean mustEnableEdgeToEdge() {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.VANILLA_ICE_CREAM;
    }

    public static void setStatusBarTransparent(boolean transparent, int color, boolean isLightMode) {
        Log.d(LOGTAG, "Set status bar transparent: " + transparent + " color: " + color + " light: " + isLightMode);
        sActivity.runOnUiThread(new StatusBarSetter(sActivity, transparent, color, isLightMode));
    }

    private static class StatusBarSetter implements Runnable {
        private Activity mActivity;
        private boolean mTransparent;
        private int mColor;
        private boolean mIsLightMode;

        StatusBarSetter(Activity activity, boolean transparent, int color, boolean isLightMode) {
            mActivity = activity;
            mTransparent = transparent;
            mColor = color;
            mIsLightMode = isLightMode;
        }

        @Override
        public void run() {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                Window window = mActivity.getWindow();

                if (Build.VERSION.SDK_INT < Build.VERSION_CODES.VANILLA_ICE_CREAM) {
                    window.setStatusBarColor(mTransparent ? Color.TRANSPARENT : mColor);
                    window.setDecorFitsSystemWindows(!mTransparent);
                }

                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                    WindowInsetsController insetsController = window.getInsetsController();

                    if (insetsController != null)
                        insetsController.setSystemBarsAppearance(mIsLightMode ? WindowInsetsController.APPEARANCE_LIGHT_STATUS_BARS : 0, WindowInsetsController.APPEARANCE_LIGHT_STATUS_BARS);
                    else
                        Log.w(LOGTAG, "Cannot get window insets controller");
                }
            }
        }
    }

    public static void setStatusBarColor(int color, boolean isLightMode) {
        Log.d(LOGTAG, "Set status bar color: " + color + " light: " + isLightMode);
        sActivity.runOnUiThread(new StatusBarColorSetter(sActivity, color, isLightMode));
    }

    private static class StatusBarColorSetter implements Runnable {
        private Activity mActivity;
        private int mColor;
        private boolean mIsLightMode;

        StatusBarColorSetter(Activity activity, int color, boolean isLightMode) {
            mActivity = activity;
            mColor = color;
            mIsLightMode = isLightMode;
        }

        @Override
        public void run() {
            Window window = mActivity.getWindow();
            window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);

            if (Build.VERSION.SDK_INT < Build.VERSION_CODES.VANILLA_ICE_CREAM)
                window.setStatusBarColor(mColor);

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                WindowInsetsController insetsController = window.getInsetsController();

                if (insetsController != null)
                    insetsController.setSystemBarsAppearance(mIsLightMode ? WindowInsetsController.APPEARANCE_LIGHT_STATUS_BARS : 0, WindowInsetsController.APPEARANCE_LIGHT_STATUS_BARS);
                else
                    Log.w(LOGTAG, "Cannot get window insets controller");
            } else {
                window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
            }
        }
    }

    public static void setStatusBarLightMode(boolean isLightMode) {
        Log.d(LOGTAG, "Set status bar light mode: " + isLightMode);
        sActivity.runOnUiThread(new StatusBarLightModeSetter(sActivity, isLightMode));
    }

    private static class StatusBarLightModeSetter implements Runnable {
        private Activity mActivity;
        private boolean mIsLightMode;

        StatusBarLightModeSetter(Activity activity, boolean isLightMode) {
            mActivity = activity;
            mIsLightMode = isLightMode;
        }

        @Override
        public void run() {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                Window window = mActivity.getWindow();
                WindowInsetsController insetsController = window.getInsetsController();

                if (insetsController != null) {
                    insetsController.setSystemBarsAppearance(mIsLightMode ? WindowInsetsController.APPEARANCE_LIGHT_STATUS_BARS : 0, WindowInsetsController.APPEARANCE_LIGHT_STATUS_BARS);
                } else {
                    Log.w(LOGTAG, "Cannot get window insets controller");
                }
            }
        }
    }


    public static void setNavigationBarColor(int color, boolean isLightMode) {
        Log.d(LOGTAG, "Set navigation bar color: " + color + " light: " + isLightMode);
        sActivity.runOnUiThread(new NavigationBarColorSetter(sActivity, color, isLightMode));
    }

    private static class NavigationBarColorSetter implements Runnable {
        private Activity mActivity;
        private int mColor;
        private boolean mIsLightMode;

        NavigationBarColorSetter(Activity activity, int color, boolean isLightMode) {
            mActivity = activity;
            mColor = color;
            mIsLightMode = isLightMode;
        }

        @Override
        public void run() {
            Window window = mActivity.getWindow();

            // HACK: although setNavigationBarColor is deprecated in Vanilla Ice Cream
            // Samsung OneUI 7 still needs it to make the light/dark icons work
            if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.VANILLA_ICE_CREAM)
                window.setNavigationBarColor(mColor);

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                WindowInsetsController insetsController = window.getInsetsController();
                Log.d(LOGTAG, "Set navigation bar light: " + mIsLightMode);

                if (insetsController != null)
                    insetsController.setSystemBarsAppearance(mIsLightMode ? WindowInsetsController.APPEARANCE_LIGHT_NAVIGATION_BARS : 0, WindowInsetsController.APPEARANCE_LIGHT_NAVIGATION_BARS);
                else
                    Log.w(LOGTAG, "Cannot get window insets controller");
            }
        }
    }

    public static void setKeepScreenOn(boolean keepOn) {
        sActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Window window = sActivity.getWindow();
                if (window == null) {
                    Log.w(LOGTAG, "Cannot get window");
                    return;
                }

                Context context = SpiralFunApplication.getContext();
                if (context == null) {
                    Log.w(LOGTAG, "No context");
                    return;
                }

                PowerManager pm = (PowerManager)context.getSystemService(Context.POWER_SERVICE);

                if (keepOn) {
                    window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
                    if (sWakeLock == null) {
                        sWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "SpiralFunTag");
                        sWakeLock.acquire();
                    }
                } else {
                    window.clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
                    if (sWakeLock != null) {
                        sWakeLock.release();
                        sWakeLock = null;
                    }
                }
            }
        });
    }
}
