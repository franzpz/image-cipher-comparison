package at.fhjoanneum.platzerf.imageciphercomparison;

/**
 * Created by platzerf on 10.06.2017.
 */

public class ImageCipher2Gmp implements ImageCipher {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib-gmp");
    }

    @Override
    public String getName() {
        return "Image Cipher 2 GMP";
    }

    @Override
    public int[] encrypt(int[] imageBytes, long sumOfBytes) {
        return encryptImageBytesCipher2(imageBytes);
    }

    @Override
    public int[] decrypt(int[] imageBytes, long sumOfBytes) {
        return decryptImageBytesCipher2(imageBytes);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native int[] encryptImageBytesCipher2(int[] originalImageBytes);
    public native int[] decryptImageBytesCipher2(int[] originalImageBytes);
}