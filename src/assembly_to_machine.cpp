#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

map<string, int> ll;

string decimaltobinary(int num, int size) // Function that converts a decimal number() into binary string of particular size().
{
    if (num < 0) // If num is negative, convert it into it's 2's complement form
    {
        int m = 1;

        for (int i = 0; i < size; i++)
        {
            m *= 2;
        }
        num += m;
    }

    string bin(size, '0');

    for (int i = 0; i < size; i++) // Moving the each bit from binary string of decimal number to bin[] string using 'and' operand.
    {
        bin[size - 1 - i] = num & 1; // + '0'
        num = num >> 1;
    }

    return bin;
}

char tohexa(char *a) // Function that converts a 4-bit binary string into hexadecimal digit.
{
    int x = 1;
    int sum = 0;

    for (int i = 0; i < 4; i++) // This converts the 4-bit binary string into a decimal number.
    {
        sum += (x * a[3 - i]);
        x *= 2;
    }

    int y = '0';

    if (sum / 10) // Converting decimal number into hexadecimal character, and returning.
    {
        y = 'a';
    }

    y += (sum % 10);

    return y;
}

struct s3
{
    string inst; // initializing variables
    int f3;
    int f7;

    s3(string a, int b, int c) //  assigning values to variables if called with values.
    {
        inst = a;
        f3 = b;
        f7 = c;
    }

    s3() // Otherwise initialize with zero's
    {
        inst = "0";
        f3 = 0;
        f7 = 0;
    }
};

struct s2
{
    string a;
    int b;
};

// Array to store all the 32 registers in 'x_' and also in ABI names including indices 0 to 31.
s2 aliastable[65] = {
    {"x0", 0}, {"x1", 1}, {"x2", 2}, {"x3", 3}, {"x4", 4}, {"x5", 5}, {"x6", 6}, {"x7", 7}, {"x8", 8}, {"x9", 9}, {"x10", 10}, {"x11", 11}, {"x12", 12}, {"x13", 13}, {"x14", 14}, {"x15", 15}, {"x16", 16}, {"x17", 17}, {"x18", 18}, {"x19", 19}, {"x20", 20}, {"x21", 21}, {"x22", 22}, {"x23", 23}, {"x24", 24}, {"x25", 25}, {"x26", 26}, {"x27", 27}, {"x28", 28}, {"x29", 29}, {"x30", 30}, {"x31", 31}, {"zero", 0}, {"ra", 1}, {"sp", 2}, {"gp", 3}, {"tp", 4}, {"t0", 5}, {"t1", 6}, {"t2", 7}, {"s0", 8}, {"fp", 8}, {"s1", 9}, {"a0", 10}, {"a1", 11}, {"a2", 12}, {"a3", 13}, {"a4", 14}, {"a5", 15}, {"a6", 16}, {"a7", 17}, {"t3", 28}, {"t4", 29}, {"t5", 30}, {"t6", 31}, {"s2", 18}, {"s3", 19}, {"s4", 20}, {"s5", 21}, {"s6", 22}, {"s7", 23}, {"s8", 24}, {"s9", 25}, {"s10", 26}, {"s11", 27}};

s3 rftable[8]; // Arrray's to store the instruction's with instruction name, funct3, funct7(if exits, if not we can use it for other purposes).

s3 iftable[9];

s3 ilftable[7];

s3 ijftable[1];

s3 sftable[4];

s3 bftable[6];

int ifrf(string s) // Listing all the R-format instructions to check which the instruction in the input file.
{
    if (s == "add")
    {
        return 1;
    }

    else if (s == "sub")
    {
        return 1;
    }

    else if (s == "and")
    {
        return 1;
    }

    else if (s == "or")
    {
        return 1;
    }

    else if (s == "xor")
    {
        return 1;
    }

    else if (s == "sll")
    {
        return 1;
    }

    else if (s == "srl")
    {
        return 1;
    }

    else if (s == "sra")
    {
        return 1;
    }

    return 0;
}

int ifif(string s) // Listing the I-format instructions(other than load) to check which the instruction in the input file.
{
    if (s == "addi")
    {
        return 1;
    }

    else if (s == "andi")
    {
        return 1;
    }

    else if (s == "ori")
    {
        return 1;
    }

    else if (s == "xori")
    {
        return 1;
    }

    else if (s == "slli")
    {
        return 1;
    }

    else if (s == "srli")
    {
        return 1;
    }

    else if (s == "srai")
    {
        return 1;
    }

    return 0;
}

int ifilf(string s) // Listing the I-format instructions(Load type) to check which the instruction in the input file.
{
    if (s == "lb")
    {
        return 1;
    }

    else if (s == "lh")
    {
        return 1;
    }

    else if (s == "lw")
    {
        return 1;
    }

    else if (s == "ld")
    {
        return 1;
    }

    else if (s == "lbu")
    {
        return 1;
    }

    else if (s == "lhu")
    {
        return 1;
    }

    else if (s == "lwu")
    {
        return 1;
    }

    return 0;
}

int ifijf(string s) // checking for jalr instruction in the input
{
    if (s == "jalr")
    {
        return 1;
    }

    return 0;
}

int ifsf(string s) // checking for S-format instructions in the input.
{
    if (s == "sb")
    {
        return 1;
    }

    else if (s == "sh")
    {
        return 1;
    }

    else if (s == "sw")
    {
        return 1;
    }

    else if (s == "sd")
    {
        return 1;
    }

    return 0;
}

int ifbf(string s) // checking for the B-format instructions in the input.
{
    if (s == "beq")
    {
        return 1;
    }

    else if (s == "bne")
    {
        return 1;
    }

    else if (s == "blt")
    {
        return 1;
    }

    else if (s == "bge")
    {
        return 1;
    }

    else if (s == "bltu")
    {
        return 1;
    }

    else if (s == "bgeu")
    {
        return 1;
    }

    return 0;
}

int ifuf(string s) // checking for lui instruction
{
    if (s == "lui")
    {
        return 1;
    }

    return 0;
}

int ifujf(string s)
{
    if (s == "jal")
    {
        return 1;
    }

    return 0;
}

string checker(string a) // used for debugging
{
    for (int i = 0; i < a.size(); i++)
    {
        a[i] += '0';
    }

    return a;
}

string rformat(s3 s, int a, int b, int c) // Function for R-format instructions.
{
    string f3(3, '0');
    int f = s.f3;

    for (int i = 0; i < 3; i++)
    {
        f3[2 - i] = f & 1;
        f = f >> 1;
    }

    f = s.f7;
    string f7(7, '0');

    for (int i = 0; i < 7; i++)
    {
        f7[6 - i] = (f & 1);
        f = f >> 1;
    }

    string rd(5, '0');
    string rs1(5, '0');
    string rs2(5, '0');
    f = a;

    for (int i = 0; i < 5; i++)
    {
        rd[4 - i] = (f & 1);
        f = f >> 1;
    }

    f = b;

    for (int i = 0; i < 5; i++)
    {
        rs1[4 - i] = f & 1;
        f = f >> 1;
    }

    f = c;

    for (int i = 0; i < 5; i++)
    {
        rs2[4 - i] = f & 1;
        f = f >> 1;
    }

    string opcode = "0110011";

    for (int i = 0; i < 7; i++)
    {
        opcode[i] -= '0';
    }

    string bi = f7 + rs2 + rs1 + f3 + rd + opcode; // concatenation

    string h(8, '0');

    for (int i = 0; i < 8; i++)
    {
        h[i] = tohexa(&(bi[0]) + 4 * i);
    }

    return h;
}

string iformat(s3 s, int a, int b, int c) // Function for I-format instructions other than load
{
    int f = s.f3;
    string f3(3, '0');

    for (int i = 0; i < 3; i++)
    {
        f3[2 - i] = f & 1;
        f = f >> 1;
    }

    f = a;
    string rd(5, '0');

    for (int i = 0; i < 5; i++)
    {
        rd[4 - i] = (f & 1);
        f = f >> 1;
    }

    f = b;
    string rs1(5, '0');

    for (int i = 0; i < 5; i++)
    {
        rs1[4 - i] = f & 1;
        f = f >> 1;
    }

    string opcode = "0010011";

    for (int i = 0; i < 7; i++)
    {
        opcode[i] -= '0';
    }

    f = s.f7;
    string dummy = "000000";

    for (int i = 0; i < 6; i++)
    {
        dummy[i] -= '0';
    }

    string imm(12, '0');

    if (f == 0)
    {
        imm = decimaltobinary(c, 12);
    }

    else if (f == 1)
    {
        string dummy = "000000";

        for (int i = 0; i < 6; i++)
        {
            dummy[i] -= '0';
        }

        imm = dummy + decimaltobinary(c, 6);
    }

    string bin = imm + rs1 + f3 + rd + opcode;

    if (s.inst == "srai")
    {
        bin[1] = 1;
    }

    string h(8, '0');

    for (int i = 0; i < 8; i++)
    {
        h[i] = tohexa(&(bin[0]) + 4 * i);
    }

    return h;
}

string ilformat(s3 s, int a, int b, int c) // Function for load instructions.
{
    int f = s.f3;
    string f3(3, '0');

    for (int i = 0; i < 3; i++)
    {
        f3[2 - i] = f & 1;
        f = f >> 1;
    }

    f = a;
    string rd(5, '0');

    for (int i = 0; i < 5; i++)
    {
        rd[4 - i] = (f & 1);
        f = f >> 1;
    }

    f = b;
    string rs1(5, '0');

    for (int i = 0; i < 5; i++)
    {
        rs1[4 - i] = f & 1;
        f = f >> 1;
    }

    string opcode = "0000011";

    for (int i = 0; i < 7; i++)
    {
        opcode[i] -= '0';
    }

    f = s.f7;

    string imm(12, '0');

    imm = decimaltobinary(c, 12);

    string bin = imm + rs1 + f3 + rd + opcode;

    string h(8, '0');

    for (int i = 0; i < 8; i++)
    {
        h[i] = tohexa(&(bin[0]) + 4 * i);
    }

    return h;
}

string ijformat(s3 s, int a, int b, int c) // Function for jalr instructions
{
    int f = s.f3;
    string f3(3, '0');

    for (int i = 0; i < 3; i++)
    {
        f3[2 - i] = f & 1;
        f = f >> 1;
    }

    f = a;
    string rd(5, '0');

    for (int i = 0; i < 5; i++)
    {
        rd[4 - i] = (f & 1);
        f = f >> 1;
    }

    f = b;
    string rs1(5, '0');

    for (int i = 0; i < 5; i++)
    {
        rs1[4 - i] = f & 1;
        f = f >> 1;
    }

    string opcode = "1100111";

    for (int i = 0; i < 7; i++)
    {
        opcode[i] -= '0';
    }

    f = s.f7;

    string imm(12, '0');

    imm = decimaltobinary(c, 12);

    string bin = imm + rs1 + f3 + rd + opcode;

    string h(8, '0');

    for (int i = 0; i < 8; i++)
    {
        h[i] = tohexa(&(bin[0]) + 4 * i);
    }

    return h;
}

string sformat(s3 s, int a, int b, int c) // Function for S-Format instructions.
{
    string f3(3, '0');
    int f = s.f3;

    for (int i = 0; i < 3; i++)
    {
        f3[2 - i] = f & 1;
        f = f >> 1;
    }

    string rs1(5, '0');
    string rs2(5, '0');

    f = a;

    for (int i = 0; i < 5; i++)
    {
        rs1[4 - i] = (f & 1);
        f = f >> 1;
    }

    f = b;

    for (int i = 0; i < 5; i++)
    {
        rs2[4 - i] = f & 1;
        f = f >> 1;
    }

    string opcode = "0100011";

    for (int i = 0; i < 7; i++)
    {
        opcode[i] -= '0';
    }

    string imm = decimaltobinary(c, 12);
    string imm1(7, '0'), imm2(5, '0');

    for (int i = 0; i < 7; i++)
    {
        imm1[i] = imm[i];
    }
    for (int i = 0; i < 5; i++)
    {
        imm2[i] = imm[7 + i];
    }

    string bin = imm1 + rs1 + rs2 + f3 + imm2 + opcode;

    string h(8, '0');

    for (int i = 0; i < 8; i++)
    {
        h[i] = tohexa(&(bin[0]) + 4 * i);
    }

    return h;
}

string bformat(s3 s, int a, int b, int c)
{
    string f3(3, '0');
    int f = s.f3;

    for (int i = 0; i < 3; i++)
    {
        f3[2 - i] = f & 1;
        f = f >> 1;
    }

    string rs1(5, '0');
    string rs2(5, '0');

    f = a;

    for (int i = 0; i < 5; i++)
    {
        rs1[4 - i] = (f & 1);
        f = f >> 1;
    }

    f = b;

    for (int i = 0; i < 5; i++)
    {
        rs2[4 - i] = f & 1;
        f = f >> 1;
    }

    string opcode = "1100011";

    for (int i = 0; i < 7; i++)
    {
        opcode[i] -= '0';
    }

    string imm = decimaltobinary(c, 13);
    string imm1(6, '0'), imm2(4, '0');

    for (int i = 0; i < 6; i++)
    {
        imm1[i] = imm[i + 2];
    }

    for (int i = 0; i < 4; i++)
    {
        imm2[i] = imm[i + 8];
    }

    string bin = imm[0] + imm1 + rs2 + rs1 + f3 + imm2 + imm[1] + opcode;

    string h(8, '0');

    for (int i = 0; i < 8; i++)
    {
        h[i] = tohexa(&(bin[0]) + 4 * i);
    }

    return h;
}

int main() // Function main
{
    ifstream f("input.s"); // f here represents a represents the "input.s" file in same directory of code execution
    int l = 1;             // indicates the line we are in the loop
    string lc;             // the line gets stored in string

    while (getline(f, lc)) // this loop iterates on every line
    {
        int i = 0;
        int r = 0;

        for (; i < lc.size(); i++) // this loop verifies whether the line contains  a label
        {
            if (lc[i] == ':')
            {
                r = 1;
                break;
            }
        }

        string label; // if line contains the label it gets stored in this string

        if (r) // loop for storing label in "string label" if line contaiins a label
        {
            for (int j = 0; j < i; j++)
            {
                label.push_back(lc[j]);
            }

            ll[label] = l; // mapping to to label and its corresponding line number
        }

        l++;
    } // for now we will have all labels mapped to their line number in mapp ll

    ifstream file1("input.s");    // f1 indicates the "input.s" file
    ofstream file2("output.hex"); // f2 represents the file in which output is to be printed

    string ins; // store the instruction
    int line = 1;

    while (getline(file1, ins))
    {
        int q = 0;
        for (; q < ins.size(); q++)
        {
            if (ins[q] == ':')
            {
                q = 0;
                break;
            }
        }

        int remc = 0;

        if (!q)
        {
            for (int i = 0; i < ins.size(); i++)
            {
                if (ins[i] == ' ')
                {
                    remc = i + 1;
                    break;
                }
            }

            ins.erase(0, remc); // if label present , we will clear it from the line using erase
        }

        string m; // this stores the operator that is to be performed with given operands

        for (int i = 0; i < ins.size(); i++)
        {
            if (ins[i] == ' ')
            {
                break;
            }

            m.push_back(ins[i]);
        }

        // from now on we will write a if ,else ifs and else conditionals
        // this is segregating the operators to their corresponding group

        if (ifrf(m)) // For R-Format instructions
        {
            rftable[0] = s3("add", 0, 0);
            rftable[1] = s3("sub", 0x0, 0x20);
            rftable[2] = s3("xor", 0x4, 0x00);
            rftable[3] = s3("or", 0x6, 0x00); // loads instructions and their corresponding f3,f7
            rftable[4] = s3("and", 0x7, 0x00);
            rftable[5] = s3("sll", 0x1, 0x00);
            rftable[6] = s3("srl", 0x5, 0x00);
            rftable[7] = s3("sra", 0x5, 0x20);

            s3 data("0", 0, 0); // stores the data of the operator in the line

            int k = 0;
            string rd, rs1, rs2;                // registers
            int rdi = -1, rs1i = -1, rs2i = -1; // their corresponding values will get stored here

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ' ')
                {
                    k++;
                    break;
                }
            }

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ',')
                {
                    k += 2;
                    break;
                } // destination register separated

                rd.push_back(ins[k]);
            }

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ',')
                {
                    k += 2;
                    break;
                } // rs1 gets separated

                rs1.push_back(ins[k]);
            }

            for (; k < ins.size(); k++)
            {
                rs2.push_back(ins[k]); // rs2 gets separated
            }

            int e = 0;

            for (int i = 0; i < 65; i++)
            {
                if (rs1 == aliastable[i].a)
                {
                    rs1i = aliastable[i].b;
                    e += 1;
                }

                if (rs2 == aliastable[i].a)
                {
                    rs2i = aliastable[i].b;
                    e += 1;
                }

                if (rd == aliastable[i].a)
                {
                    rdi = aliastable[i].b;
                    e += 1;
                }
            }

            if (e != 3) // checks whether all three registers are valid are not
            {
                cout << "error in registers of line " << line << "\n";
                return 0;
            }
            // return 0 gets program terminated
            for (int i = 0; i < 10; i++)
            {
                if (rftable[i].inst == m)
                { // extracts data related to operation
                    data = rftable[i];
                    break;
                }
            }

            file2 << rformat(data, rdi, rs1i, rs2i) << endl;
            line++;
        }

        else if (ifif(m)) // For immediate format instructions other than load.
        {
            iftable[0] = s3("addi", 0, 0);
            iftable[1] = s3("srai", 0x5, 1);
            iftable[2] = s3("xori", 0x4, 0x00);
            iftable[3] = s3("ori", 0x6, 0x00); // stores instruction , f3, f7 together together in array of structs
            iftable[4] = s3("andi", 0x7, 0x00);
            iftable[5] = s3("slli", 0x1, 0x1);
            iftable[6] = s3("srli", 0x5, 0x1);

            s3 data("0", 0, 0);

            int k = 0;
            string rd, rs1, imm;     // registers to be stored
            int rdi = -1, rs1i = -1; // register values to be stored

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ' ')
                {
                    k++;
                    break;
                }
            }

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ',')
                {
                    k += 2;
                    break;
                }

                rd.push_back(ins[k]);
            }

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ',')
                {
                    k += 2;
                    break;
                }

                rs1.push_back(ins[k]);
            }

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ',')
                {
                    k += 2;
                    break;
                }

                imm.push_back(ins[k]);
            }

            int e = 0;

            for (int i = 0; i < 65; i++)
            {
                if (rs1 == aliastable[i].a)
                {
                    rs1i = aliastable[i].b;
                    e += 7;
                }

                if (rd == aliastable[i].a)
                {
                    rdi = aliastable[i].b;
                    e += 55;
                }
            }

            if (e != 62) // checks whether all registers are valid
            {
                file2 << "error in registers of line " << line << "\n";

                return 0;
            }

            for (int i = 0; i < 7; i++)
            {
                if (iftable[i].inst == m)
                {
                    data = iftable[i]; // extracts data related to operation
                    break;
                }
            }

            int sign = 0;
            if (imm.size() == 0)
            {
                cout << "error in line " << line << endl;
            }
            if (imm[0] == '-')
            {
                sign = 1;
            }

            int immv = 0;
            int power = 1;

            for (int i = imm.size() - 1; i >= sign; i--)
            {
                if (imm[i] > '9' || imm[i] < '0')
                {
                    cout << "immediate value is incorrect in line" << line << endl;
                    return 0;
                }

                immv += (imm[i] - '0') * power;
                power = power * 10;
            }

            if (sign)
            {
                immv = -immv;
            }
            // converting imm value to its numerical value if its valid

            for (int i = 0; i < 7; i++)
            {
                if (iftable[i].inst == m)
                { // if operators are srai ,slli and srli their limit will confined
                    if (iftable[i].f7 == 1)
                    {
                        if (immv >= 0 && immv < 64)
                        {
                        }

                        else
                        {
                            cout << "immediate value out of bound in line " << line << endl;
                            return 0;
                        }
                    }

                    else
                    {
                        if (immv >= -4096 && immv <= 4094)
                        {
                        }

                        else
                        {
                            cout << "immediate value out of bound in line " << line << endl;
                            return 0;
                        }
                    }
                }
            }

            file2 << iformat(data, rdi, rs1i, immv) << endl;
            line++;
        }

        else if (ifilf(m)) // For load instructions.
        {
            ilftable[0] = s3("lb", 0, 0);
            ilftable[1] = s3("lh", 0x1, 0);
            ilftable[2] = s3("lw", 0x2, 0);
            ilftable[3] = s3("ld", 0x3, 0); // stroes this data to ilf table array
            ilftable[4] = s3("lbu", 0x4, 1);
            ilftable[5] = s3("lhu", 0x5, 1);
            ilftable[6] = s3("lwu", 0x6, 1);

            s3 data("0", 0, 0);

            int k = 0;
            string rd, rs1, imm;
            int rdi, rs1i; // register values will get stored here

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ' ')
                {
                    k++;
                    break;
                }
            }

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ',')
                {
                    k += 2;
                    break;
                }

                rd.push_back(ins[k]);
            }

            for (; k < ins.size(); k++)
            {
                if (ins[k] == '(')
                {
                    k++;
                    break;
                }

                imm.push_back(ins[k]);
            }

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ')')
                {
                    k++;
                    break;
                }

                rs1.push_back(ins[k]);
            }

            int e = 0;

            for (int i = 0; i < 65; i++)
            {
                if (rs1 == aliastable[i].a)
                {
                    rs1i = aliastable[i].b;
                    e += 7;
                }

                if (rd == aliastable[i].a)
                {
                    rdi = aliastable[i].b;
                    e += 55;
                }
            }

            if (e != 62) // verifies the correctness of registers
            {
                cout << "error in register of line " << line << "\n";
                return 0;
            }

            for (int i = 0; i < 7; i++)
            {
                if (ilftable[i].inst == m)
                {
                    data = ilftable[i];
                    break;
                }
            }

            int sign = 0;

            if (imm[0] == '-')
            {
                sign = 1;
            }

            int immv = 0;
            int power = 1;

            for (int i = imm.size() - 1; i >= sign; i--)
            {
                if (imm[i] > '9' || imm[i] < '0')
                {
                    cout << "immediate value is incorrect in line" << line << endl;
                    return 0;
                }

                immv += (imm[i] - '0') * power;
                power = power * 10;
            }

            if (sign)
            {
                immv = -immv;
            }

            if (!(immv >= -2048 && immv <= 2047))
            {
                cout << "immediate value out of bound line " << line << endl;
                return 0;
            }
            // converts immediate string to its integer value
            file2 << ilformat(data, rdi, rs1i, immv) << endl;

            line++;
        }

        else if (ifijf(m)) // For jalr
        {
            ijftable[0] = s3("jalr", 0x0, 0);
            s3 data("0", 0, 0);

            data = ijftable[0];
            int k = 0;
            string rd, rs1, imm;
            int rdi, rs1i;

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ' ')
                {
                    k++;
                    break;
                }
            }

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ',')
                {
                    k += 2;
                    break;
                }

                rd.push_back(ins[k]);
            }

            for (; k < ins.size(); k++)
            {
                if (ins[k] == '(')
                {
                    k += 1;
                    break;
                }

                imm.push_back(ins[k]);
            }

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ')')
                {
                    k += 1;
                    break;
                }
                rs1.push_back(ins[k]);
            }

            int e = 0;

            for (int i = 0; i < 65; i++)
            {
                if (rs1 == aliastable[i].a)
                {
                    rs1i = aliastable[i].b;
                    e += 7;
                }

                if (rd == aliastable[i].a)
                {
                    rdi = aliastable[i].b;
                    e += 55;
                }
            }

            // cout << e;

            if (e != 62)
            {
                cout << "error in registers of line " << line << "\n";
                return 0;
            }

            int sign = 0;

            if (imm[0] == '-')
            {
                sign = 1;
            }

            int immv = 0;
            int power = 1;

            for (int i = imm.size() - 1; i >= sign; i--)
            {
                if (imm[i] > '9' || imm[i] < '0')
                {
                    cout << "immediate value is incorrect in line" << line << endl;
                    return 0;
                }

                immv += (imm[i] - '0') * power;
                power = power * 10;
            }

            if (sign)
            {
                immv = -immv;
                // cout << immv;
            }

            if (!(immv >= -2048 && immv <= 2047))
            {
                cout << "immediate value out of bound in line " << line << endl;
                return 0;
            }

            file2 << ijformat(data, rdi, rs1i, immv) << endl;
            line++;
        }

        else if (ifsf(m))
        {
            sftable[0] = s3("sb", 0x0, 0);
            sftable[1] = s3("sh", 0x1, 0);
            sftable[2] = s3("sw", 0x2, 0); // stores the this data to sftable array
            sftable[3] = s3("sd", 0x3, 0);
            s3 data("0", 0, 0);

            int k = 0;
            string rd, rs1, imm;
            int rdi, rs1i;

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ' ')
                {
                    k++;
                    break;
                }
            }

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ',')
                {
                    k += 2;
                    break;
                }

                rd.push_back(ins[k]);
            }

            for (; k < ins.size(); k++)
            {
                if (ins[k] == '(')
                {
                    k++;
                    break;
                }

                imm.push_back(ins[k]);
            }

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ')')
                {
                    k++;
                    break;
                }

                rs1.push_back(ins[k]);
            }

            int e = 0;

            for (int i = 0; i < 65; i++)
            {
                if (rs1 == aliastable[i].a)
                {
                    rs1i = aliastable[i].b;
                    e += 7;
                }

                if (rd == aliastable[i].a)
                {
                    rdi = aliastable[i].b;
                    e += 55;
                }
            }

            if (e != 62) // checks the correctness of registers
            {
                cout << "error in registers of line " << line << "\n";

                return 0;
            }

            for (int i = 0; i < 4; i++)
            {
                if (sftable[i].inst == m)
                {
                    data = sftable[i];
                    break;
                }
            }

            int sign = 0;

            if (imm[0] == '-')
            {
                sign = 1;
            }

            int immv = 0;
            int power = 1;

            for (int i = imm.size() - 1; i >= sign; i--)
            {
                if (imm[i] > '9' || imm[i] < '0')
                {
                    cout << "immediate value is incorrect in line" << line << endl;
                    return 0;
                }

                immv += (imm[i] - '0') * power;
                power = power * 10;
            }

            if (sign)
            {
                immv = -immv;
            }

            if (!(immv >= -2048 && immv <= 2047))
            {
                cout << "immediate value out of bound line " << line << endl;
                return 0;
            }
            // will handle errors in immediate values if any and if correct immediate then convert it to integer if correct input given
            file2 << sformat(data, rdi, rs1i, immv) << endl;
            line++;
        }

        else if (ifbf(m))
        {
            bftable[0] = s3("beq", 0x0, 0);
            bftable[1] = s3("bne", 0x1, 0);
            bftable[2] = s3("blt", 0x4, 0);
            bftable[3] = s3("bge", 0x5, 0); // loads data to bf table if branch statement
            bftable[4] = s3("bltu", 0x6, 0);
            bftable[5] = s3("bgeu", 0x7, 0);
            s3 data("0", 0, 0);

            int k = 0;
            string rd, rs1, imm;
            int rdi, rs1i;

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ' ')
                {
                    k++;
                    break;
                }
            }

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ',')
                {
                    k += 2;
                    break;
                }

                rd.push_back(ins[k]);
            }

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ',')
                {
                    k += 2;
                    break;
                }

                rs1.push_back(ins[k]);
            }

            for (; k < ins.size(); k++)
            {

                imm.push_back(ins[k]);
            }

            int e = 0;

            for (int i = 0; i < 65; i++)
            {
                if (rs1 == aliastable[i].a)
                {
                    rs1i = aliastable[i].b;
                    e += 7;
                }

                if (rd == aliastable[i].a)
                {
                    rdi = aliastable[i].b;
                    e += 55;
                }
            }

            if (e != 62) // whether all registers are correct are not
            {
                cout << "error in registers of line " << line << "\n";
                return 0;
            }

            for (int i = 0; i < 7; i++)
            {
                if (bftable[i].inst == m)
                {
                    data = bftable[i]; // extracts data from bftable
                    break;
                }
            }

            auto it = ll.find(imm);

            if (it == ll.end()) // if that label is not present then  it will be considered a immediate value
            {
                int sign = 0;

                if (imm[0] == '-')
                {
                    sign = 1;
                }

                int immv = 0;
                int power = 1;

                for (int i = imm.size() - 1; i >= sign; i--)
                {
                    if (imm[i] > '9' || imm[i] < '0')
                    {
                        cout << "immediate value is incorrect in line" << line << endl;
                        return 0;
                    }

                    immv += (imm[i] - '0') * power;
                    power = power * 10;
                } // if the immediate value doesn't satisfy the decimal notation error occurs

                if (sign)
                {
                    immv = -immv;
                }

                if (!(immv >= -4096 && immv < 4096))
                {
                    cout << "immediate value out of bound in line " << line << endl;
                    return 0;
                }

                file2 << bformat(data, rdi, rs1i, immv) << endl;
            }

            else
            {
                int linenum = ll[imm];
                int immv = linenum * 4 - line * 4;

                if (!(immv >= -4096 && immv < 4096))
                {
                    cout << "immediate value out of bound in line " << line << endl;
                    return 0;
                }

                file2 << bformat(data, rdi, rs1i, immv) << endl;
            }
            line++;
        }

        else if (ifuf(m))
        {
            string opcode = "0110111"; // opcode hardcoded

            for (int i = 0; i < 7; i++)
            {
                opcode[i] -= '0';
            }

            string imm;
            int k = 0;
            int co = 0;

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ' ')
                {
                    k++;
                    break;
                }
            }

            string r;

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ',')
                {
                    k += 2;
                    break;
                }

                r.push_back(ins[k]);
            }

            int r1 = 0;
            // cout << r;

            for (int i = 0; i < 65; i++)
            {
                if (aliastable[i].a == r)
                {
                    r1 = aliastable[i].b;
                }
            }

            // cout << r1;

            for (; k < ins.size(); k++)
            {
                imm.push_back(ins[k]);
            }

            string r2(5, '0');

            for (int i = 0; i < 5; i++)
            {
                r2[4 - i] = r1 & 1;
                r1 = r1 >> 1;
            }

            int immv = 0;
            if (imm[0] == '0' && imm[1] == 'x') // if hexa decimal notation given
            {
                int power = 1;

                for (int i = imm.size() - 1; i >= 2; i--)
                {
                    if (!((imm[i] >= '0' && imm[i] <= '9') || (imm[i] >= 'a') && imm[i] <= 'f'))
                    {
                        cout << "immediate value is incorrect in line" << line << endl;
                        return 0;
                    }
                    if ((imm[i] >= '0' && imm[i] <= '9'))
                        immv += (imm[i] - '0') * power;
                    else
                        immv += (imm[i] - 'a' + 10) * power;
                    power = power * 16;
                }
            }

            else
            { // decimal taken
                if (imm[0] == '-')
                {
                    cout << "immeddiate out of bound in line " << line << endl;
                    return 0;
                }

                int power = 1;

                for (int i = imm.size() - 1; i >= 0; i--)
                {
                    if (imm[i] > '9' || imm[i] < '0')
                    {
                        cout << "immediate value is incorrect in line" << line << endl;
                        return 0;
                    }

                    immv += (imm[i] - '0') * power;
                    power = power * 10;
                }
            }
            //  cout << immv;

            if (immv >= (1 << 20))
            {
                cout << "immediate value out of bound in line " << line << endl;
            }

            string x = decimaltobinary(immv, 20);
            string a = x + r2 + opcode;
            string h(8, '0');

            for (int i = 0; i < 8; i++)
            {
                h[i] = tohexa(&(a[0]) + 4 * i);
            }

            file2 << h << endl;
            line++;
        }

        else if (ifujf(m))
        {
            string opcode = "1101111"; // opcode hardcoded

            for (int i = 0; i < 7; i++)
            {
                opcode[i] -= '0';
            }

            string imm;
            int k = 0;

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ' ')
                {
                    k++;
                    break;
                }
            }

            string r;

            for (; k < ins.size(); k++)
            {
                if (ins[k] == ',')
                {
                    k += 2;
                    break;
                }

                r.push_back(ins[k]);
            }

            int r1 = 0;

            for (int i = 0; i < 65; i++)
            {
                if (aliastable[i].a == r)
                {
                    r1 = aliastable[i].b;
                }
            }

            for (; k < ins.size(); k++)
            {
                imm.push_back(ins[k]);
            }

            auto it = ll.find(imm);
            int immv = 0;

            if (it == ll.end()) // if label not found in the map data structure ll will be considered as number and if it doesn't have properties of number then it shows errors
            {
                int sign = 0;

                if (imm[0] == '-')
                {
                    sign = 1;
                }

                int power = 1;

                for (int i = imm.size() - 1; i >= sign; i--)
                {
                    if (imm[i] > '9' || imm[i] < '0')
                    {
                        cout << "immediate value is incorrect in line" << line << endl;
                        return 0;
                    }

                    immv += (imm[i] - '0') * power;
                    power = power * 10;
                }

                if (sign)
                {
                    immv = -immv;
                }

                if (!(immv >= 0 && immv < (1 << 20)))
                {
                    cout << "immediate value out of bound in line " << line << endl;
                    return 0;
                }
            }

            else
            {
                int linenum = ll[imm];
                immv = linenum * 4 - line * 4;

                if (!(immv > (-(1 << 20)) && immv < (1 << 20)))
                {
                    cout << "immediate value out of bound in line " << line << endl;
                    return 0;
                }
            }

            string x = decimaltobinary(immv, 21);
            string rd = decimaltobinary(r1, 5);
            string x1(10, '\0');
            string x2(8, '\0');

            for (int i = 0; i < 10; i++)
            {
                x1[i] = x[10 + i];
            }

            for (int i = 0; i < 8; i++)
            {
                x2[i] = x[1 + i];
            }

            string bi = x[0] + x1 + x[9] + x2;
            string bin = bi + rd + opcode;

            string h(8, '0');

            for (int i = 0; i < 8; i++)
            {
                h[i] = tohexa(&(bin[0]) + 4 * i);
            }

            file2 << h << endl;
            line++;
        }

        else
        {
            cout << "error in line " << line << "\n";
            return 0;
            line++;
        }
    }
}
