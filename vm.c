#include <stdio.h>
#include <string.h>
#define MAX_PAS_LENGTH 500
#define MAX_TEXT_LENGTH 500
#define MAX_STACK_LENGTH 500
#define MAX_INFILE_LENGTH 256

typedef struct instr {
    int OP;
    int L;
    int M;
} instr;

// Easy access/maintence for coding for current instruction.
instr IR;    
int PC;
int BP;
int SP;
int pas[MAX_STACK_LENGTH];

// Instruction Set Architecture abreviations (1-9). 
const char ISA_ABRV[10][4] = {"", "LIT", "OPR", "LOD", "STO", "CAL", "INC", "JMP",
                        "JPC", "SYS"};

// Operations abreviations (0-12).
const char OPR_ABRV[14][4] = {"RTN", "NEG", "ADD", "SUB", "MUL", "DIV", "ODD", "MOD", "EQL",
                        "NEQ", "LSS", "LEQ", "GTR", "GEQ"};

// Given base function.
int base(int L) {
    int arb = BP;
    while (L > 0){
        arb = pas[arb];
        L--;
    }
    return arb;
}

// Driver code.
int main(int argc, char *argv[]) {
    char file_name[MAX_INFILE_LENGTH];

    // Take in args.
    if (argc == 2) {
        strcpy(file_name, argv[1]);
        // printf("%s\n", file_name); TEST
    }

    FILE *in_file = fopen(file_name, "r"); 
    FILE *out_file = fopen("stacktrace.txt", "w");
   
    int text_size = 0;
    // Read in text.
    while (fscanf(in_file, "%d %d %d\n", &pas[text_size], &pas[text_size+1], &pas[text_size+2]) != EOF) {
        // printf("%d %d %d\n", cpu.text[i].OP, cpu.text[i].L, cpu.text[i].M);
        printf("%d %d %d\n", pas[text_size], pas[text_size+1], pas[text_size+2]);
        text_size += 3;
    }
    fclose(in_file);

    // Set initial values for the pas pointer, base pointer & progam counter.
    PC = 0;
    BP = text_size;
    SP = text_size - 1;
    
    // Tracks the beginning of the stack.
    int stack_pos = SP + 1;

    // Start formatting the pastrace.
    fprintf(out_file, "\t\t\tPC\tBP\tSP\tstack\n");
    fprintf(out_file, "Initial values:\t%d\t%d\t%d\n", PC, BP, SP);
    
    int halt = 1;
    while (halt != 0) {

        // Load current text instruction into intruction register.
        IR.OP = pas[PC];
        IR.L = pas[PC+1];
        IR.M = pas[PC+2];
        
        // Handle each instruction. Along with their respective M & L values.
        switch (IR.OP) {
            // LIT:
            case 1:
                SP++;
                pas[SP] = IR.M;
                PC+=3;
                break;
            // OPR:
            case 2:
                // Handle M value for OPR.
                switch(IR.M) {
                    // RTN:
                    case 0:
                        SP = BP - 1;
                        PC = pas[SP+3];
                        BP = pas[SP+2];
                        break;
                    // NEG:
                    case 1:
                        pas[SP] = -1 * pas[SP];
                        break;
                    // ADD:
                    case 2:
                        SP--;
                        pas[SP] = pas[SP] + pas[SP+1];
                        PC+=3;
                        break;
                    // SUB:
                    case 3:
                        SP--;
                        pas[SP] = pas[SP] - pas[SP+1];
                        PC+=3;
                        break;
                    // MUL:
                    case 4:
                        SP--;
                        pas[SP] = pas[SP] * pas[SP+1];
                        PC+=3;
                        break;
                    // DIV:
                    case 5:
                        SP--;
                        pas[SP] = pas[SP] / pas[SP+1];
                        PC+=3;
                        break;
                    // ODD:
                    case 6:
                        SP--;
                        pas[SP] = pas[SP] % 2;
                        PC+=3;
                        break;
                    // MOD:
                    case 7:
                        SP--;
                        pas[SP] = pas[SP] % pas[SP+1];
                        PC+=3;
                        break;
                    // EQL
                    case 8:
                        SP--;
                        if (pas[SP] == pas[SP+1]) pas[SP] = 1;
                        else pas[SP] = 0;
                        PC+=3;
                        break;
                    // NEQL:
                    case 9:
                        SP--;
                        if (pas[SP] != pas[SP+1]) pas[SP] = 1;
                        else pas[SP] = 0;
                        PC+=3;
                        break;
                    // LSS
                    case 10:
                        SP--;
                        if (pas[SP] < pas[SP+1]) pas[SP] = 1;
                        else pas[SP] = 0;
                        PC+=3;
                        break;
                    // LEQ:
                    case 11:
                        SP--;
                        if (pas[SP] <= pas[SP+1]) pas[SP] = 1;
                        else pas[SP] = 0;
                        PC+=3;
                        break;
                    // GTR:
                    case 12:
                        SP--;
                        if (pas[SP] > pas[SP+1]) pas[SP] = 1;
                        else pas[SP] = 0;
                        PC+=3;
                        break;
                    // GEQ:
                    case 13:
                        SP--;
                        if (pas[SP] > pas[SP+1]) pas[SP] = 1;
                        else pas[SP] = 0;
                        PC+=3;
                        break;
                }
                break;
            // LOD:
            case 3:
                SP++;
                pas[SP] = pas[base(IR.L) + IR.M];
                PC+=3;
                break;
            // STO:
            case 4:
                pas[base(IR.L) + IR.M] = pas[SP];
                SP--;
                PC+=3;
                break;
            // CAL:
            case 5:
                pas[SP+1] = base(IR.L);
                pas[SP+2] = BP;
                pas[SP+3] = PC+3;
                BP = SP+1;
                PC = IR.M;
                break;
            // INC:
            case 6:
                SP += IR.M;
                PC+=3;
                break;
            // JMP:
            case 7:
                PC = IR.M;
                break;
            // JPC:
            case 8:
                if (pas[SP] == 0) PC = IR.M;
                else PC+=3;
                SP--;
                break;
            // SYS:
            case 9:
                switch(IR.M) {
                    // Write top of stack to screen.
                    case 1:
                        printf("%d", pas[SP]);
                        fprintf(out_file, "Top of Stack Value: %d\n", pas[SP]);
                        SP--;
                        PC+=3;
                        break;
                    // Read input from user & store at the top of the stack.
                    case 2:
                        SP++;
                        printf("Please enter an Integer: ");
                        scanf("%d", &pas[SP]);
                        fprintf(out_file, "Please Enter an Integer: %d\n", pas[SP]);
                        PC+=3;
                        break;
                    // Halt flag to 0.
                    case 3:
                        halt = 0;
                        break;
                }
                break;
        }

        // Format data
        fprintf(out_file, "%s\t%d\t%d\t%d\t%d\t%d\t", IR.OP == 2 ? OPR_ABRV[IR.M] : ISA_ABRV[IR.OP], IR.L, IR.M, PC, BP, SP);

        // Format pas
        for (int i = stack_pos; i <= SP; i++) {
            if (BP != stack_pos && BP == i) fprintf(out_file, "| ");
            fprintf(out_file, "%d ", pas[i]);
        }
        fprintf(out_file, "\n");
    }

    fclose(out_file);
    return 1;
}
