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
    public static void scanMediaFile(String fileName, boolean share, String configAppUri) {
        Log.d(LOGTAG, "Scan media file=" + fileName + " share=" + share);

        MediaScannerConnection.scanFile(QtNative.getContext(),
                new String[]{ fileName }, null,
                share ? new MediaScannerConnection.OnScanCompletedListener() {
                    public void onScanCompleted(String path, Uri uri) {
                        Log.d(LOGTAG, "Scanned " + path + ":");
                        Log.d(LOGTAG, "-> uri=" + uri);
                        sharePicture(uri, configAppUri);
                    }
                } : null
            );
    }

    public static void sharePicture(Uri uri, String configAppUri) {
        if (QtNative.activity() == null)
            return;

        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_SEND);
        intent.putExtra(Intent.EXTRA_SUBJECT, APP_NAME);
        intent.putExtra(Intent.EXTRA_TEXT,
            "[TEST] Created with Spiral Fun\n" +
            "[TEST] Click to start or download the app (Android only):\n" +
            configAppUri);
        intent.putExtra(Intent.EXTRA_STREAM, uri);
        intent.setType("image/jpg");
        QtNative.activity().startActivity(Intent.createChooser(intent, "Share spiral using"));
    }
}
