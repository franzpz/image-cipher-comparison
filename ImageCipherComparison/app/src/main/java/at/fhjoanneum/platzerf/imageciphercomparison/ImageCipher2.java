package at.fhjoanneum.platzerf.imageciphercomparison;

/**
 * Created by platzerf on 10.06.2017.
 */

public class ImageCipher2 implements ImageCipher {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    public String getName() {
        return "Image Cipher 2";
    }


    @Override
    public long[] encryptLong(int[] imageBytes, long sumOfBytes, int rounds) {

        return runImageBytesCipher2Long(imageBytes, rounds, Constants.SleepTimeBetweenRoundsInSeconds, 1);
    }

    @Override
    public long[] decryptLong(int[] imageBytes, long sumOfBytes, int rounds) {
        return runImageBytesCipher2Long(imageBytes, rounds, Constants.SleepTimeBetweenRoundsInSeconds, 2);
    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native long[] runImageBytesCipher2Long(int[] originalImageBytes, int rounds, int sleepInSeconds, int mode); //1 = enc, 2 = dec
}
