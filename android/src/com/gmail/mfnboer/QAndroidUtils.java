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
    private static final String LOGTAG = "spiralfun.QAndroidUtils";
    private static final String APP_NAME = "Spiral Fun";

    public static native void emitMediaScannerFinished(String uri);

    private static String getPath(String type, String subDir)
    {
        File path = Environment.getExternalStoragePublicDirectory(type);
        File appPath = new File(path, subDir);

        try {
            appPath.mkdirs();
        } catch (SecurityException e) {
            Log.w(LOGTAG, "Could not create path: " + appPath + " details: " + e.getMessage());
            return null;
        }

        return appPath.getAbsolutePath();
    }

    public static String getPicturesPath(String subDir) {
        return getPath(Environment.DIRECTORY_PICTURES, subDir);
    }

    public static String getSpiralConfigPath(String subDir) {
        return getPath(Environment.DIRECTORY_DOCUMENTS, subDir);
    }

    // Make a media file show up in the gallery
    public static void scanMediaFile(String fileName) {
        Log.d(LOGTAG, "Scan media file=" + fileName);

        MediaScannerConnection.scanFile(QtNative.getContext(),
                new String[]{ fileName }, null,
                new MediaScannerConnection.OnScanCompletedListener() {
                    public void onScanCompleted(String path, Uri uri) {
                        Log.d(LOGTAG, "Scanned " + path + ":");
                        Log.d(LOGTAG, "  uri=" + uri);
                        String uriString = (uri == null ? null : uri.toString());
                        emitMediaScannerFinished(uriString);
                    }
                }
            );
    }

    public static void sharePicture(String uriString, String configAppUri) {
        if (QtNative.activity() == null)
            return;

        Uri uri;
        try {
            uri = Uri.parse(uriString);
        } catch (Exception e) {
            Log.d(LOGTAG, "invalid uri");
            return;
        }

        if (uri == null) {
            Log.d(LOGTAG, "invalid uri");
            return;
        }

        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_SEND);
        intent.putExtra(Intent.EXTRA_SUBJECT, APP_NAME);
        intent.putExtra(Intent.EXTRA_TEXT,
            "Created with Spiral Fun\n" +
            "Click to start or get the app from the play store (Android only):\n" +
            configAppUri);
        intent.putExtra(Intent.EXTRA_STREAM, uri);
        intent.setType("image/jpg");
        QtNative.activity().startActivity(Intent.createChooser(intent, "Share spiral using"));
    }
}
