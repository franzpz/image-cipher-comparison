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
    public int[] encrypt(int[] imageBytes, long sumOfBytes) {
        return encryptImageBytesAesC(imageBytes);
    }

    @Override
    public int[] decrypt(int[] imageBytes, long sumOfBytes) {
        return decryptImageBytesAesC(imageBytes);
    }

    @Override
    public int[] encrypt(int[] imageBytes, long sumOfBytes, int rounds) {

        return encryptImageBytesAesCRounds(imageBytes, rounds);
    }

    @Override
    public int[] decrypt(int[] imageBytes, long sumOfBytes, int rounds) {
        return decryptImageBytesAesCRounds(imageBytes, rounds);
    }

    public native int[] encryptImageBytesAesC(int[] originalImageBytes);
    public native int[] decryptImageBytesAesC(int[] originalImageBytes);
    public native int[] encryptImageBytesAesCRounds(int[] originalImageBytes, int rounds);
    public native int[] decryptImageBytesAesCRounds(int[] originalImageBytes, int rounds);
}
