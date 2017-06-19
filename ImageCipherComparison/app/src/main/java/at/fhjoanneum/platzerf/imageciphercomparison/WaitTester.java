package at.fhjoanneum.platzerf.imageciphercomparison;

/**
 * Created by platzerf on 19.06.2017.
 */

public class WaitTester {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    public static String waitFor(int seconds) {
        long[] startAndEnd = waitForSecondsInC(seconds);
        return "Waited for " + seconds + ", from: " + startAndEnd[0] + " until: " + startAndEnd[1] + " nanoseconds";
    }

    public static native long[] waitForSecondsInC(int seconds);
}
