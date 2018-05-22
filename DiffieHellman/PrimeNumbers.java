/**
 * @author Kamil Król
 * @version 1.2 (for Diffie-Hellman algorithm)
 */

public class PrimeNumbers {
    private int[] primes;
    private int AmountOfPrimesInRange;


    public PrimeNumbers(int n) {
        if (n <= 0) {
            throw new IllegalArgumentException("Wrong range.");
        } else {
            AmountOfPrimesInRange = 0;
            boolean[] allNumbers = new boolean[n + 1]; //0/1 array numbers [0,n]
            allNumbers[0] = false;
            allNumbers[1] = false;
            for (int j = 2; j <= n; j++) {
                allNumbers[j] = true;

            }
            for (int i = 2; i * i <= n; i++) {
                for (int j = i * i; j <= n && j % i == 0; j = j + i) {
                    allNumbers[j] = false;
                    //AmountOfPrimesInRange--;
                }
            }
            //here we already have boolean valued array which say which number is prime
            //now we count amount of primes
            for (int j = 2; j <= n; j++) {
                if (allNumbers[j]) {
                    AmountOfPrimesInRange++;
                }
            }
            primes = new int[AmountOfPrimesInRange];
            int x = 0;
            for (int k = 0; k <= n; k++) {
                if (allNumbers[k]) {
                    primes[x] = k;
                    x++;
                }
            }
        }
    }

    public int getAmountOfPrimesInRange() {
        return AmountOfPrimesInRange;
    }

    public PrimeNumbers(long n) {
        this((int)n);
    }

    public int number(int m) //returns mth prime number in array primes
    {
        return primes[m];
    }
}
