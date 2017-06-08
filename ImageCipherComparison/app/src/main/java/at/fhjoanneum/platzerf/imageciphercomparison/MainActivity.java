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
import java.util.Dictionary;
import java.util.Hashtable;
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

    private Dictionary<String, String> filenamesToFullPath = new Hashtable<>();
    private List<String> testfiles = new ArrayList<String>();
    private String basePath = "/sdcard/Download/testimages/";

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
        upperText.setText("Using Base Path: " + basePath);

        File directory = new File(basePath);
        File[] files = directory.listFiles();

        testfiles.clear();
        filenamesToFullPath = new Hashtable<>();
        for (int i = 0; i < files.length; i++)
        {
            filenamesToFullPath.put(files[i].getName(), files[i].getAbsolutePath());
            testfiles.add(files[i].getName());
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

    private class DecryptTask extends AsyncTask<String, Integer, String> {

        @Override
        protected String doInBackground(String... params) {
            String filename = params[0];
            String originalFile = filename.replace(".encrypted.png", "");
            String newFilename = originalFile + ".decrypted.png";

            ImageConverter conv = new ImageConverter();
            ConvertedImage originalImage = conv.GetOrigImageInfo(originalFile);

            ConvertedImage encryptedImage = conv.ConvertFromArgbImage(filename);

            long startTime = System.nanoTime();
            try {
                encryptedImage.ImageBytes = decryptImageBytes(encryptedImage.ImageBytes, originalImage.SumOfBytes);
            }
            catch (Exception e){
                return e.toString();
            }
            long endTime = System.nanoTime();

            encryptedImage.Config = originalImage.Config;

            conv.saveArgbImage(encryptedImage, newFilename);

            return newFilename + "\n Took: " + ((endTime - startTime)/1000000000.0) + " seconds";
        }

        @Override
        protected void onPostExecute(String result){
            showText("Decrypted successfully: " + result);
        }
    }

    private class EncryptTask extends AsyncTask<String, Integer, String> {

        @Override
        protected String doInBackground(String... params) {
            String filename = params[0];
            String newFilename = filename + ".encrypted.png";

            ImageConverter conv = new ImageConverter();
            ConvertedImage image = conv.ConvertFromArgbImage(filename);

            /*int[] origBytes = new int[]{201, 40, 208, 214, 53, 221, 216, 50, 220,
                    200, 39, 207, 213, 52, 220, 216, 50, 220};

            long sum = 0;
            for(int i = 0; i < origBytes.length; i++) {
                sum += origBytes[i];
            }

            int [] encrypted = encryptImageBytes(origBytes, sum);

            int [] decrypted = decryptImageBytes(encrypted, sum);*/
            //image.ImageBytes = encryptImageBytes(image.ImageBytes, image.SumOfBytes);

            long startTime = System.nanoTime();
            try {
                image.ImageBytes = encryptImageBytes(image.ImageBytes, image.SumOfBytes);
            }
            catch (Exception e){
                return e.toString();
            }
            long endTime = System.nanoTime();

            conv.saveArgbImage(image, newFilename);

            return newFilename + "\n Took: " + ((endTime - startTime)/1000000000.0) + " seconds";
        }

        @Override
        protected void onPostExecute(String result){
            showText("Encrypted successfully: " + result);
        }
    }

    public void onDecrypt(View v) {
        DecryptTask t = new DecryptTask();
        showText("started decrypting");
        t.execute(filenamesToFullPath.get(spinner.getSelectedItem().toString()));
    }

    public void onEncrypt(View v) {
        EncryptTask t = new EncryptTask();
        showText("started encrypting");
        t.execute(filenamesToFullPath.get(spinner.getSelectedItem().toString()));
    }

    public void showText(String text){
        lowerText.setText(text);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native int[] encryptImageBytes(int[] originalImageBytes, long sumOfImageBytes);
    public native int[] decryptImageBytes(int[] originalImageBytes, long sumOfImageBytes);
}
