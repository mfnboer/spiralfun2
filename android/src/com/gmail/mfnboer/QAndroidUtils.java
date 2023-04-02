// Copyright (C) 2023 Michel de Boer
// License: GPLv3

package com.gmail.mfnboer;

import org.qtproject.qt.android.QtNative;

import java.io.File;
import java.lang.String;
import android.content.Context;
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

    private static String getExternalPublicPath(String type, String subDir)
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
        return getExternalPublicPath(Environment.DIRECTORY_PICTURES, subDir);
    }

    public static String getPublicSpiralConfigPath(String subDir) {
        return getExternalPublicPath(Environment.DIRECTORY_DOCUMENTS, subDir);
    }

    public static String getSpiralConfigPath(String subDir) {
        Context context = QtNative.getContext();
        if (context == null)
        {
            Log.w(LOGTAG, "No context");
            return null;
        }

        File path = context.getFilesDir();
        File configPath = new File(path, subDir);

        try {
            configPath.mkdirs();
        } catch (SecurityException e) {
            Log.w(LOGTAG, "Could not create path: " + configPath + " details: " + e.getMessage());
            return null;
        }

        return configPath.getAbsolutePath();
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

    public static void sharePicture(String uriString, String configAppUri, String mimeType) {
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
            configAppUri);
        intent.putExtra(Intent.EXTRA_STREAM, uri);
        intent.setType(mimeType);
        intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        QtNative.activity().startActivity(Intent.createChooser(intent, "Share spiral using"));
    }
}
