package at.fhjoanneum.platzerf.imageciphercomparison;

/**
 * Created by platzerf on 17.06.2017.
 */

public class ImageCipher2Java implements ImageCipher {

    static int KEY_SIZE = 32;
    static int BUFFER_SIZE = 32;
    static double LOGISTIC_R = 3.712345;

    static int[] key = new int[] {
            49, 50, 51, 52, 53, 55, 56, 57, 48, 49, 50, 51, 52, 53, 54, 55,
            56, 57, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 49, 50, 0
    };

    static int[] iv = new int[] {
            34, 45, 56, 78, 90, 12, 34, 23, 56, 78, 9, 3, 5, 23, 87,
            3, 4, 5, 1, 9, 8, 34, 89, 34, 22, 93, 75, 76, 23, 16, 39, 53
    };

    private AlgoParams params;

    public ImageCipher2Java(){
    }

    @Override
    public String getName() {
        return "Image Cipher 2 Java";
    }

    public int[] encrypt(int[] imageBytes, long sumOfBytes) {
        params = createParams(key);
        int i = 0;
        while(i < imageBytes.length) {
            encrypt(params, imageBytes, i, key, iv);
            i += 32;
        }
        return imageBytes;
    }

    public int[] decrypt(int[] imageBytes, long sumOfBytes) {

        params = createParams(key);
        int i = 0;
        while(i < imageBytes.length) {
            decrypt(params, imageBytes, i, key, iv);
            i += 32;
        }
        return imageBytes;
    }

    @Override
    public long[] encryptLong(int[] imageBytes, long sumOfBytes, int rounds) {
        return new long[0];
    }

    @Override
    public long[] decryptLong(int[] imageBytes, long sumOfBytes, int rounds) {
        return new long[0];
    }


    public class AlgoParams {
        public double X = 0.0;
        public int C = 0;
    }

    public AlgoParams createParams(int[] key) {
        AlgoParams params = new AlgoParams();

        double r = 0.0;
        for(int i = 0; i < KEY_SIZE; i++) {
            r += convertM1(key[i]);
            params.C = (params.C + key[i]) % 256;
        }

        params.X = r - Math.floor(r);
        return params;
    }

    public static double convertM1(double x){
        return x / 1000.0;
    }

    public static int convertM2(double x){
        return (int)x % 256;
    }

    public static void encrypt(AlgoParams params, int[] origBytes, int origStart, int[] key, int[] iv) {
        double x = params.X;
        int lastC = params.C;

        double xn;
        double logisticSum;
        int numberOfLogisticMapRepititions;
        int nextKeyPos;

        int bytePos = origStart;
        int end = origBytes.length - bytePos >= 32 ? 32 : origBytes.length - bytePos;
        for(int l = 0; l < end; l++) {
            // start at key pos 0 again after reaching end of key
            nextKeyPos = (l+1) % KEY_SIZE;

            x = convertM1((double)x +(double)lastC+(double)key[l]);
            //x = Mround(x * PRECISION) / PRECISION;

            numberOfLogisticMapRepititions = key[nextKeyPos] + lastC;

            xn = x;
            logisticSum = 0.0;
            for(int i = 0; i < numberOfLogisticMapRepititions; i++) {
                xn = LOGISTIC_R * xn * (1.0 - xn);
                logisticSum += xn;
            }

            origBytes[bytePos] = origBytes[bytePos]^iv[l];

            origBytes[bytePos] = (((int)origBytes[bytePos]) + convertM2(logisticSum)) % 256;
            lastC = origBytes[bytePos];

            iv[l] = lastC;
        }

        params.X = x;
        params.C = lastC;
    }

    public static void decrypt(AlgoParams params, int[] origBytes, int origStart, int[] key, int[] iv) {
        double x = params.X;
        int lastC = params.C;

        double xn;
        double logisticSum;
        int numberOfLogisticMapRepititions;
        int nextKeyPos;

        int bytePos = origStart;
        int end = origBytes.length - bytePos >= 32 ? 32 : origBytes.length - bytePos;
        for(int l = 0; l < end; l++) {
            // start at key pos 0 again after reaching end of key
            nextKeyPos = (l+1) % KEY_SIZE;

            x = convertM1((double)x +(double)lastC+(double)key[l]);
            //x = round(x * PRECISION) / PRECISION;

            numberOfLogisticMapRepititions = key[nextKeyPos] + lastC;

            xn = x;
            logisticSum = 0.0;
            for(int i = 0; i < numberOfLogisticMapRepititions; i++) {
                xn = LOGISTIC_R * xn * (1.0 - xn);
                logisticSum += xn;
            }

            lastC = origBytes[bytePos];
            origBytes[bytePos] = ((((int)origBytes[bytePos]) - convertM2(logisticSum)) % 256 + 256) % 256;

            origBytes[bytePos] = origBytes[bytePos]^iv[l];
            iv[l] = lastC;
        }

        params.X = x;
        params.C = lastC;
    }
}
