#include <stdio.h>
static char input[2048];
int main(){
    puts("Lisp version 0.1");
    while(1){
    	fputs("lispy> ",stdout);
    	fgets(input,2048,stdin);
    	printf("Your input is %s",input);
    }
    return 0;
}
