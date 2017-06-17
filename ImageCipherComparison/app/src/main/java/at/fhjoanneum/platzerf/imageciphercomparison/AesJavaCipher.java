package at.fhjoanneum.platzerf.imageciphercomparison;

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
            Cipher cipher = Cipher.getInstance("AES/CBC/NoPadding");

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

    @Override
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

    @Override
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
}
