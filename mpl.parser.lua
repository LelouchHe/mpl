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

- Ŀǰ�Ǳ�����һ��global�ĵ������
chunk: opt_block

opt_block:
opt_block: last_statement
opt_block: stats
opt_block: stats last_statement

- function-call()
stat_class_1: nobr_function_call

- (function-call)()
stat_class_2: br_function_call

- non-function-call
stat_class_3: nobr_statement
- function-call();
stat_class_3: nobr_statement ';'
- non-function-call;
stat_class_3: nobr_function_call ';'

- (non-function-call)
stat_class_4: br_statement
- (non-function-call);
stat_class_4: br_statement ';'
- (function-call)()
stat_class_4: br_function_call ';'

stats: stats_1
stats: stats_2
stats: stats_3
stats: stats_4

stats_1: stat_class_1
stats_1: stats_1 stat_class_1
stats_1: stats_2 stat_class_1
stats_1: stats_3 stat_class_1
stats_1: stats_4 stat_class_1

stats_2: stat_class_2
stats_2: stats_3 stat_class_2
stats_2: stats_4 stat_class_2

stats_3: stat_class_3
stats_3: stats_1 stat_class_3
stats_3: stats_2 stat_class_3
stats_3: stats_3 stat_class_3
stats_3: stats_4 stat_class_3

stats_4: stat_class_4
stats_4: stats_3 stat_class_4
stats_4: stats_4 stat_class_4

- ����'('��ͷ����ͨ���
nobr_statement: nobr_variable_list '=' expression_list
nobr_statement: 'do' opt_block 'end'
nobr_statement: 'while' expression 'do' opt_block 'end'
nobr_statement: 'repeat' opt_block 'until' expression
nobr_statement: 'if' expression 'then' opt_block opt_elseif_block_list opt_else_block 'end'
nobr_statement: 'for' ID '=' expression ',' expression 'do' opt_block 'end'
nobr_statement: 'for' ID '=' expression ',' expression ',' expression 'do' opt_block 'end'
nobr_statement: 'for' id_list 'in' expression_list 'do' opt_block 'end'
nobr_statement: 'function' func_name_list function_body
nobr_statement: 'function' func_name_list ':' ID function_body
nobr_statement: 'local' 'function' ID function_body
nobr_statement: 'local' id_list
nobr_statement: 'local' id_list '=' expression_list
- norb_statement: ID STRING

- '('��ͷ����ͨ���
br_statement: br_variable_list '=' expression_list

- if���
opt_elseif_block_list:
opt_elseif_block_list: elseif_block_list

elseif_block_list: elseif_block
elseif_block_list: elseif_block_list elseif_block

elseif_block: 'elseif' expression 'then' opt_block

opt_else_block:
opt_else_block: 'else' opt_block

- ������
last_statement: 'return' opt_semicolon
last_statement: 'return' expression_list opt_semicolon
last_statement: 'break' opt_semicolon

opt_semicolon:
opt_semicolon: ';'

- var�б�
nobr_variable_list: nobr_variable
nobr_variable_list: nobr_variable_list ',' nobr_variable
nobr_variable_list: nobr_variable_list ',' br_variable

br_variable_list: br_variable
br_variable_list: br_variable_list ',' nobr_variable
br_variable_list: br_variable_list ',' br_variable

- ������
func_name_list: ID
func_name_list: func_name_list '.' ID

- ���ʽ
expression: 'nil'
expression: 'false'
expression: 'true'
expression: NUMBER
expression: STRING
expression: '...'
expression: 'function' function_body
expression: nobr_prefix_expression
expression: '(' expression ')'
expression: table_constructor
expression: expression '..' expression
expression: expression '+' expression
expression: expression '-' expression
expression: expression '*' expression
expression: expression '/' expression
expression: expression '^' expression
expression: expression '%' expression
expression: expression '<' expression
expression: expression '<=' expression
expression: expression '>' expression
expression: expression '==' expression
expression: expression '~=' expression
expression: expression '>=' expression
expression: expression 'and' expression
expression: expression 'or' expression
- һԪ������,������Ҫ����
expression: 'not' expression
expression: '-' expression
expression: '#' expression

expression_list: expression
expression_list: expression_list ',' expression

- ��,���prefix��û����ǰ������
nobr_prefix_expression: nobr_variable
nobr_prefix_expression: nobr_function_call

- ����
nobr_variable: ID
nobr_variable: nobr_prefix_expression '[' expression ']'
nobr_variable: nobr_prefix_expression '.' ID

br_variable: '(' expression ')' '[' expression ']'
br_variable: '(' expression ')' '.' ID

- ����
nobr_function_call: nobr_prefix_expression arguments
nobr_function_call: nobr_prefix_expression ':' ID arguments

br_function_call: '(' expression ')' arguments
br_function_call: '(' expression ')' ':' ID arguments

arguments: '(' ')'
arguments: '(' expression_list ')'
arguments: table_constructor
arguments: STRING

function_body: '(' opt_parameter_list ')' opt_block 'end'

opt_parameter_list: 
opt_parameter_list: '...'
opt_parameter_list: id_list
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

id_list: ID
id_list: id_list ',' ID

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
