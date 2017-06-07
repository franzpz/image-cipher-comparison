package at.fhjoanneum.platzerf.imageciphercomparison;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import java.util.Arrays;

public class MainActivity extends AppCompatActivity {

    private TextView upperText;
    private TextView lowerText;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private int[] originalImageBytes = new int[]{
            201, 40, 208, 200, 39, 207, 214, 53, 221,
            213, 52, 220, 216, 50, 220, 216, 50, 220
    };

    private int[] encryptedImageBytes = new int[originalImageBytes.length];

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        upperText = (TextView) findViewById(R.id.sample_text);
        upperText.setText("Original Image Bytes: " + arrayToStringList(originalImageBytes));


        lowerText = (TextView) findViewById(R.id.textView);
        lowerText.setText("do something...");
    }

    public void onDecrypt() {
        encryptedImageBytes = decryptImageBytes(originalImageBytes);

        lowerText.setText("Decrypted successfully: " + arrayToStringList(encryptedImageBytes));
    }

    private String arrayToStringList(int[] array) {
        StringBuilder s = new StringBuilder();
        s.append("[");
        for(int i = 0; i < array.length; i++)
            s.append(array[i]).append((i < array.length - 1 ? ", " : ""));
        s.append("]");
        return s.toString();
    }

    public void onEncrypt() {


        encryptedImageBytes = encryptImageBytes(originalImageBytes);

        lowerText.setText("Encrypted successfully: " + arrayToStringList(encryptedImageBytes));
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native int[] encryptImageBytes(int[] originalImageBytes);
    public native int[] decryptImageBytes(int[] originalImageBytes);
}
