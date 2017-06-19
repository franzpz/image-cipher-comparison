package at.fhjoanneum.platzerf.imageciphercomparison;

/**
 * Created by platzerf on 08.06.2017.
 */

public interface ImageCipher {
    String getName();
    int[] encrypt(int[] imageBytes, long sumOfBytes);
    int[] decrypt(int[] imageBytes, long sumOfBytes);
    int[] encrypt(int[] imageBytes, long sumOfBytes, int rounds);
    int[] decrypt(int[] imageBytes, long sumOfBytes, int rounds);
}
