#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#include <ctype.h>

enum {
	NOTYPE = 256, EQ = '=', Integer = 'i', Left = '(', Right = ')',
	Multiply = '*', Div = '/', Plus = '+', Sub = '-',
	Hex_Num = 'h', Reg_Name = 'r', NEQ = 'n', AND = 'A', OR = 'O',
	Factorial = 'F', DeReference = '?', Neg = 'm'

	/* TODO: Add more token types */
};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	// token_type is an integer, so use a char to represent it.

	// 1st level
	{" +",	NOTYPE},				// spaces
	{"0[xX][0-9a-fA-F]+", Hex_Num},      // hex-num
	{"[0-9]+", Integer},            // get an integer 这里可能需要更改
	{"\\$[a-z]+", Reg_Name},    // Register name

	// 2nd level
	{"\\(", Left},                  // left parenthesis
	{"\\)", Right},                 // right parenthesis

	// 3rd level
	{"\\*", Multiply},              // multiply
	{"/", Div}, 					// div

	// 4th level
	{"\\+", Plus},					// plus
	{"-", Neg},                     // Negitive number
	{"==", EQ},						// equal
	{"!=", NEQ},                    // not equal

	// 5th level
	{"&&", AND},                    // and
	{"||", OR},                     // or
	

};

#define NR_REGEX ( sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

// 单元结构体
typedef struct token {
	int type;
	char * str; // 为了避免缓冲区溢出, 直接利用堆的空间即可
	int priority; // 运算的优先级
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0; // 当前扫描的位置
	int i;
	regmatch_t pmatch;
	
	nr_token = 0; // token计数器


	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position; // 子串的起始位置
				int substr_len = pmatch.rm_eo; // 子串的长度

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				// printf("%s\n", e + position);
				// printf("%d\n", substr_len);

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */
				
				// 丢弃空格
				if ( rules[i].token_type == NOTYPE)
					continue;
				
				// 记录token的类型
				tokens[ nr_token ].type = rules[i].token_type;

				// 开辟一块新的空间来保存该子串, 留一个字符作为结束符
				char * new_space = (char *)malloc(substr_len + 1);
				strncpy(new_space, substr_start, substr_len);
				new_space[substr_len] = '\0'; // strcpy不会添加上\0结束符

				// int l = strlen(new_space);
				// printf("%d\n !!!!", l);
				// printf("%s\n", new_space);

				// 更新指针
				tokens[ nr_token ].str = new_space;
				// 确保字符串相等
				Assert(strlen(new_space) == substr_len, "copy failed!");

				// 更新计数器
				nr_token++;

				Assert(nr_token <= 32, "the tokens array over flow!");
				
				// 一些特殊的token必须经过某些处理, 现在还用不到
				switch(rules[i].token_type) {
					// default: panic("please implement me");
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}



/* char_Stack *************************************************************/
typedef struct {
	char * element;
	int top; // 栈顶指针 未放入
	int max_size; // 栈最大元素个数
} stack, *stack_p; // stack_p是指向栈结构体的指针

stack_p Creat_Stack(int max_size){
	// 开辟空间
	stack_p new_stack = (stack_p) malloc(sizeof(stack));
	new_stack->element = (char *) malloc(sizeof(max_size+2));
	memset(new_stack->element, 0, sizeof(max_size+2)); // 清空字符数组

	// 初始化
	new_stack->max_size = max_size;
	new_stack->top = 0;

	return new_stack;
}

void Destory_Stack(stack_p x){
	free(x->element);
	free(x);
}

bool is_empty(stack_p x){
	if (x->top == 0)
		return true;
	return false;
}

void push(stack_p x, char c){
	Assert(x->top < x->max_size, "栈已满.");
	x->element[ x->top++ ] = c;
}

void pop(stack_p x){
	Assert(!is_empty(x), "栈空, 但是进行了pop操作.");
	x->top--;
}

char top(stack_p x){
	Assert(!is_empty(x), "栈空, 无top元素.");
	return x->element[x->top-1];
}
/*********************************************************************/





/* 判断括号是否匹配 *****************************************************/
bool is_match(char * str){
    int len = strlen(str);

    // 开辟一个栈
    stack_p s = Creat_Stack(len);

    // 进行匹配
	int i;
    for (i = 0; i < len; i++)
        switch (str[i]){
            case '(':{
                push(s, str[i]); break;
            }
            case ')':{
                if (is_empty(s))
                    return false;
                else if (top(s) == '(')
                    pop(s);
                else
                    push(s, ')');
				break;
            }
			default: break;
        }
    
    // 判断结果
    if (is_empty(s)){
        Destory_Stack(s);
        return true;
    }
    else {
        Destory_Stack(s);
        return false;
    }
}
/*****************************************************************/





/* sub_str *******************************************************/
char * sub_str(char * str, int p, int q){
	// 返回p至q的子串
	Assert(p <= q, "p,q传参错误");

	char * res = (char *)malloc(q - p + 1 + 1); // 留一个结束符

	strncpy(res, str + p, q - p + 1);
	res[q-p+1] = '\0'; // 写入结束符

	return res;
}
/*****************************************************************/






/* 判断是否是配对的表达式(通过样例) ***********************************/
bool check_parentheses(char * expression, int p, int q){
	// 获取p至q子串
	char * temp = sub_str(expression, p, q);
	int len = strlen(temp);

	// 首先, 表达式的开头与结尾必须是一对括号
	if (temp[0] != '(' || temp[len-1] != ')') {
		free(temp);
		return false;
	}

	// 其次, 表达式必须符合括号匹配
	if (!is_match(temp)){
		free(temp);
		return false;
	}
	
	// 最后, 确保两边的括号是相互匹配的。
	// 要完成这一步, 只要保证剩余括号相互匹配即可
	char * sub_temp = sub_str(temp, 1, len-2);

	Assert(strlen(sub_temp) == len-2, "子串错误！");

	if (!is_match(sub_temp)){
		free(temp); free(sub_temp);
		return false;
	}

	free(temp); free(sub_temp);
	return true;
}
/**************************************************************/






/* 获取某个运算符的优先级 *****************************************/
int assign_priority(char c) {
	if (c == AND || c == OR)
		return 0;
	else if (c == EQ || c == NEQ)
		return 1;
	else if (c == Plus || c == Sub)
		return 2;
	else if (c == Multiply || c == Div)
		return 3;
	else if (c == Left || c == Right)
		return 4;
	else
		return 5;
}
/**************************************************************/


/* 判断是否可作为dominant operator ******************************/
bool is_d_op(char c) {
	// 暂时不包含指针解引用
	if (c == Plus || c == Sub || c == Multiply || 
	    c == Div || c == Left || c == Right ||
		c == AND || c == OR || c == NEQ || c == EQ)
		return true;
	
	return false;
}



/* 寻找dominant operator ***************************************/
int find_dominant_operator(int p, int q) {

	int min_priority = 1000;
	int index = -1;

	bool in_range = false; // 判断操作符是否在括号内部
	int i;
	for (i = p; i <= q; i++) {
		char c = tokens[i].type; // 取出操作的种类
		// 必须能够作为dominant operator
		if (!is_d_op(c))
			continue;
		
		// 若遇到右括号, 则更改当前的in_range状态
		if (c == Right) {
			if (in_range)
				in_range = false;
			continue;
		}

		// 若在括号内部
		if (in_range == true)
			continue;
		
		// 若遇到左括号, 就开启in_range状态
		if (c == Left) {
			in_range = true;
			continue;
		}

		// 剩余的符号都是操作符
		printf("%c\n", tokens[i].type);
		if (index == -1) { // 如果还没有赋值
			index = i;
			min_priority = tokens[i].priority;
		} else if (tokens[i].priority <= min_priority) {
			index = i;
			min_priority = tokens[i].priority;
		}
	}

	return index;
}
/**************************************************************/







/* 检查字符串是否为整数 ******************************************/
bool is_integer(char * expression) {

    int len = strlen(expression);
	
	int i;
    for (i = 0; i < len; i++){
        if (!isdigit(expression[i]))
            return false;
    }
    
    return true;

}
/**************************************************************/


/* 判断表达式是否为阶乘 ******************************************/
bool is_factorical(char * sub_expression) {
	return false;
}

bool is_negative(char * sub_expression) {
	return false;
}




/* 求解表达式的值主程序 ******************************************/
// 用32位的数据来保存位运算的结果
uint32_t get_value(char * expression, int p, int q) {
	// 先取出 p 至 q 的字符串
	char * sub_expression = sub_str(expression, p, q);
	uint32_t ret_val;

	if (p > q) {
		printf("出现了不可计算的情况, 结果不可信");
		ret_val = 0;

	} else if (is_integer(sub_expression)) {
		ret_val = atoi(sub_expression);

	//} else if (is_factorial(sub_expression)) {
	//	ret_val = factorial(sub_expression); // 待实现
	
	//} else if (is_negative(sub_expression)) {
	//	ret_val = negative(sub_expression); // 待实现
	
	} else if (check_parentheses(expression, p, q) == true) {
		ret_val = get_value(expression, p+1, q-1);

	} else {
		/* Complicated */
		// 1. 找出当前子串的dominant operator
		int d_op_ind = find_dominant_operator(p, q);
		Assert(d_op_ind != -1, "找不到dominant operator!");

		// 2. 根据dominant operator求解表达式
		uint32_t value1 = get_value(expression, p, p + d_op_ind-1);
		uint32_t value2 = get_value(expression, p + d_op_ind + 1, q);

		// 3. 根据dominant operator求值
		char d_op = tokens[p + d_op_ind].type;
		switch (d_op) {
			case Plus: {ret_val = value1 + value2; break;}
			case Sub: {ret_val = value1 - value2; break;}
			case Multiply: {ret_val = value1 * value2; break;}
			case Div: {ret_val = value1 / value2; break;}
			case EQ: {ret_val = value1 == value2; break;}
			case NEQ: {ret_val = value1 != value2; break;}
			case AND: {ret_val = value1 && value2; break;}
			case OR: {ret_val = value1 || value2; break;}

			default: Assert(0, "取出的操作符不太对劲儿");
		}
	}

	free(sub_expression);
    return ret_val;
}
/***************************************************************/






/* 输出表达式的值 ************************************************/
uint32_t expr(char *e, bool *success) {

	// 如果找不到正常的表达式就直接退出
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	*success = true;

	// 找出tokens中的减号
	int i;
	for (i = 1; i < nr_token && (strcmp(tokens[i].str, "-") == 0); i++)
		if (tokens[i-1].type == Right || tokens[i-1].type == Integer
		    || tokens[i-1].type == Hex_Num || tokens[i-1].type == Reg_Name)
			tokens[i].type = Sub;

	// 设置tokens中运算符的优先级
	for (i = 0; i < nr_token; i++){
		tokens[i].priority = assign_priority(tokens[i].type);
		printf("%d\n", tokens[i].priority);
	}
	
	// for (i = 0; i < nr_token; i++)
	// 	printf("%d ", tokens[i].priority);
	// printf("\n");
	
	// 找出tokens中的解引用


	// 拼接表达式
	int expression_size = 100;
	char * expression = (char *)malloc(expression_size); // 先设置100个字节的空间
	memset(expression, 0, expression_size);

	for (i = 0; i < nr_token; i++)
		strcat(expression, tokens[i].str);
	
	for (i = 0; i < nr_token; i++)
		printf("%c\n", tokens[i].type);

	
	printf("%s\n", expression);

	// 计算表达式
	int len = strlen(expression);
	uint32_t res = get_value(expression, 0, len-1);
	printf("计算结果为: %d\n", res);


	// 释放tokens中的字符串空间
	for (i = 0; i < nr_token; i++)
		free(tokens[i].str);

	// 释放表达式空间
	free(expression);

	// panic("please implement me");
	return 0;
}
/*******************************************************************/

