package at.fhjoanneum.platzerf.imageciphercomparison;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.SystemClock;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.NumberPicker;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.FileFilter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Dictionary;
import java.util.Enumeration;
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
     * <p>
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

    private TextView lowerText;
    private EditText editText;
    private NumberPicker numberPicker;
    private NumberPicker internalroundspicker;

    private Dictionary<String, String> filenamesToFullPath = new Hashtable<>();
    private List<String> testfiles = new ArrayList<String>();
    private String basePath = "/sdcard/Download/testimages/";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // Example of a call to a native method
        lowerText = (TextView) findViewById(R.id.textView);
        editText = (EditText) findViewById(R.id.editText);
        numberPicker = (NumberPicker) findViewById(R.id.numberPicker);
        internalroundspicker = (NumberPicker) findViewById(R.id.internalroundspicker);

        spinner = (Spinner) findViewById(R.id.spinner);

        numberPicker.setMinValue(1);
        numberPicker.setMaxValue(150);
        internalroundspicker.setMinValue(1);
        internalroundspicker.setMaxValue(500);

        verifyStoragePermissions(this);

        editText.setText(basePath);
        numberPicker.setValue(1);

        loadFileList();
        spinner.requestFocus();

        lowerText.setText("do something...");
    }

    void loadFileList() {
        basePath = editText.getText().toString();
        prependText("Using Base Path: " + basePath);

        File directory = new File(basePath);
        if (!directory.isDirectory()) {
            Toast.makeText(this, "directory does not exist", Toast.LENGTH_LONG).show();
            return;
        }

        File[] files = directory.listFiles(new FileFilter() {
            @Override
            public boolean accept(File pathname) {
                return pathname.getName().endsWith(".png");
            }
        });

        testfiles.clear();
        filenamesToFullPath = new Hashtable<>();
        for (int i = 0; i < files.length; i++) {
            filenamesToFullPath.put(files[i].getName(), files[i].getAbsolutePath());
            testfiles.add(files[i].getName());
        }

        Collections.sort(testfiles);

        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item);
        adapter.addAll(testfiles);
        spinner.setAdapter(adapter);
    }

    public void onReloadFileList(View v) {
        loadFileList();
    }

    public void onRunAll(View v) {
        TestConfig config = new TestConfig();
        config.Functions.add(Function.Encrypt);
        config.Functions.add(Function.Decrypt);
        run(config);
    }

    public void onRunEnc(View v) {

        /*String path = getSelectedImages().get(0);
        AesJavaCipher c = new AesJavaCipher();
        c.WriteEncryptedToFileSystem(path);
        return;*/

        TestConfig config = new TestConfig();
        config.Functions.add(Function.Encrypt);
        run(config);

    }

    public void onRunDec(View v) {
        /*String path = getSelectedImages().get(0);
        AesJavaCipher c = new AesJavaCipher();
        c.WriteDecryptedToFileSystem(path);
        return;*/


        TestConfig config = new TestConfig();
        config.Functions.add(Function.Decrypt);
        run(config);

    }

    public void run(TestConfig config){
        config.Ciphers.addAll(getSelectedCiphers());
        config.PauseBetweenFunctionsInSeconds = 20;
        config.PauseBetweenCiphersInSeconds = 15;
        config.PauseBetweenExtRounds = 12;

        config.NumberOfIntRoundsToRun = internalroundspicker.getValue();
        config.NumberOfExtRoundsToRun = numberPicker.getValue();

        config.Image = getSelectedImages().get(0);

        TestRunner runner = new TestRunner(config);

        runner.output.Writers.add(new SimpleFileLogger(this));
        runner.output.Writers.add(new Writer() {
            @Override
            public void Write(String text) {
                prependText(text);
            }

            @Override
            public void Flush() {

            }
        });

        runner.execute();
    }

    private List<String> getSelectedImages(){
        List<String> images = new ArrayList<>();
        images.add(filenamesToFullPath.get(spinner.getSelectedItem().toString()));
        return images;
    }

    private List<ImageCipher> getSelectedCiphers(){
        List<ImageCipher> imageCiphersToRun = new ArrayList<>();

        if(((CheckBox)this.findViewById(R.id.aesC)).isChecked()){
            imageCiphersToRun.add(new AesCCipher());
        }
        if(((CheckBox)this.findViewById(R.id.aesJava)).isChecked()){
            imageCiphersToRun.add(new AesJavaCipher());
        }
        if(((CheckBox)this.findViewById(R.id.imageCipher1)).isChecked()){
            imageCiphersToRun.add(new ImageCipher1());
        }
        if(((CheckBox)this.findViewById(R.id.imageCipher2)).isChecked()){
            imageCiphersToRun.add(new ImageCipher2());
        }

        if(((CheckBox)this.findViewById(R.id.imageCipher1gmp)).isChecked()){
            imageCiphersToRun.add(new ImageCipher1Gmp());
        }
        if(((CheckBox)this.findViewById(R.id.imageCipher2gmp)).isChecked()){
            imageCiphersToRun.add(new ImageCipher2Gmp());
        }

        return imageCiphersToRun;
    }

    public void onSleepTest(View v) {
        showText("Starting wait for 30 rounds with 30 seconds per round");
        String result = WaitTester.waitFor(30);
        prependText(result);
    }

    public void onTestAes(View v) {
        String result = CipherProviderTests.policyTests();
        prependText(result);
    }

    public void showText(String text){
        lowerText.setText(text);
    }

    public void prependText(String text){
        if(lowerText.getText().length() > 300)
            lowerText.setText("");

        lowerText.setText(  text + "\n" + lowerText.getText());
    }
}
