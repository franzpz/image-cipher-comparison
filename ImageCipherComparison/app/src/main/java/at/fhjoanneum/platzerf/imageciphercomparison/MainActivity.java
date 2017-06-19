package at.fhjoanneum.platzerf.imageciphercomparison;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.provider.ContactsContract;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;
import android.widget.ArrayAdapter;
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
    private EditText editText;
    private NumberPicker numberPicker;
    private NumberPicker internalroundspicker;

    private Spinner cipherselection;

    private Dictionary<String, String> filenamesToFullPath = new Hashtable<>();
    private Dictionary<String, ImageCipher> ciphers = new Hashtable<>();
    private List<String> cipherList = new ArrayList<>();
    private List<String> testfiles = new ArrayList<String>();
    private String basePath = "/sdcard/Download/testimages/";
    private ImageCipher selectedCipher;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // Example of a call to a native method
        upperText = (TextView) findViewById(R.id.sample_text);
        lowerText = (TextView) findViewById(R.id.textView);
        editText = (EditText) findViewById(R.id.editText);
        numberPicker = (NumberPicker) findViewById(R.id.numberPicker);
        internalroundspicker = (NumberPicker) findViewById(R.id.internalroundspicker);

        spinner = (Spinner)findViewById(R.id.spinner);
        cipherselection = (Spinner)findViewById(R.id.cipherselection);

        numberPicker.setMinValue(1);
        numberPicker.setMaxValue(150);
        internalroundspicker.setMinValue(1);
        internalroundspicker.setMaxValue(500);

        verifyStoragePermissions(this);

        ImageCipher one = new ImageCipher1();
        ImageCipher two = new ImageCipher2();
        ImageCipher oneGmp = new ImageCipher1Gmp();
        ImageCipher twoGmp = new ImageCipher2Gmp();
        ImageCipher aesJava = new AesJavaCipher();
        ImageCipher twoJava = new ImageCipher2Java();
        ImageCipher aesC = new AesCCipher();
        ciphers.put(one.getName(), one);
        ciphers.put(oneGmp.getName(), oneGmp);
        ciphers.put(two.getName(), two);
        ciphers.put(twoGmp.getName(), twoGmp);
        ciphers.put(aesJava.getName(), aesJava);
        ciphers.put(twoJava.getName(), twoJava);
        ciphers.put(aesC.getName(), aesC);
        selectedCipher = one;

        Enumeration<ImageCipher> x = ciphers.elements();
        while(x.hasMoreElements()){
            cipherList.add(x.nextElement().getName());
        }

        Collections.sort(cipherList);

        ArrayAdapter<String> a = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item);
        a.addAll(cipherList);
        cipherselection.setAdapter(a);

        editText.setText(basePath);
        numberPicker.setValue(1);

        loadFileList();
        spinner.requestFocus();

        //upperText.setText("Original Image Bytes: " + arrayToStringList(originalImageBytes));

        lowerText.setText("do something...");
    }

    void loadFileList(){
        basePath = editText.getText().toString();
        upperText.setText("Using Base Path: " + basePath);

        File directory = new File(basePath);
        if(!directory.isDirectory()) {
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
        for (int i = 0; i < files.length; i++)
        {
            filenamesToFullPath.put(files[i].getName(), files[i].getAbsolutePath());
            testfiles.add(files[i].getName());
        }

        Collections.sort(testfiles);

        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item);
        adapter.addAll(testfiles);
        spinner.setAdapter(adapter);

        CsvLogger.FlushLog();
        CsvLogger.InitLogger(this.getApplicationContext(), basePath);
    }

    public void onReloadFileList(View v) {
        loadFileList();
    }

    /*public void onShowProviders(View v) {
        String result = CipherProviderTests.policyTests();
        upperText.setText(result);
    }*/

    public void onSleepTest(View v) {
        upperText.setText("Starting wait for 30 rounds with 30 seconds per round");
        String result = WaitTester.waitFor(30);
        showText(result);
    }


    public void onTestAes(View v) {
        String result = CipherProviderTests.policyTests();
        CsvLogger.AddLine("test cipher aes 256bit", "encrypt", "Provider Test", 1, result);
        upperText.setText(result);
    }

    private class TestCycle extends AsyncTask<String, Integer, String> {

        @Override
        protected String doInBackground(String... params) {
            return null;
        }


    }

    private class DecryptTask extends AsyncTask<String, Integer, String> {

        private String tag = "decrypt";
        private int rounds;

        public DecryptTask(int rounds){
            this.rounds = rounds;
        }

        @Override
        protected String doInBackground(String... params) {
            String filename = params[0];
            String originalFile = filename.replace(".encrypted.png", "");
            String newFilename = originalFile + ".decrypted.png";

            CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "start reading files " + originalFile + "; " + filename);

            ImageConverter conv = new ImageConverter();
            ConvertedImage originalImage = conv.GetOrigImageInfo(originalFile);

            ConvertedImage encryptedImage = conv.ConvertFromArgbImage(filename);

            CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "end reading files " + originalFile + "; " + filename);

            CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "starting " + tag + " overall");

            long startTime = System.nanoTime();
            try {
                encryptedImage.ImageBytes = selectedCipher.decrypt(encryptedImage.ImageBytes, originalImage.SumOfBytes, rounds);
                /*for(int i = 0; i < rounds; i++) {
                    CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "starting " + tag + " round " + (i+1));
                    long startRound = System.nanoTime();
                    encryptedImage.ImageBytes = selectedCipher.decrypt(encryptedImage.ImageBytes, originalImage.SumOfBytes);
                    long endRound = System.nanoTime();
                    CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "done " + tag + " round " + (i+1) + " - took: " + (endRound-startRound) + " ns");
                }*/
            }
            catch (Exception e){
                return e.toString();
            }
            long endTime = System.nanoTime();

            CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "done " + tag + " overall - took: " + (endTime-startTime) + " ns");

            CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "start writing file " + newFilename);

            encryptedImage.Config = originalImage.Config;

            conv.saveArgbImage(encryptedImage, newFilename);

            CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "done writing file " + newFilename);

            CsvLogger.FlushLog();

            return newFilename + "\n Took: " + ((endTime - startTime)/1000000000.0) + " seconds";
        }

        @Override
        protected void onPostExecute(String result){
            showText("Decrypted successfully using " + selectedCipher.getName() + ": " + result);
        }
    }

    private class EncryptTask extends AsyncTask<String, String, String> {

        private final int internalrounds;
        private String tag = "encrypt";
        private int rounds;

        public EncryptTask(int rounds, int internalrounds){
            this.rounds = rounds;
            this.internalrounds = internalrounds;
        }

        protected void onProgressUpdate(String... progress) {
            appendText(progress[0]);
        }


        @Override
        protected String doInBackground(String... params) {
            String filename = params[0];
            String newFilename = filename + ".encrypted.png";

            CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "start reading file " + filename);

            ImageConverter conv = new ImageConverter();
            ConvertedImage image = conv.ConvertFromArgbImage(filename);

            CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "end reading file " + filename);

            /*int[] origBytes = new int[]{201, 40, 208, 214, 53, 221, 216, 50, 220,
                    200, 39, 207, 213, 52, 220, 216, 50, 220};

            long sum = 0;
            for(int i = 0; i < origBytes.length; i++) {
                sum += origBytes[i];
            }

            int [] encrypted = encryptImageBytes(origBytes, sum);

            int [] decrypted = decryptImageBytes(encrypted, sum);*/
            //image.ImageBytes = encryptImageBytes(image.ImageBytes, image.SumOfBytes);

            CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "starting " + tag + " overall");

            long sumMillisecondsOverall = 0;
            try {
                //image.ImageBytes = selectedCipher.encrypt(image.ImageBytes, image.SumOfBytes, rounds);
                for(int i = 0; i < rounds; i++) {

                    long sumMillisecondsPerRound = 0;
                    CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "starting " + tag + " round " + (i+1));

                    ImageCipher1 c = new ImageCipher1();
                    long[] measurements = c.runEncTest(image.ImageBytes, image.SumOfBytes, this.internalrounds);

                    for(int j = 0; j < internalrounds; j++){
                        CsvLogger.AddLine(c.getName(), tag, filename, i, "internal round took " + measurements[j] + " ms ");
                        sumMillisecondsPerRound += measurements[j];
                    }

                    sumMillisecondsOverall += sumMillisecondsPerRound;

                    publishProgress("in round " + (i+1) + " and it took " + sumMillisecondsPerRound + " ms");

                    CsvLogger.FlushLog();

                    //image.ImageBytes = selectedCipher.encrypt(image.ImageBytes, image.SumOfBytes, rounds);
                //image.ImageBytes = selectedCipher.decrypt(image.ImageBytes, image.SumOfBytes);

                    CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "done " + tag + " round " + (i+1) + " - took: " + sumMillisecondsPerRound + " ms");
                }
            }
            catch (Exception e){
                return e.toString();
            }

            CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "done " + tag + " overall - took: " + sumMillisecondsOverall + " ms");

            CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "start writing file " + newFilename);

            //conv.saveArgbImage(image, newFilename);

            CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "done writing file " + newFilename);

            CsvLogger.FlushLog();

            return newFilename + "\n Took: " + sumMillisecondsOverall + " ms";
        }

        @Override
        protected void onPostExecute(String result){
            showText("Encrypted successfully using " + selectedCipher.getName() + ": " + result);
        }
    }

    public void onDecrypt(View v) {
        selectedCipher = ciphers.get(cipherselection.getSelectedItem().toString());
        DecryptTask t = new DecryptTask(numberPicker.getValue());
        upperText.setText("number of rounds: "+ numberPicker.getValue());
        showText("started decrypting using " + selectedCipher.getName());
        t.execute(filenamesToFullPath.get(spinner.getSelectedItem().toString()));
    }

    public void onEncrypt(View v) {
        selectedCipher = ciphers.get(cipherselection.getSelectedItem().toString());
        EncryptTask t = new EncryptTask(numberPicker.getValue(), internalroundspicker.getValue());
        upperText.setText("number of rounds: "+ numberPicker.getValue());
        showText("started encrypting using " + selectedCipher.getName());
        t.execute(filenamesToFullPath.get(spinner.getSelectedItem().toString()));
    }

    public void showText(String text){
        lowerText.setText(text);
    }

    public void appendText(String text){
        lowerText.setText(lowerText.getText() + "\n" + text);
    }
}
