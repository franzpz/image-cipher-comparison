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
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
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
    private String filename = "/sdcard/Download/testimages/" + (size*5) + "x" + (size*5) + "image.png";
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

        loadFileList();

        //upperText.setText("Original Image Bytes: " + arrayToStringList(originalImageBytes));

        lowerText = (TextView) findViewById(R.id.textView);
        lowerText.setText("do something...");
    }

    void loadFileList(){
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
    }

    public void onReloadFileList(View v) {
        loadFileList();
    }

    public void onDecrypt(View v) {
        DecryptTask t = new DecryptTask();
        Toast.makeText(this, "start decrypting", Toast.LENGTH_LONG).show();
        String file = t.doInBackground(spinner.getSelectedItem().toString());
        Toast.makeText(this, "done decrypting", Toast.LENGTH_LONG).show();

        lowerText.setText("Decrypted successfully: " + file);
    }

    private String arrayToStringList(int[] array) {
        StringBuilder s = new StringBuilder();
        s.append("[");
        for(int i = 0; i < 5; i++)
            s.append(array[i]).append((i < array.length - 1 ? ", " : ""));
        s.append("]");
        return s.toString();
    }

    private class DecryptTask extends AsyncTask<String, Integer, String> {

        @Override
        protected String doInBackground(String... params) {
            String filename = params[0];
            String originalFile = filename.replace(".encrypted.png", "");
            String newFilename = filename + ".decrypted.png";

            ImageConverter conv = new ImageConverter();
            ConvertedImage originalImage = conv.GetOrigImageInfo(originalFile);

            ConvertedImage encryptedImage = conv.ConvertFromArgbImage(filename);

            encryptedImage.ImageBytes = encryptImageBytes(encryptedImage.ImageBytes, originalImage.SumOfBytes);

            conv.saveArgbImage(encryptedImage, newFilename);

            return newFilename;
        }
    }

    private class EncryptTask extends AsyncTask<String, Integer, String> {

        @Override
        protected String doInBackground(String... params) {
            String filename = params[0];
            String newFilename = filename + ".encrypted.png";

            ImageConverter conv = new ImageConverter();
            ConvertedImage image = conv.ConvertFromArgbImage(filename);

            image.ImageBytes = encryptImageBytes(image.ImageBytes, image.SumOfBytes);

            conv.saveArgbImage(image, newFilename);

            return newFilename;
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
