package at.fhjoanneum.platzerf.imageciphercomparison;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.AsyncTask;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    // Storage Permissions
    private static final int REQUEST_EXTERNAL_STORAGE = 1;
    private static String[] PERMISSIONS_STORAGE = {
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE
    };
    private Spinner spinner;

    /**
     * Checks if the app has permission to write to device storage
     *
     * If the app does not has permission then the user will be prompted to grant permissions
     *
     * @param activity
     */
    public static void verifyStoragePermissions(Activity activity) {
        // Check if we have write permission
        int permission = ActivityCompat.checkSelfPermission(activity, Manifest.permission.WRITE_EXTERNAL_STORAGE);

        if (permission != PackageManager.PERMISSION_GRANTED) {
            // We don't have permission so prompt the user
            ActivityCompat.requestPermissions(
                    activity,
                    PERMISSIONS_STORAGE,
                    REQUEST_EXTERNAL_STORAGE
            );
        }
    }

    private TextView upperText;
    private TextView lowerText;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private int[] originalImageBytes;

    private long sumOfImageBytes = 0;

    private int[] encryptedImageBytes;

    private int size = 50;
    private String filename = "/sdcard/Download/testimages/" + (size*5) + "x" + (size*5) + "image.jpg";
    private int height;
    private int width;
    private Bitmap.Config config;

    private List<String> testfiles = new ArrayList<String>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        upperText = (TextView) findViewById(R.id.sample_text);

        verifyStoragePermissions(this);

        File directory = new File("/sdcard/Download/testimages/");
        File[] files = directory.listFiles();
        for (int i = 0; i < files.length; i++)
        {
            testfiles.add(files[i].getAbsolutePath());
        }

        spinner = (Spinner)findViewById(R.id.spinner);

        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item);
        adapter.addAll(testfiles);
        spinner.setAdapter(adapter);



        //upperText.setText("Original Image Bytes: " + arrayToStringList(originalImageBytes));

        lowerText = (TextView) findViewById(R.id.textView);
        lowerText.setText("do something...");
    }

    void saveRgbIntArrayToImage(int[] convertedImage, String newFilename) {

        Bitmap image = Bitmap.createBitmap(width, height, config);

        int pos = 0;
        for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {
                image.setPixel(
                    x,
                    y,
                    Color.rgb(
                        convertedImage[pos*3],
                        convertedImage[pos*3+1],
                        convertedImage[pos*3+2])
                );
                pos++;
            }
        }

        FileOutputStream out = null;
        try {
            out = new FileOutputStream(newFilename);
            image.compress(Bitmap.CompressFormat.PNG, 100, out); // bmp is your Bitmap instance
            // PNG is a lossless format, the compression factor (100) is ignored
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (out != null) {
                    out.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }



    public void onLoadImage(View v) {
    }

    public void onDecrypt(View v) {
        int[] decryptImageBytes = decryptImageBytes(encryptedImageBytes, sumOfImageBytes);

        saveRgbIntArrayToImage(decryptImageBytes, filename + ".decrypted.jpg");

        lowerText.setText("Decrypted successfully: " + arrayToStringList(decryptImageBytes));
    }

    private String arrayToStringList(int[] array) {
        StringBuilder s = new StringBuilder();
        s.append("[");
        for(int i = 0; i < 5; i++)
            s.append(array[i]).append((i < array.length - 1 ? ", " : ""));
        s.append("]");
        return s.toString();
    }

    private class EncryptTask extends AsyncTask<String, Integer, String> {

        private int[] originalImageBytes;

        private long sumOfImageBytes = 0;

        private int[] encryptedImageBytes;
        private int height;
        private int width;
        private Bitmap.Config config;

        @Override
        protected String doInBackground(String... params) {
            String filename = params[0];

            loadImageIntoRgbIntArray(filename);

            encryptedImageBytes = encryptImageBytes(originalImageBytes, sumOfImageBytes);

            saveRgbIntArrayToImage(encryptedImageBytes, filename + ".encrypted.jpg");

            return filename + ".encrypted.jpg";
        }

        void loadImageIntoRgbIntArray(String path){
            Bitmap image = BitmapFactory.decodeFile(path);

            config = image.getConfig();
            height = image.getHeight();
            width = image.getWidth();
            originalImageBytes = new int[image.getHeight()*image.getWidth()*3];

            int pos = 0;
            for(int y = 0; y < image.getHeight(); y++) {
                for(int x = 0; x < image.getWidth(); x++) {
                    int pixel = image.getPixel(x, y);
                    originalImageBytes[pos*3] = Color.red(pixel);
                    originalImageBytes[pos*3+1] = Color.green(pixel);
                    originalImageBytes[pos*3+2] = Color.blue(pixel);
                    pos++;
                }
            }

            encryptedImageBytes = new int[originalImageBytes.length];

            for(int i = 0; i < originalImageBytes.length; i++){
                sumOfImageBytes += originalImageBytes[i];
            }
        }

        void saveRgbIntArrayToImage(int[] convertedImage, String newFilename) {

            Bitmap image = Bitmap.createBitmap(width, height, config);

            int pos = 0;
            for(int y = 0; y < height; y++) {
                for(int x = 0; x < width; x++) {
                    image.setPixel(
                            x,
                            y,
                            Color.rgb(
                                    convertedImage[pos*3],
                                    convertedImage[pos*3+1],
                                    convertedImage[pos*3+2])
                    );
                    pos++;
                }
            }

            FileOutputStream out = null;
            try {
                out = new FileOutputStream(newFilename);
                image.compress(Bitmap.CompressFormat.JPEG, 100, out); // bmp is your Bitmap instance
                // PNG is a lossless format, the compression factor (100) is ignored
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
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

    public void onEncrypt(View v) {
        EncryptTask t = new EncryptTask();
        Toast.makeText(this, "start encrypting", Toast.LENGTH_LONG).show();
        String file = t.doInBackground(spinner.getSelectedItem().toString());
        Toast.makeText(this, "done encrypting", Toast.LENGTH_LONG).show();

        lowerText.setText("Encrypted successfully: " + file);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native int[] encryptImageBytes(int[] originalImageBytes, long sumOfImageBytes);
    public native int[] decryptImageBytes(int[] originalImageBytes, long sumOfImageBytes);
}
