#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"
static char input[2048];
typedef struct {
  int type;
  long num;
  int err;
} lval;
/* 创建枚举*/
enum{LVAL_NUM,LVAL_ERR};
/* div_zero 代表 4 lerr_bad_op 是2 lerr_bad_num 是1 */
enum{LERR_DIV_ZERO,LERR_BAD_OP,LERR_BAD_NUM};
lval lval_num(long x){
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}
lval lval_err(int x){
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}
void print_lval(lval v){
    switch(v.type){
        case LVAL_NUM:printf("%ld",v.num);break;
        case LVAL_ERR:
        if(v.err==LERR_DIV_ZERO){
            printf("Error:You cant divide with 0");
        }
        if(v.err==LERR_BAD_OP){
            printf("Error:Your oprator error!");
        }
        if(v.err==LERR_BAD_NUM){
            printf("Error:Number too big");
        }
    }
}
lval eval_op(lval x,char *op,lval y){
    if(x.type==LVAL_ERR){
        return x;
    }
    if(y.type==LVAL_ERR){
        return y;
    }
    if(strcmp(op,"+")==0){return lval_num(x.num+y.num);}
    if(strcmp(op,"-")==0){return lval_num(x.num-y.num);}
    if(strcmp(op,"*")==0){return lval_num(x.num*y.num);}
    if(strcmp(op,"/")==0){return y.num==0?lval_err(LERR_DIV_ZERO):lval_num(x.num/y.num);}
    return lval_err(LERR_BAD_OP);
}
lval eval(mpc_ast_t *a){
	if(strstr(a->tag,"number")){
        errno = 0;
        long x = strtol(a->contents, NULL, 10);
		return errno!= ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
	}
	int i = 0,operator_posi=-1;
	while(i<a->children_num){
		if(!strstr(a->children[i]->tag,"symbol")){
			i++;
		}else{
			operator_posi=i;
			break;
		}
	}
	if(operator_posi!=-1){
		char *op = a->children[i]->contents;
		lval value1 = eval(a->children[i+1]);
		lval value2 = eval(a->children[i+2]);
        return eval_op(value1,op,value2);
	}
    else{
        return lval_err(LERR_BAD_OP);
    }
}
int main(){
    puts("Lisp version 0.1");
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Symbol = mpc_new("symbol");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");
    mpc_parser_t* Sexpr = mpc_new("sexpr");
    mpca_lang(MPCA_LANG_DEFAULT,
    	"\
    	number:/-?[0-9]+/;\
    	symbol:'+' |'-' | '*'| '/';\
        sexpr:'(' <expr>* ')';\
    	expr:<number> | <symbol> | <sexpr>;\
    	lispy: <expr>*;\
    	",
    	Number,Symbol,Expr,Lispy,Sexpr);
    while(1){
    	fputs("lispy> ",stdout);
    	fgets(input,2048,stdin);
        if(strstr(input,"exit()")){
            puts("exit success");
            break;
        }
        if(strlen(input)<=1){
            continue;
        }
    	mpc_result_t r;
    	if(mpc_parse("<stdin>",input,Lispy,&r)){
    		mpc_ast_t* a = r.output;
    		print_lval(eval(a));
            printf("\n");
    		//mpc_ast_t* a0 = a->children[0];
    		//mpc_ast_print(r.output);
    		mpc_ast_delete(r.output);
    	}
    	else{
    		mpc_err_print(r.error);
    		mpc_err_delete(r.error);
    	}
    }
    mpc_cleanup(4,Number,Symbol,Expr,Lispy,Sexpr);
    return 0;
}
