package at.fhjoanneum.platzerf.imageciphercomparison;

/**
 * Created by platzerf on 08.06.2017.
 */

public class ImageCipher1Gmp implements ImageCipher {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib-gmp");
    }

    @Override
    public String getName() {
        return "Image Cipher 1 GMP";
    }

    @Override
    public int[] encrypt(int[] imageBytes, long sumOfBytes) {
        return encryptImageBytesCipher1(imageBytes, sumOfBytes);
    }

    @Override
    public int[] decrypt(int[] imageBytes, long sumOfBytes) {
        return decryptImageBytesCipher1(imageBytes, sumOfBytes);
    }

    @Override
    public int[] encrypt(int[] imageBytes, long sumOfBytes, int rounds) {
        return encryptImageBytesCipher1Rounds(imageBytes, sumOfBytes, rounds);
    }

    @Override
    public int[] decrypt(int[] imageBytes, long sumOfBytes, int rounds) {
        return decryptImageBytesCipher1Rounds(imageBytes, sumOfBytes, rounds);
    }
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native int[] encryptImageBytesCipher1(int[] originalImageBytes, long sumOfImageBytes);
    public native int[] decryptImageBytesCipher1(int[] originalImageBytes, long sumOfImageBytes);
    public native int[] encryptImageBytesCipher1Rounds(int[] originalImageBytes, long sumOfImageBytes, int rounds);
    public native int[] decryptImageBytesCipher1Rounds(int[] originalImageBytes, long sumOfImageBytes, int rounds);
}
