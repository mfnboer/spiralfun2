// Copyright (C) 2023 Michel de Boer
// License: GPLv3

package com.gmail.mfnboer;

import org.qtproject.qt.android.QtNative;

import java.io.File;
import java.lang.String;
import android.content.Intent;
import android.media.MediaScannerConnection;
import android.net.Uri;
import android.os.Environment;
import android.util.Log;

public class QAndroidUtils
{
    private static final String TAG = "QAndroidUtils";
    private static final String APP_DIR = "SpiralFun";
    private static final String APP_NAME = "Spiral Fun";

    public static String getPicturesPath() {
        File path = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES);
        File appPath = new File(path, APP_DIR);

        try {
            appPath.mkdirs();
        } catch (SecurityException e) {
            Log.w(TAG, "Could not create path: " + appPath + " details: " + e.getMessage());
            return null;
        }

        return appPath.getAbsolutePath();
    }

    // Make a media file show up in the gallery
    public static void scanMediaFile(String fileName, boolean share) {
        Log.d(TAG, "Scan media file=" + fileName + " share=" + share);

        MediaScannerConnection.scanFile(QtNative.getContext(),
                new String[]{ fileName }, null,
                share ? new MediaScannerConnection.OnScanCompletedListener() {
                    public void onScanCompleted(String path, Uri uri) {
                        Log.d(TAG, "Scanned " + path + ":");
                        Log.d(TAG, "-> uri=" + uri);
                        sharePicture(uri);
                    }
                } : null
            );
    }

    public static void sharePicture(Uri uri) {
        if (QtNative.activity() == null)
            return;

        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_SEND);
        intent.putExtra(Intent.EXTRA_SUBJECT, APP_NAME);
        intent.putExtra(Intent.EXTRA_TEXT,
            "[TEST] Created with Spiral Fun\n" +
            "Get app for Android: https://play.google.com/store/apps/details?id=com.gmail.mfnboer.spiralfun");
        intent.putExtra(Intent.EXTRA_STREAM, uri);
        intent.setType("image/jpg");
        QtNative.activity().startActivity(Intent.createChooser(intent, "Share spiral using"));
    }
}
