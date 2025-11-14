/*
 * Copyright (C) Rida Bazzi, 2017-2025
 *
 * Do not share this file with anyone
 */
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cctype>
#include <cstring>
#include <string>
#include "execute.h"
#include "lexer.h"



using namespace std;

const map<ConditionalOperatorType, const char *> condition = {
    {CONDITION_GREATER, ">"},
    {CONDITION_LESS, "<"},
    {CONDITION_NOTEQUAL, "!="}
};

#define DEBUG 0     // 1 => Turn ON debugging, 0 => Turn OFF debugging

int mem[1000];
int next_available = 0;

std::vector<int> inputs;
int next_input = 0;

int instr_no = 0;
void printNo() {
    debug("At instruction number %d (1-indexed)", instr_no);
}

void execute_program(struct InstructionNode * program)
{
    struct InstructionNode * pc = program;
    int op1, op2, result;

    while(pc != NULL)
    {
        instr_no++;
        switch(pc->type)
        {
            case NOOP:
                pc = pc->next;
                break;
            case IN:

                mem[pc->input_inst.var_loc] = inputs[next_input];
                next_input++;
                pc = pc->next;
                break;
            case OUT:
                printf("%d ", mem[pc->output_inst.var_loc]);
		fflush(stdin);
                pc = pc->next;
                break;
            case ASSIGN:
                switch(pc->assign_inst.op)
                {
                    case OPERATOR_PLUS:
                        op1 = mem[pc->assign_inst.op1_loc];
                        op2 = mem[pc->assign_inst.op2_loc];
                        result = op1 + op2;
                        break;
                    case OPERATOR_MINUS:
                        op1 = mem[pc->assign_inst.op1_loc];
                        op2 = mem[pc->assign_inst.op2_loc];
                        result = op1 - op2;
                        break;
                    case OPERATOR_MULT:
                        op1 = mem[pc->assign_inst.op1_loc];
                        op2 = mem[pc->assign_inst.op2_loc];
                        result = op1 * op2;
                        break;
                    case OPERATOR_DIV:
                        op1 = mem[pc->assign_inst.op1_loc];
                        op2 = mem[pc->assign_inst.op2_loc];
                        result = op1 / op2;
                        break;
                    case OPERATOR_NONE:
                        op1 = mem[pc->assign_inst.op1_loc];
                        result = op1;
                        break;
                }
                mem[pc->assign_inst.lhs_loc] = result;
                pc = pc->next;
                break;
            case CJMP:
                if (pc->cjmp_inst.target == NULL)
                {
                    debug("Error: pc->cjmp_inst->target is null.\n");
                    exit(1);
                }
                op1 = mem[pc->cjmp_inst.op1_loc];
                op2 = mem[pc->cjmp_inst.op2_loc];
                switch(pc->cjmp_inst.condition_op)
                {
                    // so it handles it for us, great!
                    case CONDITION_GREATER:
                        if(op1 > op2)
                            pc = pc->next;
                        else
                            pc = pc->cjmp_inst.target;
                        break;
                    case CONDITION_LESS:
                        if(op1 < op2)
                            pc = pc->next;
                        else
                            pc = pc->cjmp_inst.target;
                        break;
                    case CONDITION_NOTEQUAL:
                        if(op1 != op2)
                            pc = pc->next;
                        else
                            pc = pc->cjmp_inst.target;
                        break;
                }
                break;
            case JMP:
  
                if (pc->jmp_inst.target == NULL)
                {
                    debug("Error: pc->jmp_inst->target is null.\n");
                    exit(1);
                }
                pc = pc->jmp_inst.target;
                break;
            default:
                debug("Error: invalid value for pc->type (%d).\n", pc->type);
                printNo();
                exit(1);
                break;
        }
    }
}

void trace_execution(struct InstructionNode * program) {
    struct InstructionNode * pc = program;
    int op1, op2, result;
    int next_input = 0;
    int instr_count = 0;

    while(pc != NULL)
    {
        printf("[%d] ", instr_count++);
        switch(pc->type)
        {
            case NOOP:
                pc = pc->next;
                debug("NOOP");
                break;
            case IN:

                debug("IN addr [%d] <- %d", pc->input_inst.var_loc, inputs[next_input]);
                mem[pc->input_inst.var_loc] = inputs[next_input];
                next_input++;
                pc = pc->next;
                break;
            case OUT:
                debug("OUT %d", mem[pc->output_inst.var_loc]);
                printf("%d ", mem[pc->output_inst.var_loc]);
                fflush(stdin);
                pc = pc->next;
                break;
            case ASSIGN:
                switch(pc->assign_inst.op)
                {
                    case OPERATOR_PLUS:
                        op1 = mem[pc->assign_inst.op1_loc];
                        op2 = mem[pc->assign_inst.op2_loc];
                        result = op1 + op2;
                        debug("ASSIGN [%d] <- (%d + %d) = %d",
                            pc->assign_inst.lhs_loc,
                            op1,
                            op2,
                            result
                        );
                        break;
                    case OPERATOR_MINUS:
                        op1 = mem[pc->assign_inst.op1_loc];
                        op2 = mem[pc->assign_inst.op2_loc];
                        result = op1 - op2;
                        debug("ASSIGN [%d] <- (%d - %d) = %d",
                            pc->assign_inst.lhs_loc,
                            op1,
                            op2,
                            result
                        );
                        break;
                    case OPERATOR_MULT:
                        op1 = mem[pc->assign_inst.op1_loc];
                        op2 = mem[pc->assign_inst.op2_loc];
                        result = op1 * op2;
                        debug("ASSIGN [%d] <- (%d * %d) = %d",
                            pc->assign_inst.lhs_loc,
                            op1,
                            op2,
                            result
                        );
                        break;
                    case OPERATOR_DIV:
                        op1 = mem[pc->assign_inst.op1_loc];
                        op2 = mem[pc->assign_inst.op2_loc];
                        result = op1 / op2;
                        debug("ASSIGN [%d] <- (%d / %d) = %d",
                            pc->assign_inst.lhs_loc,
                            op1,
                            op2,
                            result
                        );
                        break;
                    case OPERATOR_NONE:
                        op1 = mem[pc->assign_inst.op1_loc];
                        result = op1;
                        debug("ASSIGN [%d] = %d",
                            pc->assign_inst.lhs_loc,
                            op1,
                            result
                        );
                        break;
                }
                mem[pc->assign_inst.lhs_loc] = result;
                pc = pc->next;
                break;
            case CJMP:
                if (pc->cjmp_inst.target == NULL)
                {
                    debug("Error: pc->cjmp_inst->target is null.\n");
                    exit(1);
                }
                op1 = mem[pc->cjmp_inst.op1_loc];
                op2 = mem[pc->cjmp_inst.op2_loc];
                debug("CJMP TARGET[%d] not IF (%d %s %d)",
                    pc->cjmp_inst.target_num,
                    op1,
                    condition.at(pc->cjmp_inst.condition_op),
                    op2
                );
                switch(pc->cjmp_inst.condition_op)
                {
                    // so it handles it for us, great!
                    case CONDITION_GREATER:
                        if(op1 > op2)
                            pc = pc->next;
                        else
                            pc = pc->cjmp_inst.target;
                        break;
                    case CONDITION_LESS:
                        if(op1 < op2)
                            pc = pc->next;
                        else
                            pc = pc->cjmp_inst.target;
                        break;
                    case CONDITION_NOTEQUAL:
                        if(op1 != op2)
                            pc = pc->next;
                        else
                            pc = pc->cjmp_inst.target;
                        break;
                }
                
                break;
            case JMP:
  
                if (pc->jmp_inst.target == NULL)
                {
                    debug("Error: pc->jmp_inst->target is null.\n");
                    exit(1);
                }
                pc = pc->jmp_inst.target;
                break;
            default:
                debug("Error: invalid value for pc->type (%d).\n", pc->type);
                printNo();
                exit(1);
                break;
        }
    }
}

const bool TRACE_EXECUTION = true;
int main()
{
    struct InstructionNode * program;
    program = parse_Generate_Intermediate_Representation();
    if (TRACE_EXECUTION) {
        trace_execution(program);
    }
    else {
        execute_program(program);
    }
    return 0;
}
