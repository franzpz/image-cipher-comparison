package at.fhjoanneum.platzerf.imageciphercomparison;

import java.util.ArrayList;

/**
 * Created by platzerf on 20.10.2017.
 */

public class Output implements Writer {
    public ArrayList<Writer> Writers = new ArrayList<Writer>();

    @Override
    public void Write(String text) {
        for (Writer writer : Writers) {
            writer.Write(text);
        }
    }

    @Override
    public void Flush() {
        for (Writer writer : Writers) {
            writer.Flush();
        }
    }
}
