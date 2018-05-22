/**
 * @author Kamil Król
 * @version 1.0
 */

import java.security.SecureRandom;
import java.util.HashSet;
import java.util.Set;

public class DiffieHellman {

    public static final long n = 1000003;

    /**
     * Factorize given number to prime factors.
     *
     * @param n number to factorize.
     * @return Set of factors of given number.
     */
    public static Set<Long> factorizeNumber(long n) {
        Set<Long> factors = new HashSet<>();
        for (long i = 2; i < Math.sqrt(n); i++) {
            while (n % i == 0) {
                n = Math.floorDiv(n, i);
                factors.add(i);
                //System.out.println("i:"+i);
            }
        }
        factors.add(n);
        //System.out.println(factors.toString());
        return factors;
    }

    /**
     * Generates smallest generator of cyclic group of given order.
     *
     * @param n order of the group, should be prime number.
     * @return smallest generator of Cn group. -1 if failed to find generator.
     * (Program will fail to find generator if n is not prime number).
     */
    public static long findSmallestGenerator(long n) {
        long ord = 1;
        for (int i = 2; i < n; i++) {
            //System.out.println("i=" + i);
            while (ord <= n) {
                //System.out.println(fastPowerModulo(i,ord,n));
                if (fastPowerModulo(i, ord, n) == i && ord != 1) {
                    if (ord == n) {
                        return i;
                    } else {
                        break;
                    }
                }
                ord++;
            }
            ord = 1;
        }
        return -1;
    }

    /**
     * Finds smallest generator of multiplicative group (Z/nZ,*).
     * n has to be be prime.
     * @param n prime number, which is not checked
     * @return smallest generator of group (Z/nZ,*), -1 if failed to find it.
     */
    public static long findSmallestGeneratorButBetter(long n) {
        long setCardinality=n-1;
        Set<Long> factors = factorizeNumber(setCardinality);
        for(long i=2;i<setCardinality;i++) {
            if(fastPowerModulo(i,setCardinality,n)==1) {
                boolean isGenerator=true;
                for (Long factor : factors) {
                    if(fastPowerModulo(i,setCardinality/factor,n)==1){
                        isGenerator=false;
                        break;
                    }
                }
                if(isGenerator) return i;
            }
        }
        return -1;
    }

    /**
     * Returns argument to given power modulo given mod value
     *
     * @param argument argument.
     * @param exponent positive exponent.
     * @param mod      mod value.
     * @return argument to power exponent modulo mod value.
     */
    public static long fastPowerModulo(long argument, long exponent, long mod) {
        long result = -1;
        if (exponent == 0) return 1;
        else if (exponent % 2 == 0) {
            result = fastPowerModulo(argument, Math.floorDiv(exponent, 2), mod);
            return (result * result) % mod;
        } else {
            result = fastPowerModulo(argument, Math.floorDiv(exponent, 2), mod);
            return (result * result * argument) % mod;
        }
    }

    public static void main(String[] args) {
        //--check if 1000003 is prime--
        System.out.println("Factorization of 1000003: "+factorizeNumber(1000003));
        //--check smallest generator of cyclic group of order 1000003--
        long generator = findSmallestGeneratorButBetter(n); //m=1000003
        System.out.println("Generator found = " + generator);
        //--create Prime Numbers object to generate prime numbers and create Random object--
        PrimeNumbers primeNumbers = new PrimeNumbers(n);
        SecureRandom secureRandom = new SecureRandom();
        int amountOfPrimesInRange = primeNumbers.getAmountOfPrimesInRange();
        //--a and b are supposed to be private keys, below they are just random number from range [0, amountOFPrimeNumbers] but later it changes--
        int a = secureRandom.nextInt(amountOfPrimesInRange);
        int b = secureRandom.nextInt(amountOfPrimesInRange);

        int securityCounter = 0; //to handle condition when 0 is drawn lots
        while ((a == 0 || b == 0) && securityCounter < 10) {
            a = secureRandom.nextInt(amountOfPrimesInRange);
            b = secureRandom.nextInt(amountOfPrimesInRange);
            securityCounter++;
        }
        //now a and b are private keys
        a = primeNumbers.number(a);
        b = primeNumbers.number(b);
        System.out.println("a random number = " + a);
        System.out.println("b random number = " + b);
        long aPublic = fastPowerModulo(generator, a, n);
        long bPublic = fastPowerModulo(generator, b, n);
        System.out.println("aPublic = " + aPublic);
        System.out.println("bPublic = " + bPublic);
        System.out.println("aSecret = " + fastPowerModulo(bPublic, a, n));
        System.out.println("bSecret = " + fastPowerModulo(aPublic, b, n));
    }
}
