package at.fhjoanneum.platzerf.imageciphercomparison;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;

import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Created by platzerf on 08.06.2017.
 */

public class ImageConverter {

    // only for ARGB Config
    public ConvertedImage ConvertFromArgbImage(String path){
        ConvertedImage convImage = new ConvertedImage();
        Bitmap image = BitmapFactory.decodeFile(path);

        convImage.Config = image.getConfig();
        convImage.Height = image.getHeight();
        convImage.Width = image.getWidth();
        convImage.ImageBytes = new int[image.getHeight()*image.getWidth()*4];

        int pos = 0;
        for(int y = 0; y < image.getHeight(); y++) {
            for(int x = 0; x < image.getWidth(); x++) {
                int pixel = image.getPixel(x, y);
                convImage.ImageBytes[pos*3] = Color.alpha(pixel);
                convImage.ImageBytes[pos*3+1] = Color.red(pixel);
                convImage.ImageBytes[pos*3+2] = Color.green(pixel);
                convImage.ImageBytes[pos*3+3] = Color.blue(pixel);
                pos++;
            }
        }

        convImage.SumOfBytes = 0;
        for(int i = 0; i < convImage.ImageBytes.length; i++){
            convImage.SumOfBytes += convImage.ImageBytes[i];
        }

        return convImage;
    }

    public ConvertedImage GetOrigImageInfo(String path) {
        ConvertedImage orig = ConvertFromArgbImage(path);
        orig.ImageBytes = null;
        return orig;
    }

    public void saveArgbImage(ConvertedImage convertedImage, String newFilename) {

        Bitmap image = Bitmap.createBitmap(convertedImage.Width, convertedImage.Height, convertedImage.Config);

        int pos = 0;
        for(int y = 0; y < convertedImage.Height; y++) {
            for(int x = 0; x < convertedImage.Width; x++) {
                image.setPixel(
                        x,
                        y,
                        Color.argb(
                                convertedImage.ImageBytes[pos*3],
                                convertedImage.ImageBytes[pos*3+1],
                                convertedImage.ImageBytes[pos*3+2],
                                convertedImage.ImageBytes[pos*3+3])
                );
                pos++;
            }
        }

        FileOutputStream out = null;
        try {
            out = new FileOutputStream(newFilename);
            image.compress(Bitmap.CompressFormat.PNG, 100, out); // bmp is your Bitmap instance
            // PNG is a lossless format, the compression factor (100) is ignored
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (out != null) {
                    out.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}


