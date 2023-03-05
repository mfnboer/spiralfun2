// Copyright (C) 2023 Michel de Boer
// License: GPLv3

package com.gmail.mfnboer;

import org.qtproject.qt.android.QtNative;
import org.qtproject.qt.android.bindings.QtActivity;

import java.lang.String;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;

public class QSpiralFunActivity extends QtActivity {

    private static final String LOGTAG = "spiralfun.QSpiralFunActivity";
    private boolean mIsIntentPending = false;
    private boolean mIsReady = false;

    public static native void emitViewUriReceived(String uri);

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(LOGTAG, "onCreate");

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
}
