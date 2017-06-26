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

import static android.os.Process.THREAD_PRIORITY_BACKGROUND;
import static android.os.Process.THREAD_PRIORITY_MORE_FAVORABLE;
import android.os.Process;

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
        while (x.hasMoreElements()) {
            cipherList.add(x.nextElement().getName());
        }

        Collections.sort(cipherList);

        ArrayAdapter<String> a = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item);
        a.addAll(cipherList);
        //cipherselection.setAdapter(a);

        editText.setText(basePath);
        numberPicker.setValue(1);

        loadFileList();
        spinner.requestFocus();

        //upperText.setText("Original Image Bytes: " + arrayToStringList(originalImageBytes));

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

    public void onRunAll(View v) {
        run(3);
    }

    public void onRunEnc(View v) {
        run(1);
    }

    public void onRunDec(View v) {
        run(2);
    }

    private void run(int types) { // 1 = enc, 2 = dec, 3 = both
        final int externalRounds = numberPicker.getValue();
        final int internalRounds = internalroundspicker.getValue();
        //lowerText.setText("");
        //showText("Starting run all for " + externalRounds + "x" + internalRounds);

        ((Activity) this).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ((EditText) editText).setText("Starting run all for " + externalRounds + "x" + internalRounds);
            }
        });

        showText("Started with Run All");

        new LongAsyncRunner(getSelectedCiphers(), getSelectedImages())
                .doInBackground(externalRounds, internalRounds, types); // run both enc/dec
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

        return imageCiphersToRun;
    }

    public class LongAsyncRunner extends AsyncTask<Integer, String, String> {

        private List<ImageCipher> imageCiphersToRun;
        private List<String> imagesToRun;

        public LongAsyncRunner(List<ImageCipher> imageCiphers, List<String> images){
            imageCiphersToRun = imageCiphers;
            imagesToRun = images;
        }

        @Override
        protected void onProgressUpdate(String... progress) {
            prependText(progress[0]);
        }

        @Override
        protected String doInBackground(Integer... params) {
            Process.setThreadPriority(THREAD_PRIORITY_BACKGROUND + THREAD_PRIORITY_MORE_FAVORABLE);

            int externalRounds = params[0];
            int internalRounds = params[1];

            int type = 0;
            int typesToRun = 0;

            if(params[2] == 3) {
                type = 0;
                typesToRun = 2; // enc & dec
            }
            else if(params[2] == 2){
                type = 1;
                typesToRun = 2; // nur dec
            }
            else if(params[2] == 1){
                type = 0;
                typesToRun = 1; // nur enc
            }

            ImageConverter conv = new ImageConverter();

            CsvLogger.AddLine("-", "-", "-", 0, "running for " + externalRounds + " ext rounds and " + internalRounds + " internal rounds");

            for(;type < typesToRun; type++) {
                // log starting type

                publishProgress("doing " + (type == 0 ? "encrypt" : "decrypt"));

                CsvLogger.AddLine("-", type == 0 ? "encrypt" : "decrypt", "-", 0, "start for images");

                for(int imagesCounter = 0; imagesCounter < imagesToRun.size(); imagesCounter++) {

                    // load Image = async
                    String filename = imagesToRun.get(imagesCounter);
                    ConvertedImage newImage = conv.ConvertFromArgbImage(filename);
                    String newFilename = "";
                    long sumOfBytes = 0;

                    if(filename.contains("encrypted") && type == 1) {
                        String originalFile = filename.replace(".encrypted.png", "");
                        newFilename = originalFile + ".decrypted.png";
                        ConvertedImage originalImage = conv.GetOrigImageInfo(originalFile);
                        sumOfBytes = originalImage.SumOfBytes;
                    }
                    else if(type == 0) {
                        newFilename = filename + ".encrypted.png";
                    }

                    publishProgress("working on image: " + filename);
                    CsvLogger.AddLine("-", type == 0 ? "encrypt" : "decrypt", filename, 0, "done loading image");

                    for (int i = 0; i < imageCiphersToRun.size(); i++) {

                        SystemClock.sleep((int)(Constants.SleepTimeBetweenRoundsInSeconds*1.5*1000));

                        ImageCipher curr = imageCiphersToRun.get(i);

                        // log starting cipher
                        publishProgress("working on cipher: " + curr.getName());
                        CsvLogger.AddLine(curr.getName(), type == 0 ? "encrypt" : "decrypt", filename, 0, "starting external rounds");
                        long[] measurements = new long[0];

                        for (int extRound = 0; extRound < externalRounds; extRound++) {
                            // log ext round
                            CsvLogger.AddLine(curr.getName(), type == 0 ? "encrypt" : "decrypt", filename, extRound, "starting round");

                            long startTime = System.nanoTime();

                            if (type == 0)
                                measurements = curr.encryptLong(newImage.ImageBytes, sumOfBytes, internalRounds);
                            else if (type == 1){
                                measurements = curr.decryptLong(newImage.ImageBytes, sumOfBytes, internalRounds);
                            }

                            long endTime = System.nanoTime();

                            for(int r = 0; r < measurements.length; r++){
                                CsvLogger.AddLine(curr.getName(), "times for inner round in ms", filename, r, measurements[r] + "");
                            }
                            //r = curr.decrypt(imageBytes, internalRounds);

                            CsvLogger.AddLine(curr.getName(), "times for ext round in ms", filename, extRound, (endTime-startTime)/1000000 + "");
                            publishProgress("done cipher " + curr.getName() + ", took " + (endTime-startTime)/1000000 + " ms");
                            CsvLogger.FlushLog();
                        }

                        CsvLogger.AddLine(curr.getName(), type == 0 ? "encrypt" : "decrypt", filename, 0, "done with cipher");
                        // flush logs
                    }

                    CsvLogger.AddLine("-", type == 0 ? "encrypt" : "decrypt", filename, 0, "done with image");
                }

                // log type done
                publishProgress("done with mode: " + (type == 0 ? "encrypt" : "decrypt"));
                CsvLogger.AddLine("-", type == 0 ? "encrypt" : "decrypt", "-", 0, "done with mode");
                CsvLogger.FlushLog();
                // flush logs
            }

            publishProgress("done with LongAsyncTask");
            CsvLogger.FlushLog();

            return "";
        }
    }

    public void onSleepTest(View v) {
        showText("Starting wait for 30 rounds with 30 seconds per round");
        String result = WaitTester.waitFor(30);
        prependText(result);
    }

    public void onTestAes(View v) {
        String result = CipherProviderTests.policyTests();
        CsvLogger.AddLine("test cipher aes 256bit", "encrypt", "Provider Test", 1, result);
        prependText(result);
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

            prependText(progress[0]);
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


                }
            }
            catch (Exception e){
                return e.toString();
            }

            CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "done " + tag + " overall - took: " + sumMillisecondsOverall + " ms");

            CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "start writing file " + newFilename);

            //conv.saveArgbImage(image, newFilename);

            //CsvLogger.AddLine(selectedCipher.getName(), tag, filename, rounds, "done writing file " + newFilename);

            CsvLogger.FlushLog();

            return newFilename + "\n Took: " + sumMillisecondsOverall + " ms";
        }

        @Override
        protected void onPostExecute(String result){
            showText("Encrypted successfully using " + selectedCipher.getName() + ": " + result);
        }
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
