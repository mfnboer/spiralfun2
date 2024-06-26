// Copyright (C) 2023 Michel de Boer
// License: GPLv3

package com.gmail.mfnboer;

import org.qtproject.qt.android.QtNative;
import org.qtproject.qt.android.bindings.QtActivity;

import java.lang.String;

import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.os.PowerManager;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;

public class QSpiralFunActivity extends QtActivity {

    private static final String LOGTAG = "spiralfun.QSpiralFunActivity";
    private boolean mIsIntentPending = false;
    private boolean mIsReady = false;
    private PowerManager.WakeLock mWakeLock = null;

    public static native void emitViewUriReceived(String uri);

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(LOGTAG, "onCreate");

        Window window = this.getWindow();
        window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
        window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
        window.setStatusBarColor(Color.BLACK);

        Intent intent = getIntent();
        if (intent == null)
            return;

        String action = intent.getAction();
        if (action == null)
            return;

        Log.d(LOGTAG, "action: " + action);

        // App is starting up and not ready to receive intents.
        mIsIntentPending = true;
    }

    @Override
    public void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        Log.d(LOGTAG, "onNewIntent");

        if (intent == null)
            return;

        Log.d(LOGTAG, "action: " + intent.getAction());
        setIntent(intent);

        if (mIsReady)
            handleIntent();
        else
            mIsIntentPending = true;
    }

    public void handlePendingIntent() {
        Log.d(LOGTAG, "handlePendingIntent");
        mIsReady = true;

        if (mIsIntentPending) {
            mIsIntentPending = false;
            handleIntent();
        }
    }

    private void handleIntent() {
        Log.d(LOGTAG, "handleIntent");
        Intent intent = getIntent();

        if (intent.getAction() != "android.intent.action.VIEW")
        {
            Log.d(LOGTAG, "Received unknown intent action: " + intent.getAction());
            return;
        }

        Uri uri = intent.getData();
        Log.d(LOGTAG, "VIEW uri: " + uri);
        emitViewUriReceived(uri.toString());
    }

    public void setKeepScreenOn(boolean keepOn) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Window window = getWindow();
                if (window == null) {
                    Log.w(LOGTAG, "Cannot get window");
                    return;
                }

                Context context = QtNative.getContext();
                if (context == null) {
                    Log.w(LOGTAG, "No context");
                    return;
                }

                PowerManager pm = (PowerManager)context.getSystemService(Context.POWER_SERVICE);

                if (keepOn) {
                    window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
                    if (mWakeLock == null) {
                        mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "SpiralFunTag");
                        mWakeLock.acquire();
                    }
                } else {
                    window.clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
                    if (mWakeLock != null) {
                        mWakeLock.release();
                        mWakeLock = null;
                    }
                }
            }
        });
    }

    // Avoid the app to close when the user presses the back button.
    public void goToBack() {
        Log.d(LOGTAG, "Moving task to back");
        moveTaskToBack(true);
    }
}
