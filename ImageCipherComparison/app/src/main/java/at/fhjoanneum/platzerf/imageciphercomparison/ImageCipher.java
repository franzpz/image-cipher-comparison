package at.fhjoanneum.platzerf.imageciphercomparison;

/**
 * Created by platzerf on 08.06.2017.
 */

public interface ImageCipher {
    String getName();
    long[] encryptLong(int[] imageBytes, long sumOfBytes, int rounds);
    long[] decryptLong(int[] imageBytes, long sumOfBytes, int rounds);
}
