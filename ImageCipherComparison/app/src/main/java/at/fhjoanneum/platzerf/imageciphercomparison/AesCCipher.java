package at.fhjoanneum.platzerf.imageciphercomparison;

/**
 * Created by platzerf on 17.06.2017.
 */

public class AesCCipher implements ImageCipher {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib-aes");
    }

    @Override
    public String getName() {
        return "AES C";
    }

    @Override
    public long[] encryptLong(int[] imageBytes, long sumOfBytes, int rounds) {
        return runAesCLong(imageBytes, rounds, Constants.SleepTimeBetweenRoundsInSeconds, 1);
    }

    @Override
    public long[] decryptLong(int[] imageBytes, long sumOfBytes, int rounds) {
        return runAesCLong(imageBytes, rounds, Constants.SleepTimeBetweenRoundsInSeconds, 2);
    }


    public native long[] runAesCLong(int[] imageBytes, int rounds, int sleepInSeconds, int mode); // 1 = enc, 2 = dec
}
