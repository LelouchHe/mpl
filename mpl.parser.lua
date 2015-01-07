- include

#include "../ast/all_nodes.h"
#include "../ast/all_actions.h"

%%
- parser�﷨,����ֱ��дlexer�е�����(ID),Ҳ����д���ַ���('define', '(')
- Ҳ������re
- ��Ӿ����ast����
- actionԭ��
- void ReduceAction(const ParserNodePtr& left, const std::vector<ParserNodePtr>& right);
- ParserNode�а���token��ast��public�ṹ
- Ŀǰ��ֹ����action,��һ�����һ���ַ�ֻ����'{'
- ����һЩ�ظ�/��ѡ��ô�����Ի��ı��

- EBNF�޷�ֱ��ʹ��,��ת��ΪBNF
- {E} -> X = EPSILON | X E
- [E] -> X = EPSILON | E
- ת������﷨����LR�﷨,��Ҫ�ٴ�ת��
- ��Ҫ��stat:functioncall��prefixexp:functioncallǰ׺��ͬ��
- �ο� http://code.google.com/p/lua-checker/source/browse/trunk/lua.y
- ��������﷨����lalr�﷨,����shift/reduce��ͻ
- �ο�yacc�Ĺ���
- shift/reduce -> shift
- reduce/reduce -> ǰһ��reduce

- action������д����,�����Ͳ���ÿ����������parser��

- Ŀǰ�Ǳ�����һ��global�ĵ������
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

- ����'('��ͷ����ͨ���
- ���������ʵ�ASTNode,����ÿ������������?�о����߸�������
- ASTNode����ͨ,�������Ϳ������ĸ�ϸ
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

- '('��ͷ����ͨ���
br_statement: br_variable_list '=' expression_list

- if���
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

- ������
last_statement: 'return' opt_semicolon {
    return_action(left, right);
}
last_statement: 'return' expression_list opt_semicolon {
    return_exp_action(left, right);
}
last_statement: 'break' opt_semicolon

opt_semicolon:
opt_semicolon: ';'

- var�б�
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

- ������
func_name_list: ID {
    left->ast = ::mpl::ast::IDNode::create(right[0]->token.text);
}
func_name_list: func_name_list '.' ID {
    right[2]->ast = ::mpl::ast::IDNode::create(right[2]->token.text);
    loop(left, right[0], right[2]);
}

- ���ʽ
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
- һԪ������,������Ҫ����
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

- ��,���prefix��û����ǰ������
nobr_prefix_expression: nobr_variable {
    simple_action(left, right);
}
nobr_prefix_expression: nobr_function_call {
    simple_action(left, right);
}

- ����
nobr_variable: ID {
    left->ast = ::mpl::ast::IDNode::create(right[0]->token.text);
}
nobr_variable: nobr_prefix_expression '[' expression ']'
nobr_variable: nobr_prefix_expression '.' ID

br_variable: '(' expression ')' '[' expression ']'
br_variable: '(' expression ')' '.' ID

- ����
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
    // ��ʱ������'...'
    simple_action(left, right);
}
opt_parameter_list: id_list ',' '...'

- ��
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
- ���ȼ�+�����
- ֻ�����token
- ���ȼ��ӵ͵���
- �����: LEFT/RIGHT/NONE

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

- ����������������
- '-': 7 NONE

'^': 8 RIGHT

%%
