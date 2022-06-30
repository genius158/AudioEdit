package com.yan.audioedit;

import android.content.Context;
import android.content.res.AssetManager;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * @author yanxianwei
 */
public class Assets2Sdcard {

    private final AssetManager am;

    private static final String ASSET_MUSIC_DIR = "music";

    public static Context ctx;

    public Assets2Sdcard(Context ctx) {
        this.am = ctx.getAssets();
    }


    public static File getMusicDir() {
        return new File(ctx.getExternalFilesDir(null), ASSET_MUSIC_DIR);
    }

    public void copyMusic(){
        copy(ctx.getExternalFilesDir(null), ASSET_MUSIC_DIR);
    }

    public void execute() {
        copy(ctx.getExternalFilesDir(null), ASSET_MUSIC_DIR);
    }

    private void copy(File samiFile, String path) {
        File resDir = new File(samiFile, path);
        if (!resDir.exists()) {
            resDir.mkdirs();
        }
        String[] files;
        try {
            files = am.list(path);
        } catch (IOException e) {
            return;
        }

        String totalPath;
        InputStream in = null;
        OutputStream out = null;

        for (String name : files) {
            totalPath = path + File.separator + name;

            try {
                in = am.open(totalPath);
                File dir = new File(samiFile.getAbsolutePath() + File.separator + path);
                if (!dir.exists()) {
                    dir.mkdirs();
                }
                File outFile = new File(dir.getAbsolutePath(), name);
                if (outFile.exists()) {
                    continue;
                }

                File tmpOutFile = new File(dir.getAbsolutePath(), name + "tmp");
                out = new FileOutputStream(tmpOutFile);
                copyFile(in, out);
                out.flush();
                tmpOutFile.renameTo(outFile);

            } catch (IOException e) {
                copy(samiFile, totalPath);
            } finally {
                try {
                    if (in != null) {
                        in.close();
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }

                try {
                    if (out != null) {
                        out.close();
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private void copyFile(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[8192];
        int read;
        while ((read = in.read(buffer)) != -1) {
            out.write(buffer, 0, read);
        }
    }

}
