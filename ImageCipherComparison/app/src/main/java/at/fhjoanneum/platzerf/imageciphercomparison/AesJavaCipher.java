package at.fhjoanneum.platzerf.imageciphercomparison;

import android.os.SystemClock;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.security.NoSuchAlgorithmException;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

/**
 * Created by platzerf on 08.06.2017.
 */

public class AesJavaCipher implements ImageCipher {

    private Cipher cipherEnc;
    private Cipher cipherDec;
    private String encoding = "UTF-8";

    public AesJavaCipher(){
    }

    private Cipher init(int mode) {
        try {
            Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding", "BC");

            String key = "01234567890123456789012345678901";
            byte[] keyAsBytes = key.getBytes(encoding);

            String iv ="myrandominitvect";
            byte[] ivAsBytes = iv.getBytes(encoding);

            if(keyAsBytes.length != 32)
                throw new IllegalArgumentException("key is too short");

            if(ivAsBytes.length != 16)
                throw new IllegalArgumentException("iv is too short");

            SecretKeySpec skeySpec = new SecretKeySpec(keyAsBytes, "AES");
            IvParameterSpec ivSpec = new IvParameterSpec(ivAsBytes);

            cipher.init(mode, skeySpec, ivSpec);

            return cipher;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    @Override
    public String getName() {
        return "AES Java";
    }

    public int[] encrypt(int[] imageBytes, long sumOfBytes) {
        cipherEnc = init(Cipher.ENCRYPT_MODE);

        byte[] inputBytes = new byte[imageBytes.length];
        for(int i = 0; i < imageBytes.length; i++)
            inputBytes[i] = (byte)imageBytes[i];

        try {
            inputBytes = cipherEnc.doFinal(inputBytes);
        }
        catch (Exception e) {
            throw new IllegalStateException("AES encryption failed", e);
        }

        for(int i = 0; i < imageBytes.length; i++)
            imageBytes[i] = inputBytes[i] < 0 ? ((int)inputBytes[i]) + 256 : inputBytes[i];

        return imageBytes;
    }

    public int[] decrypt(int[] imageBytes, long sumOfBytes) {
        cipherDec = init(Cipher.DECRYPT_MODE);

        byte[] inputBytes = new byte[imageBytes.length];
        for(int i = 0; i < imageBytes.length; i++)
            inputBytes[i] = (byte)imageBytes[i];

        try {
            inputBytes = cipherDec.doFinal(inputBytes);
        }
        catch (Exception e) {
            throw new IllegalStateException("AES Decryption failed", e);
        }

        for(int i = 0; i < imageBytes.length; i++)
            imageBytes[i] = inputBytes[i] < 0 ? ((int)inputBytes[i]) + 256 : inputBytes[i];

        return imageBytes;
    }

    public byte[] encryptRaw(byte[] clear) throws Exception {
        cipherEnc = init(Cipher.ENCRYPT_MODE);
        byte[] encrypted = cipherEnc.doFinal(clear);
        return encrypted;
    }

    public byte[] decryptRaw(byte[] encrypted) throws Exception {
        cipherEnc = init(Cipher.DECRYPT_MODE);
        byte[] decrypted = cipherEnc.doFinal(encrypted);
        return decrypted;
    }

    public void WriteEncryptedToFileSystem(String path){

        try {
            byte[] bytes = readFileAsRawBytes(path);

            byte[] encrypted = encryptRaw(bytes);

            String targetPath = path.replace(".png", ".aesjavaencrypted.png");
            BufferedOutputStream out = new BufferedOutputStream(new FileOutputStream(targetPath));
            out.write(encrypted);
            out.close();
        }
        catch(Exception ex){
            throw new RuntimeException(ex);
        }
    }

    private byte[] readFileAsRawBytes(String path) throws IOException {
        File file = new File(path);
        FileInputStream fis = new FileInputStream(path);
        byte[] bytes = new byte[(int)file.length()];

        BufferedInputStream buf = new BufferedInputStream(new FileInputStream(file));
        buf.read(bytes, 0, bytes.length);
        buf.close();
        return bytes;
    }

    public void WriteDecryptedToFileSystem(String path){
        ImageConverter conv = new ImageConverter();
        String targetPath = path.replace("aesjavaencrypted.png", ".aesjavadecrypted.png");

        ConvertedImage orig = conv.ConvertFromArgbImage(path);

        orig.ImageBytes = decrypt(orig.ImageBytes, orig.SumOfBytes);

        conv.saveArgbImage(orig, targetPath);
    }

    int[] imageBytesResult;

    @Override
    public long[] encryptLong(int[] imageBytes, long sumOfBytes, int rounds) {

        long[] measurements = new long[rounds];
        long start;

        try {

            SystemClock.sleep(Constants.SleepTimeBetweenRoundsInSeconds * 1000);

            for (int r = 0; r < rounds; r++) {

                start = System.nanoTime();

                cipherDec = init(Cipher.ENCRYPT_MODE);

                byte[] inputBytes = new byte[imageBytes.length];
                for (int i = 0; i < imageBytes.length; i++)
                    inputBytes[i] = (byte) imageBytes[i];

                try {
                    inputBytes = cipherDec.doFinal(inputBytes);
                } catch (Exception e) {
                    throw new IllegalStateException("AES decryption failed", e);
                }

                for(int i = 0; i < imageBytes.length; i++)
                    imageBytes[i] = inputBytes[i] < 0 ? ((int)inputBytes[i]) + 256 : inputBytes[i];

                imageBytesResult = imageBytes;

                measurements[r] = (System.nanoTime() - start)/1000000;
            }

            SystemClock.sleep(Constants.SleepTimeBetweenRoundsInSeconds * 1000);
        }
        catch(Exception ex){
            throw new RuntimeException(ex);
        }

        return measurements;
    }

    public long[] decryptLongFromImage(int[] imageBytes, String file, int rounds){


        long[] measurements = new long[rounds];
        long start;

        try {
            // do before pause so not counted in measurements
            byte[] rawInput = readFileAsRawBytes(file);

            Thread.sleep(Constants.SleepTimeBetweenRoundsInSeconds * 1000);

            for (int r = 0; r < rounds; r++) {

                start = System.currentTimeMillis();

                cipherDec = init(Cipher.DECRYPT_MODE);

                // unnecessary but to for compatibility with other cipher measurements
                byte[] inputBytes = new byte[imageBytes.length];
                for (int i = 0; i < imageBytes.length; i++)
                    inputBytes[i] = (byte) imageBytes[i];

                try {
                    inputBytes = cipherDec.doFinal(rawInput);
                } catch (Exception e) {
                    throw new IllegalStateException("AES encryption failed", e);
                }

                // unnecessary but to for compatibility with other cipher measurements
                for(int i = 0; i < imageBytes.length; i++)
                    imageBytes[i] = imageBytes[i] < 0 ? ((int)imageBytes[i]) + 256 : imageBytes[i];

                measurements[r] = System.currentTimeMillis() - start;
            }

            Thread.sleep(Constants.SleepTimeBetweenRoundsInSeconds * 1000);
        }
        catch(Exception ex){
            throw new RuntimeException(ex);
        }

        return measurements;

    }

    @Override
    public long[] decryptLong(int[] imageBytes, long sumOfBytes, int rounds) {

        long[] measurements = new long[rounds];
        long start;

        try {

            Thread.sleep(Constants.SleepTimeBetweenRoundsInSeconds * 1000);

            for (int r = 0; r < rounds; r++) {

                start = System.currentTimeMillis();

                cipherDec = init(Cipher.DECRYPT_MODE);

                byte[] inputBytes = new byte[imageBytes.length];
                for (int i = 0; i < imageBytes.length; i++)
                    inputBytes[i] = (byte) imageBytes[i];

                try {
                    inputBytes = cipherDec.doFinal(inputBytes);
                } catch (Exception e) {
                    throw new IllegalStateException("AES encryption failed", e);
                }

                for(int i = 0; i < imageBytes.length; i++)
                    imageBytes[i] = inputBytes[i] < 0 ? ((int)inputBytes[i]) + 256 : inputBytes[i];

                measurements[r] = System.currentTimeMillis() - start;
            }

            Thread.sleep(Constants.SleepTimeBetweenRoundsInSeconds * 1000);
        }
        catch(Exception ex){
            throw new RuntimeException(ex);
        }

        return measurements;

    }
}
