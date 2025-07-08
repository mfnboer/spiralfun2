// Copyright (C) 2025 Michel de Boer
// License: GPLv3

package com.gmail.mfnboer;

import org.qtproject.qt.android.bindings.QtApplication;

import android.app.Application;
import android.content.Context;
import android.util.Log;

public class SpiralFunApplication extends QtApplication {
    private static final String LOGTAG = "SpiralFunApplication";

    private static Application sApplication = null;

    @Override
    public void onCreate() {
        super.onCreate();
        sApplication = this;
    }

    public static Context getContext() {
        return sApplication.getApplicationContext();
    }
}
