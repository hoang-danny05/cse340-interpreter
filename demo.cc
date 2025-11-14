#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <vector>
#include <map>
#include <iostream>
#include "execute.h"
#include "lexer.h"

const char * token_string[] =  { "END_OF_FILE", \
    "VAR", "FOR", "IF", "WHILE", "SWITCH", "CASE", "DEFAULT", "INPUT", "OUTPUT", "ARRAY",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN", "LBRACE", "RBRACE",
    "NOTEQUAL", "GREATER", "LESS",
    "NUM", "ID", "ERROR"
};


// NOTE: even though this file does not peek() at or get() any
// tokens, the very fact that a lexer is declared
// requires that you provide input to the program
// when you execute it even if you don't add any code
// to execute just run ./a.out < test, where test is any
// test case
LexicalAnalyzer lexer;   
const bool DEBUG = true;


using namespace std;
map<string, int> location_of; // returns address of variable
map<int, int> const_location; // returns address of const

void append_const(const int newConst) {
    // add const into memory AND inserts it into const_location
    

    // do nothing if already in list
    if(const_location.find(newConst) != const_location.end()) {
        debug("const %d already in consts!");
        return;
    }


    const_location.insert({newConst, next_available});
    mem[next_available] = newConst;
    next_available++;

}

struct InstructionNode *start = new InstructionNode;
struct InstructionNode *recent_instr = start;
int instr_num = 1;

void append_instruction(InstructionNode* newInstr) {
    recent_instr->next = newInstr;
    recent_instr = newInstr;
    recent_instr->next = NULL;
    instr_num++;
}


void debug(const char* format, ...)
{
    va_list args;
    if (DEBUG)
    {
        va_start (args, format);
        vfprintf (stdout, format, args);
        cout << endl;
        va_end (args);
    }
}

void error() {
    cout << ("SYNTAX ERROR!");
    exit(-1);
}

void assertTokenType(Token t, TokenType type, const char* msg) {
    if (t.token_type != type) {
        debug(msg);
        debug("Got %s, expected %s", 
           token_string[t.token_type],
           token_string[type]);
        error();
    }
}

void assertTokenType(Token t, vector<TokenType> types, const char* msg) {
    for (TokenType type : types) {
        // if any type is equal
        if (t.token_type == type) 
            return;
    }
    debug(msg);
    debug("Got %s", token_string[t.token_type]);
    error();
}



void parse_var_section() {
    while (lexer.peek(1).token_type == ID) {

        debug("Inserting locationof(%s) == %d", lexer.peek(1).lexeme.c_str(), next_available);
        // int address_for_i = next_available;

        const Token t = lexer.GetToken();
        location_of.insert({t.lexeme, next_available});
        mem[next_available] = 0;
        next_available++;


        const Token follow = lexer.GetToken();
        if (follow.token_type == SEMICOLON) {
            debug("Comma no longer present!");
            break;
        }

        // neither a semicolon or whatever
        assertTokenType(follow, COMMA, "Expected Comma or semicolon!");
    }
}

void parse_statement(void);
void parse_assign(void);
void parse_while(void);
void parse_if(void);
void parse_switch(void);
void parse_for(void);
void parse_output(void);
void parse_input(void);

struct InstructionNode* parse_body() {
    assertTokenType(lexer.GetToken(), LBRACE, "Body expected starting LBRACE!");

    // stmt_list
    while (true) {
        // check if it is a statement
        if(lexer.peek(1).token_type == RBRACE) {
            lexer.GetToken();
            debug("END OF CURRENT BODY");
            break;
        }

        parse_statement();
    }
    return recent_instr;
}

void parse_statement() {
    /*
    assign stmt | 
        FIRST ID
    while stmt | 
        FIRST WHILE
    if stmt | 
        FIRST IF
    switch stmt | 
        FIRST SWITCH
    for stmt
        FIRST FOR
    output stmt |
        FIRST OUTPTUT
    input stmt
        FIRST INPUT
    */
   if (DEBUG)
    printf("[%d] ", instr_num);
   switch (lexer.peek(1).token_type) {
    case ID:
        debug("Adding ASSIGN Instruction");
        parse_assign();
        break;
    case WHILE:
        debug("Adding WHILE Instruction");
        // parse_while();
        break;
    case IF:
        debug("Adding IF Instruction");
        parse_if();
        break;
    case SWITCH:
        debug("Adding SWITCH Instruction");
        // parse_switch();
        break;
    case FOR:
        debug("Adding FOR Instruction");
        // parse_for();
        break;
    case OUTPUT:
        debug("Adding OUTPUT Instruction");
        parse_output();
        break;
    case INPUT:
        debug("Adding INPUT Instruction");
        parse_input();
        break;
    default:
        debug("SWITCH STATEMENT DEFAULT!!");
        error();
   }
}

void parse_condition(InstructionNode* instr) {
    Token p1 = lexer.GetToken();
    Token comp = lexer.GetToken();
    Token p2 = lexer.GetToken();
    assertTokenType(p1, {NUM, ID}, "Condition expected PRIMARY in spot 1");
    assertTokenType(comp, {GREATER, LESS, NOTEQUAL}, "Condition expected Comparator at pos 3");
    assertTokenType(p2, {NUM, ID}, "Condition expected PRIMARY in spot 2");

    if (p1.token_type == NUM) {
        append_const(stoi(p1.lexeme));
        instr->cjmp_inst.op1_loc = const_location[stoi(p1.lexeme)];
    }
    else {
        instr->cjmp_inst.op1_loc = location_of[p1.lexeme];
    }

    instr->cjmp_inst.condition_op = condOperatorType.at(comp.token_type);

    if (p2.token_type == NUM) {
        append_const(stoi(p2.lexeme));
        instr->cjmp_inst.op2_loc = const_location[stoi(p2.lexeme)];
    }
    else {
        instr->cjmp_inst.op2_loc = location_of[p2.lexeme];
    }

}

void parse_if(void) {
    InstructionNode *cjump = new InstructionNode;
    InstructionNode *nop_target = new InstructionNode;
    cjump->cjmp_inst.target = nop_target;
    nop_target->type = NOOP;
    assertTokenType(lexer.GetToken(), IF, "expected IF");
    parse_condition(cjump);

    parse_body();

    append_instruction(nop_target);
}



void parse_input(void) {
    // input ID semicolon
    struct InstructionNode *newInstr = new InstructionNode;
    assertTokenType(lexer.GetToken(), INPUT, "err");

    Token t = lexer.GetToken();
    assertTokenType(t, ID, "Expected ID in parse_input");

    assertTokenType(lexer.GetToken(), SEMICOLON, "Expected ; in parse_input");

    newInstr->type = IN;
    newInstr->input_inst.var_loc = location_of[t.lexeme];
    append_instruction(newInstr);
}

void parse_output(void) {
    // input ID semicolon
    struct InstructionNode *newInstr = new InstructionNode;
    assertTokenType(lexer.GetToken(), OUTPUT, "err");

    Token t = lexer.GetToken();
    assertTokenType(t, ID, "Expected ID in parse_output");

    assertTokenType(lexer.GetToken(), SEMICOLON, "Expected ; in parse_output");

    newInstr->type = OUT;
    newInstr->input_inst.var_loc = location_of[t.lexeme];
    append_instruction(newInstr);
}

void parse_assign() {
    struct InstructionNode *newInstr = new InstructionNode;
    newInstr->type = ASSIGN;
    

    Token left = lexer.GetToken();
    assertTokenType(left, ID, "Error with STATEMENT->ASSIGN->ID");
    newInstr->assign_inst.lhs_loc = location_of[left.lexeme];

    assertTokenType(lexer.GetToken(), EQUAL, "Error with STATEMENT->ASSIGN->EQUAL");

    // now, either primary or expression
    Token p1 = lexer.GetToken();
    assertTokenType(p1, {ID, NUM}, "Error with STATEMENT->ASSIGN->PRIMARY");

    if (p1.token_type == NUM) {
        // left = p1 (num)
        const int rhs_1 = stoi(p1.lexeme);
        append_const(rhs_1);
        newInstr->assign_inst.op1_loc = const_location[rhs_1];
    } else { // token type == ID
        const string rhs_1 = p1.lexeme;
        newInstr->assign_inst.op1_loc = location_of[rhs_1];
    }

    if (lexer.peek(1).token_type == SEMICOLON) {
        lexer.GetToken(); // everything consumed
        newInstr->assign_inst.op = OPERATOR_NONE;

        append_instruction(newInstr);
        return;
    }
    else {
        Token op = lexer.GetToken();
        assertTokenType(op , {PLUS, MINUS, MULT, DIV}, "Error with STATEMENT->ASSIGN->PRIMARY2");
        Token p2 = lexer.GetToken();
        assertTokenType(p2, {ID, NUM}, "Error with STATEMENT->ASSIGN->PRIMARY2");
        assertTokenType(lexer.GetToken(), SEMICOLON, "Error with STATEMENT->ASSIGN->SEMICOLON");
        

        newInstr->assign_inst.op = arithOperatorType.at(op.token_type);

        if (p2.token_type == NUM) {
            // left = p1 op p2(num)
            const int rhs_2 = stoi(p2.lexeme);
            append_const(rhs_2);
            newInstr->assign_inst.op2_loc = const_location[rhs_2];
        } else { // token type == ID
            const string rhs_2 = p2.lexeme;
            newInstr->assign_inst.op2_loc = location_of[rhs_2];
        }

        append_instruction(newInstr);
        return;
    }
}

void parse_inputs_section(void) {
    // NUM LIST (NUM NUM ... NUM EOF)
    while (true) {
        Token t = lexer.GetToken();
        if (t.token_type == END_OF_FILE) {
            return;
        }
        assertTokenType(t, NUM, "EXPECTED NUMBERS IN INPUTS");
        debug("Adding input %s", t.lexeme.c_str());
        inputs.push_back(stoi(t.lexeme));
    }
}


struct InstructionNode* parse_Generate_Intermediate_Representation() {
    start->type = NOOP;
    debug("===========Entering Var Section:");
    parse_var_section();
    debug("===========Entering Body Section:");
    parse_body();
    debug("===========Entering Inputs Section:");
    parse_inputs_section();
    debug("===========DONE, EXECUTING CODE==========");
    return start;
}


struct InstructionNode *pre_parse_Generate_Intermediate_Representation()
{
     // Sample program for demonstration purpose only
     // Replace the following with a call to a parser that reads the
     // program from stdin & creates appropriate data structures to be
     // executed by execute_program()
     // This is the supposed input for the following construction:

     // a, b, c, d;
     // {
     //     input a;
     //     input b;
     //     c = 10;
     //
     //     IF c <> a
     //     {
     //         output b;
     //     }
     //
     //     IF c > 1
     //     {
     //         a = b + 900;
     //         input d;
     //         IF a > 10
     //         {
     //             output d;
     //         }
     //     }
     //
     //     d = 0;
     //     WHILE d < 4
     //     {
     //         c = a + d;
     //         IF d > 1
     //         {
     //             output d;
     //         }
     //         d = d + 1;
     //     }
     // }
     // 1 2 3 4 5 6

     // Assigning location for variable "a"
     int address_a = next_available;
     mem[next_available] = 0; // next_available is a global variable that is decalred in
     next_available++;        // execute.cc

     // Assigning location for variable "b"
     int address_b = next_available;
     mem[next_available] = 0;
     next_available++;

     // Assigning location for variable "c"
     int address_c = next_available;
     mem[next_available] = 0;
     next_available++;

     // Assigning location for variable "d"
     int address_d = next_available;
     mem[next_available] = 0;
     next_available++;

     // Assigning location for constant 10
     int address_ten = next_available;
     mem[next_available] = 10;
     next_available++;

     // Assigning location for constant 1
     int address_one = next_available;
     mem[next_available] = 1;
     next_available++;

     // Assigning location for constant 900
     int address_ninehundred = next_available;
     mem[next_available] = 900;
     next_available++;

     // Assigning location for constant 3
     int address_three = next_available;
     mem[next_available] = 3;
     next_available++;

     // Assigning location for constant 0
     int address_zero = next_available;
     mem[next_available] = 0;
     next_available++;

     // Assigning location for constant 4
     int address_four = next_available;
     mem[next_available] = 4;
     next_available++;

     struct InstructionNode *i1 = new InstructionNode;
     struct InstructionNode *i2 = new InstructionNode;
     struct InstructionNode *i3 = new InstructionNode;
     struct InstructionNode *i4 = new InstructionNode;
     struct InstructionNode *i5 = new InstructionNode;
     struct InstructionNode *i6 = new InstructionNode;
     struct InstructionNode *i7 = new InstructionNode;
     struct InstructionNode *i8 = new InstructionNode;
     struct InstructionNode *i9 = new InstructionNode;
     struct InstructionNode *i10 = new InstructionNode;
     struct InstructionNode *i11 = new InstructionNode;
     struct InstructionNode *i12 = new InstructionNode;
     struct InstructionNode *i13 = new InstructionNode;
     struct InstructionNode *i14 = new InstructionNode;
     struct InstructionNode *i15 = new InstructionNode;
     struct InstructionNode *i16 = new InstructionNode;
     struct InstructionNode *i17 = new InstructionNode;
     struct InstructionNode *i18 = new InstructionNode;
     struct InstructionNode *i19 = new InstructionNode;
     struct InstructionNode *i20 = new InstructionNode;
     struct InstructionNode *i21 = new InstructionNode;
     struct InstructionNode *i22 = new InstructionNode;

     i1->type = IN; // input a
     i1->input_inst.var_loc = address_a;
     i1->next = i2;

     i2->type = IN; // input b
     i2->input_inst.var_loc = address_b;
     i2->next = i3;

     i3->type = ASSIGN; // c = 10
     i3->assign_inst.lhs_loc = address_c;
     i3->assign_inst.op = OPERATOR_NONE;
     i3->assign_inst.op1_loc = address_ten;
     i3->next = i4;

     i4->type = CJMP; // if c <> a
     i4->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
     i4->cjmp_inst.op1_loc = address_c;
     i4->cjmp_inst.op2_loc = address_a;
     i4->cjmp_inst.target = i6; // if not (c <> a) skip forward to NOOP
     i4->next = i5;

     i5->type = OUT; // output b
     i5->output_inst.var_loc = address_b;
     i5->next = i6;

     i6->type = NOOP; // NOOP after IF
     i6->next = i7;

     i7->type = CJMP; // if c > 1
     i7->cjmp_inst.condition_op = CONDITION_GREATER;
     i7->cjmp_inst.op1_loc = address_c;
     i7->cjmp_inst.op2_loc = address_one;
     i7->cjmp_inst.target = i13; // if not (c > 1) skip forward to NOOP (way down)
     i7->next = i8;

     i8->type = ASSIGN; // a = b + 900
     i8->assign_inst.lhs_loc = address_a;
     i8->assign_inst.op = OPERATOR_PLUS;
     i8->assign_inst.op1_loc = address_b;
     i8->assign_inst.op2_loc = address_ninehundred;
     i8->next = i9;

     i9->type = IN; // input d
     i9->input_inst.var_loc = address_d;
     i9->next = i10;

     i10->type = CJMP; // if a > 10
     i10->cjmp_inst.condition_op = CONDITION_GREATER;
     i10->cjmp_inst.op1_loc = address_a;
     i10->cjmp_inst.op2_loc = address_ten;
     i10->cjmp_inst.target = i12; // if not (a > 10) skipp forward to NOOP
     i10->next = i11;

     i11->type = OUT; // output d
     i11->output_inst.var_loc = address_d;
     i11->next = i12;

     i12->type = NOOP; // NOOP after inner IF
     i12->next = i13;

     i13->type = NOOP; // NOOP after outer IF
     i13->next = i14;

     i14->type = ASSIGN; // d = 0
     i14->assign_inst.lhs_loc = address_d;
     i14->assign_inst.op = OPERATOR_NONE;
     i14->assign_inst.op1_loc = address_zero;
     i14->next = i15;

     i15->type = CJMP; // if d < 4
     i15->cjmp_inst.condition_op = CONDITION_LESS;
     i15->cjmp_inst.op1_loc = address_d;
     i15->cjmp_inst.op2_loc = address_four;
     i15->cjmp_inst.target = i22; // if not (d < 4) skip whole WHILE body
     i15->next = i16;

     i16->type = ASSIGN; // c = a + d
     i16->assign_inst.lhs_loc = address_c;
     i16->assign_inst.op = OPERATOR_PLUS;
     i16->assign_inst.op1_loc = address_a;
     i16->assign_inst.op2_loc = address_d;
     i16->next = i17;

     i17->type = CJMP; // if d > 1
     i17->cjmp_inst.condition_op = CONDITION_GREATER;
     i17->cjmp_inst.op1_loc = address_d;
     i17->cjmp_inst.op2_loc = address_one;
     i17->cjmp_inst.target = i19; // if not (d > 1) skip body of IF
     i17->next = i18;

     i18->type = OUT; // output d
     i18->output_inst.var_loc = address_d;
     i18->next = i19;

     i19->type = NOOP; // NOOP after body of IF
     i19->next = i20;

     i20->type = ASSIGN; // d = d + 1
     i20->assign_inst.lhs_loc = address_d;
     i20->assign_inst.op = OPERATOR_PLUS;
     i20->assign_inst.op1_loc = address_d;
     i20->assign_inst.op2_loc = address_one;
     i20->next = i21;

     i21->type = JMP;
     i21->jmp_inst.target = i15;
     i21->next = i22;

     i22->type = NOOP; // NOOP after body of WHILE
     i22->next = NULL;

     // Inputs
     inputs.push_back(1);
     inputs.push_back(2);
     inputs.push_back(3);
     inputs.push_back(4);
     inputs.push_back(5);
     inputs.push_back(6);

     return i1;
 }
