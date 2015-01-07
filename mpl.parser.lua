- include

#include "../ast/all_nodes.h"
#include "../ast/all_actions.h"

%%
- parser语法,可以直接写lexer中的名字(ID),也可以写纯字符串('define', '(')
- 也可以是re
- 后接具体的ast动作
- action原型
- void ReduceAction(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
- ParserNode中包含token和ast的public结构
- 目前禁止单行action,第一行最后一个字符只能是'{'
- 考虑一些重复/可选怎么更人性化的表达

- EBNF无法直接使用,先转化为BNF
- {E} -> X = EPSILON | X E
- [E] -> X = EPSILON | E
- 转化后的语法不是LR语法,需要再次转换
- 主要是stat:functioncall和prefixexp:functioncall前缀相同了
- 参考 http://code.google.com/p/lua-checker/source/browse/trunk/lua.y
- 不过这个语法不是lalr语法,存在shift/reduce冲突
- 参考yacc的规则
- shift/reduce -> shift
- reduce/reduce -> 前一个reduce

- action尽量都写函数,这样就不用每次重新生成parser了

- 目前是必须有一个global的单项存在
chunk: opt_block {
    left->ast = right[0]->ast;
}

opt_block:
opt_block: last_statement {
    simple_action(left, right);
}
opt_block: stats {
    simple_action(left, right);
}
opt_block: stats last_statement {
    loop(left, right[0], right[1]);
}

- function-call()
stat_class_1: nobr_function_call {
    simple_action(left, right);
}

- (function-call)()
stat_class_2: br_function_call {
    simple_action(left, right);
}

- non-function-call
stat_class_3: nobr_statement {
    simple_action(left, right);
}
- function-call();
stat_class_3: nobr_statement ';' {
    assign(left, right[0]);
}
- non-function-call;
stat_class_3: nobr_function_call ';' {
    assign(left, right[0]);
}

- (non-function-call)
stat_class_4: br_statement {
    simple_action(left, right);
}
- (non-function-call);
stat_class_4: br_statement ';' {
    assign(left, right[0]);
}
- (function-call)()
stat_class_4: br_function_call ';' {
    assign(left, right[0]);
}

stats: stats_1 {
    simple_action(left, right);
}
stats: stats_2 {
    simple_action(left, right);
}
stats: stats_3 {
    simple_action(left, right);
}
stats: stats_4 {
    simple_action(left, right);
}

stats_1: stat_class_1 {
    simple_action(left, right);
}
stats_1: stats_1 stat_class_1 {
    loop(left, right[0], right[1]);
}
stats_1: stats_2 stat_class_1 {
    loop(left, right[0], right[1]);
}
stats_1: stats_3 stat_class_1 {
    loop(left, right[0], right[1]);
}
stats_1: stats_4 stat_class_1 {
    loop(left, right[0], right[1]);
}

stats_2: stat_class_2 {
    simple_action(left, right);
}
stats_2: stats_3 stat_class_2 {
    loop(left, right[0], right[1]);
}
stats_2: stats_4 stat_class_2 {
    loop(left, right[0], right[1]);
}

stats_3: stat_class_3 {
    simple_action(left, right);
}
stats_3: stats_1 stat_class_3 {
    loop(left, right[0], right[1]);
}
stats_3: stats_2 stat_class_3 {
    loop(left, right[0], right[1]);
}
stats_3: stats_3 stat_class_3 {
    loop(left, right[0], right[1]);
}
stats_3: stats_4 stat_class_3 {
    loop(left, right[0], right[1]);
}

stats_4: stat_class_4 {
    simple_action(left, right);
}
stats_4: stats_3 stat_class_4 {
    loop(left, right[0], right[1]);
}
stats_4: stats_4 stat_class_4 {
    loop(left, right[0], right[1]);
}

- 不是'('开头的普通语句
- 究竟用普适的ASTNode,还是每次区分类型呢?感觉二者各有优劣
- ASTNode更普通,区分类型可以做的更细
nobr_statement: nobr_variable_list '=' expression_list {
    assign_action(left, right);
}
nobr_statement: 'do' opt_block 'end'
nobr_statement: 'while' expression 'do' opt_block 'end'
nobr_statement: 'repeat' opt_block 'until' expression
nobr_statement: 'if' expression 'then' opt_block opt_elseif_block_list opt_else_block 'end' {
    if_action(left, right);
}
nobr_statement: 'for' ID '=' expression ',' expression 'do' opt_block 'end'
nobr_statement: 'for' ID '=' expression ',' expression ',' expression 'do' opt_block 'end'
nobr_statement: 'for' id_list 'in' expression_list 'do' opt_block 'end'
nobr_statement: 'function' func_name_list function_body {
    func_def_action(left, right);
}
nobr_statement: 'function' func_name_list ':' ID function_body
nobr_statement: 'local' 'function' ID function_body
nobr_statement: 'local' id_list
nobr_statement: 'local' id_list '=' expression_list
- norb_statement: ID STRING

- '('开头的普通语句
br_statement: br_variable_list '=' expression_list

- if语句
opt_elseif_block_list:
opt_elseif_block_list: elseif_block_list {
    simple_action(left, right);
}

elseif_block_list: elseif_block {
    simple_action(left, right);
}
elseif_block_list: elseif_block_list elseif_block {
    loop(left, right[0], right[1]);
}

elseif_block: 'elseif' expression 'then' opt_block {
    elseif_action(left, right);
}

opt_else_block:
opt_else_block: 'else' opt_block {
    else_action(left, right);
}

- 结束符
last_statement: 'return' opt_semicolon {
    return_action(left, right);
}
last_statement: 'return' expression_list opt_semicolon {
    return_exp_action(left, right);
}
last_statement: 'break' opt_semicolon

opt_semicolon:
opt_semicolon: ';'

- var列表
nobr_variable_list: nobr_variable {
    simple_action(left, right);
}
nobr_variable_list: nobr_variable_list ',' nobr_variable {
    loop(left, right[0], right[2]);
}
nobr_variable_list: nobr_variable_list ',' br_variable {
    loop(left, right[0], right[2]);
}

br_variable_list: br_variable
br_variable_list: br_variable_list ',' nobr_variable
br_variable_list: br_variable_list ',' br_variable

- 函数名
func_name_list: ID {
    left->ast = ::mpl::ast::IDNode::create(right[0]->token.text);
}
func_name_list: func_name_list '.' ID {
    right[2]->ast = ::mpl::ast::IDNode::create(right[2]->token.text);
    loop(left, right[0], right[2]);
}

- 表达式
expression: 'nil' {
    left->ast = ::mpl::ast::NilNode::create();
}
expression: 'false' {
    left->ast = ::mpl::ast::BoolNode::create(false);
}
expression: 'true' {
    left->ast = ::mpl::ast::BoolNode::create(true);
}
expression: NUMBER {
    left->ast = ::mpl::ast::NumberNode::create(right[0]->token.text);
}
expression: STRING {
    left->ast = ::mpl::ast::StringNode::create(right[0]->token.text);
}
expression: '...'
expression: 'function' function_body
expression: nobr_prefix_expression {
    simple_action(left, right);
}
expression: '(' expression ')' {
    parenthesis_action(left, right);
}
expression: table_constructor
expression: expression '..' expression {
    binary_op_action(left, right);
}
expression: expression '+' expression {
    binary_op_action(left, right);
}
expression: expression '-' expression {
    binary_op_action(left, right);
}
expression: expression '*' expression {
    binary_op_action(left, right);
}
expression: expression '/' expression {
    binary_op_action(left, right);
}
expression: expression '^' expression {
    binary_op_action(left, right);
}
expression: expression '%' expression {
    binary_op_action(left, right);
}
expression: expression '<' expression {
    binary_op_action(left, right);
}
expression: expression '<=' expression {
    binary_op_action(left, right);
}
expression: expression '>' expression {
    binary_op_action(left, right);
}
expression: expression '>=' expression {
    binary_op_action(left, right);
}
expression: expression '==' expression {
    binary_op_action(left, right);
}
expression: expression '~=' expression {
    binary_op_action(left, right);
}
expression: expression 'and' expression {
    binary_op_action(left, right);
}
expression: expression 'or' expression {
    binary_op_action(left, right);
}
- 一元操作符,可能需要增加
expression: 'not' expression {
    unary_op_action(left, right);
}
expression: '-' expression {
    unary_op_action(left, right);
}
expression: '#' expression {
    unary_op_action(left, right);
}

expression_list: expression {
    simple_action(left, right);
}
expression_list: expression_list ',' expression {
    loop(left, right[0], right[2]);
}

- 看,这个prefix就没有以前的隐患
nobr_prefix_expression: nobr_variable {
    simple_action(left, right);
}
nobr_prefix_expression: nobr_function_call {
    simple_action(left, right);
}

- 变量
nobr_variable: ID {
    left->ast = ::mpl::ast::IDNode::create(right[0]->token.text);
}
nobr_variable: nobr_prefix_expression '[' expression ']'
nobr_variable: nobr_prefix_expression '.' ID

br_variable: '(' expression ')' '[' expression ']'
br_variable: '(' expression ')' '.' ID

- 函数
nobr_function_call: nobr_prefix_expression arguments {
    func_call_action(left, right);
}
nobr_function_call: nobr_prefix_expression ':' ID arguments

br_function_call: '(' expression ')' arguments
br_function_call: '(' expression ')' ':' ID arguments

arguments: '(' ')' {
    // none
}
arguments: '(' expression_list ')' {
    assign(left, right[1]);
}
arguments: table_constructor
arguments: STRING

function_body: '(' opt_parameter_list ')' opt_block 'end' {
    func_body_action(left, right);
}

opt_parameter_list: 
opt_parameter_list: '...'
opt_parameter_list: id_list {
    // 暂时不考虑'...'
    simple_action(left, right);
}
opt_parameter_list: id_list ',' '...'

- 表
table_constructor: '{' '}'
table_constructor: '{' field_list opt_field_separator '}'

field_list: field
field_list: field_list field_separator field

field: '[' expression ']' '=' expression
field: ID '=' expression
field: expression

field_separator: ','
field_separator: ';'

opt_field_separator:
opt_field_separator: field_separator

id_list: ID {
    left->ast = ::mpl::ast::IDNode::create(right[0]->token.text);
}
id_list: id_list ',' ID {
    right[2]->ast = ::mpl::ast::IDNode::create(right[2]->token.text);
    loop(left, right[0], right[2]);
}

%%
- 优先级+结合性
- 只能针对token
- 优先级从低到高
- 结合性: LEFT/RIGHT/NONE

'or': 1 LEFT
'and': 2 LEFT
'<': 3 LEFT
'<=': 3 LEFT
'>': 3 LEFT
'>=': 3 LEFT
'==': 3 LEFT
'~=': 3 LEFT
'..': 4 LEFT
'+': 5 LEFT
'-': 5 LEFT
'*': 6 LEFT
'/': 6 LEFT
'%': 6 LEFT
'not': 7 NONE
'#': 7 NONE

- 如何区分运算符类型
- '-': 7 NONE

'^': 8 RIGHT

%%
