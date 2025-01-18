#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
void reverse_string(char *, int);
void word_print(char *, int);
void replace_words(char *, int, char *, char *, int);


/*
    The following code buffers a given string in order to
    ensure that the output is at a fixed length. The code
    ensures that there is no trailing white space at the
    beginning and end of the string. And ensures that
    the remaining space in the buffer is filled with '.'
    Additional code is made to ensure that strings inputs 
    are not over max length.
*/
int setup_buff(char *buff, char *user_str, int buffer_len) {
    int i = 0;  
    int j = 0;  
    int in_whitespace = 0;  
    
    while (user_str[i] != '\0' && (user_str[i] == ' ' || user_str[i] == '\t')) {
        i++;
    }

    //
    while (user_str[i] != '\0' && j < buffer_len) {
        char current = user_str[i];  
        if (current == ' ' || current == '\t') {
            if (!in_whitespace) {
                *(buff + j) = ' ';  
                j++;  
            }
            in_whitespace = 1;  
        } else {
            *(buff + j) = current;  
            j++; 
            in_whitespace = 0;  
        }
        i++;  
    }

    if (user_str[i] != '\0') {
        fprintf(stderr, "Error: Input string is too long.");
        return -1;  
    }
    
    if (j > 0 && *(buff + j - 1) == ' ') {
        j--; 
    }

    while (j < buffer_len) {
        *(buff + j) = '.';  
        j++;
    }

    return j;  
}

/*
    The following code counts the words in a given string 
    and returns the word count as an integer.  A word is defined 
    as a sequence of non-space (' '), non-tab ('\t'),
    and non-dot ('.') characters. The function iterates through 
    the string character by character and uses 'in_word'
    to track whether it is currently inside a word. The word count 
    or 'wc' is incremented each time the function encounters the 
    start of a new word.
*/
int count_words(char *buff, int buffer_len, int str_len){
    int wc = 0;
    int in_word = 0;

    for (int i = 0; i < str_len; i++) {
        char current = *(buff + i);
        if (current == '.') {
            continue;
        }
        if (current == ' ' || current == '\t'){
            in_word = 0;
        } else {
            if (!in_word){
                in_word = 1;
                wc++;
            }
        }
    }
    return wc;
}

/*
    Below is a function that prints each word in 
    the input buffer along with the number
    of letters in each word. A word is defined 
    as a sequence of non-space (' '), non-tab ('\t'),
    and non-dot ('.') characters. After processing 
    all words, the function prints the total number of words.
*/
void word_print(char *buff, int buffer_len){
    int wc = 1;
    int in_word = 0;
    int wlen = 0;

    for (int i = 0; i < buffer_len; i++) {
        if (*(buff + i) == ' ' || *(buff + i) == '\t' || *(buff + i) == '.') {
            if (in_word) {
                printf("%d. %.*s(%d)\n", wc, wlen, buff + i - wlen, wlen);
                wc++;
                in_word = 0;
            }
            wlen = 0;
        } else {
            if (!in_word) {
                in_word = 1;
                wlen = 1;
            } else {
                wlen++;
            }
        }
    }
    printf("\nNumber of words returned: %d\n", wc - 1);
}
/*
    Below is code that reverses a string in place. 
    This is done by swapping the characters at 
    the "start" and "end" positions within the 
    buffer's memory address repeatedly inward 
    until the string is completely reversed.
*/
void reverse_string(char *buff, int buffer_len){
    int start = 0;
    int end = buffer_len - 1;
    
    while (*(buff + end) == '.' && end > start) {
        end--;
    }

    while (start < end) {
        char temp = *(buff + start); 
        *(buff + start) = *(buff + end); 
        *(buff + end) = temp; 
        start++;
        end--;
    }
}

void print_buff(char *buff, int buffer_len){
    printf("Buffer:  [");
    for (int i=0; i<buffer_len; i++){
        putchar(*(buff+i));
    }
    putchar(']');
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //          
    /*
            This is safe because the if statement ensures that the input
            contains at least one argument. argc < 2 means that the user
            would not have provided any of the stringfun -[h|c|r|w|x] commands.
            *argv[1] != '-' ensures that the second argument is a valid option
            that starts with a -. With this if statement, the program can run
            without unknown behaviors.
    */
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    /*
            The purpose of the if statement below is the to
            check if the user provided the required input string
            to properly execute the program. If true, the program
            will not execute an action, as it was not provided
            with a proper input.
    */
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff =  (char *)malloc(BUFFER_SZ * sizeof(char));
    if (buff == NULL){
        fprintf(stderr, "Error: Failed to correctly allocate memory");
        exit(99);
    } 

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos

    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        free(buff);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        case 'r':
            reverse_string(buff, user_str_len);
            break;

        case 'w':
            printf("Word Print\n----------\n");
            word_print(buff, BUFFER_SZ);
            break;
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
/*
            I think that providing both the pointer and the length is 
            good practice because it ensures that we don't input a 
            string that is greater than the buffer which could cause 
            issues in the program, we can easily change the size of 
            the buffer, and it is clearer to know how much memory
            the buffer is using for the coder/readers.
*/ 