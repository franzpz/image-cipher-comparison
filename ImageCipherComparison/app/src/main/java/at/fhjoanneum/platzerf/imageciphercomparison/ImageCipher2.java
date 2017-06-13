package at.fhjoanneum.platzerf.imageciphercomparison;

/**
 * Created by platzerf on 10.06.2017.
 */

public class ImageCipher2 implements ImageCipher {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib-cipher2");
    }

    @Override
    public String getName() {
        return "Image Cipher 2";
    }

    @Override
    public int[] encrypt(int[] imageBytes, long sumOfBytes) {
        int h = gmpTest(5);

        return imageBytes;
    }

    @Override
    public int[] decrypt(int[] imageBytes, long sumOfBytes) {
        return decryptImageBytes(imageBytes);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native int[] encryptImageBytes(int[] imageBytes);
    public native int[] decryptImageBytes(int[] imageBytes);
    public native int gmpTest(int a);
}
