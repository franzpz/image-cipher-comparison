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
    public int[] encrypt(int[] imageBytes, long sumOfBytes) {
        return encryptImageBytesCipher2(imageBytes);
    }

    @Override
    public int[] decrypt(int[] imageBytes, long sumOfBytes) {
        return decryptImageBytesCipher2(imageBytes);
    }

    @Override
    public int[] encrypt(int[] imageBytes, long sumOfBytes, int rounds) {
        return encryptImageBytesCipher2Rounds(imageBytes, rounds);
    }

    @Override
    public int[] decrypt(int[] imageBytes, long sumOfBytes, int rounds) {
        return decryptImageBytesCipher2Rounds(imageBytes, rounds);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native int[] encryptImageBytesCipher2(int[] originalImageBytes);
    public native int[] decryptImageBytesCipher2(int[] originalImageBytes);
    public native int[] encryptImageBytesCipher2Rounds(int[] originalImageBytes, int rounds);
    public native int[] decryptImageBytesCipher2Rounds(int[] originalImageBytes, int rounds);
}
