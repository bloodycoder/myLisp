#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"
static char input[2048];
long eval(mpc_ast_t *a){
	if(strstr(a->tag,"number")){
		return atoi(a->contents);
	}
	int i = 0,operator_posi=-1;
	while(i<a->children_num){
		if(!strstr(a->children[i]->tag,"operator")){
			i++;
		}else{
			operator_posi=i;
			break;
		}
	}
	if(operator_posi!=-1){
		char *op = a->children[i]->contents;
		long value1 = eval(a->children[i+1]);
		long value2 = eval(a->children[i+2]);
		if(strcmp(op,"+")==0){return value1+value2;}
		if(strcmp(op,"-")==0){return value1-value2;}
		if(strcmp(op,"/")==0){return value1/value2;}
		if(strcmp(op,"*")==0){return value1*value2;}
	}
}
int main(){
    puts("Lisp version 0.1");
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");
    mpca_lang(MPCA_LANG_DEFAULT,
    	"\
    	number:/-?[0-9]+/;\
    	operator:'+'|'-'|'*'|'/';\
    	expr:<number> | '('<operator><expr>+')';\
    	lispy: <operator><expr>+|<expr>;\
    	",
    	Number,Operator,Expr,Lispy);
    while(1){
    	fputs("lispy> ",stdout);
    	fgets(input,2048,stdin);
    	mpc_result_t r;
    	if(mpc_parse("<stdin>",input,Lispy,&r)){
    		mpc_ast_t* a = r.output;
    		printf("%ld\n",eval(a));
    		//mpc_ast_t* a0 = a->children[0];
    		printf("%s\n",a->tag);
    		//mpc_ast_print(r.output);
    		mpc_ast_delete(r.output);
    	}
    	else{
    		mpc_err_print(r.error);
    		mpc_err_delete(r.error);
    	}
    }
    mpc_cleanup(4,Number,Operator,Expr,Lispy);
    return 0;
}
