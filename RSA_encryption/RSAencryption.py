###
### author: Kamil Król
###
from math import sqrt,ceil
import random

class Task:
    message_decrypted=[]
    message_10=[]
    message_16=["3f824fd","033c7a71","050a6706","050a6706","03ffab5e","03f824fd","0189a78d"," 005bca7d","00734305","04046ca6","017698b6","005bca7d","03f824fd","03d10ac0"," 003622e4","011c1c7e","030cf03c","011c1c7e","03d10ac0","01b60e5d","03f824fd"," 00734305","007a18e6","03ffab5e","00734305","0179f797","037906bb","050a6706"," 007a18e6","015d897d","03f824fd","037906bb","03f824fd","0451f198","059ff1e0"," 03d10ac0","02e6b154","037906bb","03f824fd","00734305","003622e4","011c1c7e","0414fa45","03f824fd","00a6891a","042edbee"]
    #n=101080891
    private_key=(2062465,101080891)
    def __init__(self):
        for s in self.message_16:
            self.message_10.append(int(s,16))

    def fast_power_modulo(self, argument, exponent, modulus):
        if exponent==0:
            return 1
        answer = Task.fast_power_modulo(self,argument,exponent//2,modulus)
        if exponent%2==0:
            return (answer*answer)%modulus
        else:
            return (answer*answer*argument)%modulus 
        
    def decrypt(self,message):
        for s in message:
            self.message_decrypted.append(chr(Task.fast_power_modulo(Task,s,Task.private_key[0],Task.private_key[1])))
         
    def factorize_number(self, number):
        factors= set()
        for i in range(2,ceil(sqrt(number))):
            while number%i==0:
                factors.add(i)
                number=number//i
        factors.add(number)
        return factors

    def find_modular_multiplicative_inverse(self, number, modulus):
        for i in range(2,modulus):
            if (number * i) % modulus == 1:
                return i

def main():
    #RSAencryption.sieve_of_eratosthenes(RSAencryption,82)
    #print(RSAencryption.generate_keys(RSAencryption,4))
    #RSAencryption.print_primes(RSAencryption)
    task=Task()
    #print(task.message_10)
    n=10108091
    k=2062465
    Task.decrypt(task,task.message_10)
    print("Message:")
    print(''.join(task.message_decrypted))
    print("Factorization of 10108091:")
    print(task.factorize_number(101080891))
    phi_number=(10009-1)*(10099-1)
    print("Multiplicative inverse of 2062465 modulo "+str(phi_number)+":")
    print(task.find_modular_multiplicative_inverse(k,phi_number))
    
if __name__ == '__main__':
    main()