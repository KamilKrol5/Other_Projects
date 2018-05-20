import java.util.Scanner;

class RSAencryption //abandoned 
{
    public static void main(String[] args) {
        Scanner scan = new Scanner(System.in);
        String messege;
        
        System.out.print("Type a messege to encrypt: ");
        messege=scan.nextLine();
        System.out.println(ConvertMessToNumber(messege));
    }
    static private int ConvertMessToNumber(String messege) {
        String messege_in_numbers="";
        int messege_as_number;
        int character;
        messege=messege.toLowerCase();
        System.out.println("New messege."+messege);
        if(messege.length()==0){
            System.out.println("No messege.");
            return 0;
        }
        for(int i=0;i<messege.length();i++){
            character=(int)messege.charAt(i);
            System.out.println("Character: "+character);
            messege_in_numbers=messege_in_numbers.concat(Integer.toString(character));
            //catch(NumberFormatException ex) {System.out.println("Inner error during converting int to String");}
            System.out.println(messege_in_numbers);
        }
        messege_as_number=Integer.parseInt(messege_in_numbers);
        return messege_as_number;
    }
    
}