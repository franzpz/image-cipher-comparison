package at.fhjoanneum.platzerf.imageciphercomparison;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    private TextView upperText;
    private TextView lowerText;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private int[] originalImageBytes;

    private long sumOfImageBytes = 0;

    private int[] encryptedImageBytes;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        upperText = (TextView) findViewById(R.id.sample_text);

        Bitmap image = BitmapFactory.decodeFile("/sdcard/Download/2x3image.jpg");

        int[] pixels = new int[image.getHeight()*image.getWidth()*3];

        int pos = 0;
        for(int y = 0; y < image.getHeight(); y++) {
            for(int x = 0; x < image.getWidth(); x++) {
                int pixel = image.getPixel(x, y);
                pixels[pos*3] = Color.red(pixel);
                pixels[pos*3+1] = Color.green(pixel);
                pixels[pos*3+2] = Color.blue(pixel);
                pos++;
            }
        }

        originalImageBytes = pixels;
        encryptedImageBytes = new int[originalImageBytes.length];

        for(int i = 0; i < originalImageBytes.length; i++){
            sumOfImageBytes += originalImageBytes[i];
        }

        upperText.setText("Original Image Bytes: " + arrayToStringList(originalImageBytes));

        lowerText = (TextView) findViewById(R.id.textView);
        lowerText.setText("do something...");
    }

    void

    public void onDecrypt(View v) {
        int[] decryptImageBytes = decryptImageBytes(encryptedImageBytes, sumOfImageBytes);

        lowerText.setText("Decrypted successfully: " + arrayToStringList(decryptImageBytes));
    }

    private String arrayToStringList(int[] array) {
        StringBuilder s = new StringBuilder();
        s.append("[");
        for(int i = 0; i < array.length; i++)
            s.append(array[i]).append((i < array.length - 1 ? ", " : ""));
        s.append("]");
        return s.toString();
    }

    public void onEncrypt(View v) {
        encryptedImageBytes = encryptImageBytes(originalImageBytes, sumOfImageBytes);

        lowerText.setText("Encrypted successfully: " + arrayToStringList(encryptedImageBytes));
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native int[] encryptImageBytes(int[] originalImageBytes, long sumOfImageBytes);
    public native int[] decryptImageBytes(int[] originalImageBytes, long sumOfImageBytes);
}
