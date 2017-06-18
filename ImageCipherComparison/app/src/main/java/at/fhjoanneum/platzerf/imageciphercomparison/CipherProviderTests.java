package at.fhjoanneum.platzerf.imageciphercomparison;

import java.security.AlgorithmParameters;
import java.security.GeneralSecurityException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.Policy;
import java.security.Provider;
import java.security.spec.KeySpec;
import java.util.ArrayList;
import java.util.Enumeration;

import javax.crypto.Cipher;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.SecretKey;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.PBEKeySpec;
import javax.crypto.spec.PBEParameterSpec;
import javax.crypto.spec.SecretKeySpec;

/**
 * Created by platzerf on 18.06.2017.
 */

public class CipherProviderTests {

    public static String policyTests(){
        String algorithm = "AES/CBC/NoPadding";
        String provider = "BC";
        byte[]           data = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };

        SecretKeySpec    key128 = new SecretKeySpec(new byte[] {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}, "Blowfish");

        StringBuilder b = new StringBuilder();

        try
        {
            Cipher       c = Cipher.getInstance(algorithm, provider);

            c.init(Cipher.ENCRYPT_MODE, key128);

            c.doFinal(data);

            b.append("128 bit test: passed");
        }
        catch (SecurityException e)
        {
            if (e.getMessage() == "Unsupported keysize or algorithm parameters")
            {
                b.append("128 bit test failed: unrestricted policy files have not been installed for this runtime.");
            }
            else
            {
                b.append("128 bit test failed: there are bigger problems than just policy files: " + e);
            }
        }
        catch (GeneralSecurityException e)
        {
            b.append("128 bit test failed: there are bigger problems than just policy files: " + e);
        }

        SecretKey key192 = new SecretKeySpec(new byte[] {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17}, "Blowfish");

        try
        {
            Cipher       c = Cipher.getInstance(algorithm, provider);

            c.init(Cipher.ENCRYPT_MODE, key192);

            c.doFinal(data);

            b.append("192 bit test: passed");
        }
        catch (SecurityException e)
        {
            if (e.getMessage() == "Unsupported keysize or algorithm parameters")
            {
                b.append("192 bit test failed: unrestricted policy files have not been installed for this runtime.");
            }
            else
            {
                b.append("192 bit test failed: there are bigger problems than just policy files: " + e);
            }
        }
        catch (GeneralSecurityException e)
        {
            b.append("192 bit test failed: there are bigger problems than just policy files: " + e);
        }

        SecretKey key256 = new SecretKeySpec(new byte[] {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F/*,
                0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
                0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F*/}, "Blowfish");

        try
        {
            Cipher       c = Cipher.getInstance(algorithm, provider);

            c.init(Cipher.ENCRYPT_MODE, key256);

            c.doFinal(data);

            b.append("256 bit test: passed");
        }
        catch (SecurityException e)
        {
            if (e.getMessage() == "Unsupported keysize or algorithm parameters")
            {
                b.append("256 bit test failed: unrestricted policy files have not been installed for this runtime.");
            }
            else
            {
                b.append("256 bit test failed: there are bigger problems than just policy files: " + e);
            }
        }
        catch (GeneralSecurityException e)
        {
            b.append("256 bit test failed: there are bigger problems than just policy files: " + e);
        }

        b.append("Tests completed");

        return b.toString();
    }

    /*public static String readProviders(){
        try {
            // alternative "AES/CBC/NoPadding"
            String algorithm = "PBEWithSHA256And256BitAES-CBC-BC";
            Cipher cipher = Cipher.getInstance(algorithm);
            String password = "01234567890123456789012345678901"; // 256bit
            byte[] salt = { 0xF, 0x1, 0x2, 0x3, 0x3, 0x2, 0x0, 0x9, 0x8, 0x7, 0x6, 0x5, 0x4 };
            String iv ="myrandominitvect";
            byte[] ivAsBytes = iv.getBytes("UTF-8"); // 128bit

            PBEParameterSpec pbeParamSpec = new PBEParameterSpec(salt, 2048);
            PBEKeySpec pbeKeySpec = new PBEKeySpec(password.toCharArray(), salt, 2048, 256);
            SecretKeyFactory keyFac = SecretKeyFactory.getInstance(algorithm);
            SecretKey pbeKey = keyFac.generateSecret(pbeKeySpec);

            cipher.init(Cipher.ENCRYPT_MODE, pbeKey, pbeParamSpec);

            StringBuilder b = new StringBuilder();

            b.append("Algorithm: ");
            b.append(cipher.getAlgorithm());
            b.append("; Blocksize: ");
            b.append(cipher.getBlockSize());
            b.append("; IV: ");
            b.append(cipher.getIV());
            b.append(";Provider: ");

            Provider p = cipher.getProvider();

            ArrayList<String> keysProvider = new ArrayList<>();

            Enumeration<Object> providerEnumeration = p.elements();
            while(providerEnumeration.hasMoreElements()) {
                String element = (String) providerEnumeration.nextElement();
                if(element.contains(algorithm))
                    keysProvider.add(element);
            }

            b.append(cipher.getProvider().getInfo());

            AlgorithmParameters params = cipher.getParameters();

            return b.toString();

        } catch (Exception e) {
            return e.toString();
        }
    }*/
}
