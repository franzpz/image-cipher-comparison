package at.fhjoanneum.platzerf.imageciphercomparison;

import java.util.ArrayList;

/**
 * Created by platzerf on 20.10.2017.
 */

public class TestConfig {
    public int PauseBetweenFunctionsInSeconds = 1;
    public int PauseBetweenCiphersInSeconds = 1;
    public int PauseBetweenExtRounds = 1;

    public int NumberOfExtRoundsToRun = 1;
    public int NumberOfIntRoundsToRun = 1;

    public String Image = "";

    public ArrayList<ImageCipher> Ciphers = new ArrayList<ImageCipher>();
    public ArrayList<Function> Functions = new ArrayList<Function>();
}

