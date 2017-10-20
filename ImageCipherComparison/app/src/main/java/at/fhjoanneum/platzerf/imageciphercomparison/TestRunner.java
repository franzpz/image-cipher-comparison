package at.fhjoanneum.platzerf.imageciphercomparison;

/**
 * Created by platzerf on 20.10.2017.
 */

import static android.os.Process.THREAD_PRIORITY_BACKGROUND;
import static android.os.Process.THREAD_PRIORITY_FOREGROUND;
import static android.os.Process.THREAD_PRIORITY_MORE_FAVORABLE;
import android.os.Process;
import android.os.AsyncTask;
import android.os.SystemClock;

public class TestRunner extends AsyncTask<Integer, String, String> {

    public Output output = new Output();
    private TestConfig config;


    public TestRunner(TestConfig config){
        this.config = config;
    }

    @Override
    protected String doInBackground(Integer... params) {
        Process.setThreadPriority(THREAD_PRIORITY_FOREGROUND);

        ImageConverter conv = new ImageConverter();

        publishProgress("running for " + config.NumberOfExtRoundsToRun + " ext rounds and " + config.NumberOfIntRoundsToRun + " internal rounds");

        long totalStart = System.nanoTime();

        for(Function f : config.Functions) {

            Wait(config.PauseBetweenFunctionsInSeconds);

            publishProgress("doing " + f.toString());

            // load Image = async
            String filename = config.Image;
            ConvertedImage newImage = conv.ConvertFromArgbImage(filename);
            String newFilename = "";
            long sumOfBytes = 0;

            if(filename.contains("encrypted") && f == Function.Decrypt) {
                String originalFile = filename.replace(".encrypted.png", "");
                newFilename = originalFile + ".decrypted.png";
                ConvertedImage originalImage = conv.GetOrigImageInfo(originalFile);
                sumOfBytes = originalImage.SumOfBytes;
            }
            else if(f == Function.Encrypt) {
                newFilename = filename + ".encrypted.png";
            }

            publishProgress("done loading image: " + filename);

            for (ImageCipher curr : config.Ciphers) {

                Wait(config.PauseBetweenCiphersInSeconds);

                publishProgress("working on cipher: " + curr.getName());

                long[] measurements = new long[0];

                for (int extRound = 0; extRound < config.NumberOfExtRoundsToRun; extRound++) {

                    publishProgress(curr.getName() + " - " + f + " - filename: " + filename + " - starting round: " + extRound);

                    long startTime = System.nanoTime();

                    Wait(config.PauseBetweenExtRounds);

                    if (f == Function.Encrypt)
                        measurements = curr.encryptLong(newImage.ImageBytes, sumOfBytes, config.NumberOfIntRoundsToRun);
                    else if (f == Function.Decrypt){
                        measurements = curr.decryptLong(newImage.ImageBytes, sumOfBytes, config.NumberOfIntRoundsToRun);
                    }

                    long endTime = System.nanoTime();

                    for(int r = 0; r < measurements.length; r++){
                        publishProgress(curr.getName() + " - inner round " + r + " took " + measurements[r] + "");
                    }

                    float timeTaken = (endTime-startTime)/1000000;
                    publishProgress(curr.getName() + " - ext round " + extRound + " took " + timeTaken + "");
                }

                publishProgress(curr.getName() + " - done with cipher");
            }

            publishProgress("done with " + f);
        }

        publishProgress("done with LongAsyncTask , took " + (System.nanoTime()-totalStart)/1000000 + " ms");

        return "";
    }

    public void Wait(int seconds){
        output.Write("waiting for " + seconds);
        output.Flush();
        SystemClock.sleep((int)(config.PauseBetweenCiphersInSeconds*1000));
        output.Write("done waiting");
        output.Flush();
    }

    @Override
    protected void onProgressUpdate(String... progress) {
        output.Write(progress[0]);
    }
}