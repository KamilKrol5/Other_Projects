###
### author: KamilKról
###
from math import sqrt
import random
class RSAencryption:
    primes =[]
    def sieve_of_eratosthenes(self,sup):
        allnumbers =sup*[True]
        allnumbers[0]=False
        allnumbers[1]=False
        for i in range(2,int(sqrt(sup))+1):
            for j in range(i*i,sup):
                if j%i==0:
                    allnumbers[j]=False
        for k in range(0, len(allnumbers)):
            if allnumbers[k]==True:
                self.primes.append(k)

    def print_primes(self):
        print(self.primes)

    def encrypt(self,message):
        pass
    
    def decrypt(self,cypher):
        pass

    def generate_keys(self,supremum):
        RSAencryption.sieve_of_eratosthenes(self,supremum)
        if len(RSAencryption.primes)<2:
            print("Too short range")
            return None
        key1 = random.choice(RSAencryption.primes)
        key2 = random.choice(RSAencryption.primes)
        while key2==key1:
            key2 = random.choice(RSAencryption.primes)
        return [key1,key2]    
    
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
         
        

def main():
    #RSAencryption.sieve_of_eratosthenes(RSAencryption,82)
    #print(RSAencryption.generate_keys(RSAencryption,4))
    #RSAencryption.print_primes(RSAencryption)
    task=Task()
    #print(task.message_10)
    Task.decrypt(task,task.message_10)
    print(''.join(task.message_decrypted))


if __name__ == '__main__':
    main()