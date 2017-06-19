package at.fhjoanneum.platzerf.imageciphercomparison;

import android.content.Context;
import android.util.Log;
import android.widget.Toast;

import java.io.File;
import java.io.FileWriter;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

/**
 * Created by platzerf on 17.06.2017.
 */

public class CsvLogger {

    private static String path;
    private static Context context;
    private static String currentFile = null;
    private static ArrayList<String> logCache = new ArrayList<>();

    public static void InitLogger(Context context, String base) {
        CsvLogger.context = context;
        base += "measurements";

        try {
            File gpxfile = new File(base, "csv-log-" + getCurrentTimeStamp() + ".csv");
            currentFile = gpxfile.getAbsolutePath();

            FileWriter writer = new FileWriter(gpxfile);
            writer.append("timestamp;cipher;mode;file;rounds;message;\n");
            writer.flush();
            writer.close();
        } catch (Exception e) {
            Log.e("Ciphers", "logging failed", e);
        }
    }

    public static void AddLine(String cipher, String mode, String file, int rounds, String message){
        logCache.add(getCurrentTimeStamp() + ";" + cipher + ";" + mode  + ";" + file + ";" + rounds + ";" + message + ";\n");
    }

    public static void FlushLog(){
        if(currentFile == null)
            return;

        try {
            File gpxfile = new File(currentFile);

            FileWriter writer = new FileWriter(gpxfile);
            for(int i = 0; i < logCache.size(); i++)
                writer.append(logCache.get(i));
            writer.flush();
            writer.close();
            logCache.clear();
        } catch (Exception e) {
            Log.e("Ciphers", "logging failed", e);
        }
    }

    public static String getCurrentTimeStamp(){
        try {

            SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd-HH.mm.ss.SSS");
            String currentDateTime = dateFormat.format(new Date()); // Find todays date

            return currentDateTime;
        } catch (Exception e) {
            e.printStackTrace();

            return null;
        }
    }
}
