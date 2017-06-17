package at.fhjoanneum.platzerf.imageciphercomparison;

/**
 * Created by platzerf on 08.06.2017.
 */

public class ImageCipher1 implements ImageCipher {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    public String getName() {
        return "Image Cipher 1";
    }

    @Override
    public int[] encrypt(int[] imageBytes, long sumOfBytes) {
        return encryptImageBytesCipher1(imageBytes, sumOfBytes);
    }

    @Override
    public int[] decrypt(int[] imageBytes, long sumOfBytes) {
        return decryptImageBytesCipher1(imageBytes, sumOfBytes);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native int[] encryptImageBytesCipher1(int[] originalImageBytes, long sumOfImageBytes);
    public native int[] decryptImageBytesCipher1(int[] originalImageBytes, long sumOfImageBytes);
}
