#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <stack>
#include <cstdlib>
#include <ctime>

using namespace std;

map<string, int> label_line; // Map that links labels and lines in given assembly code for quick jumps.
map<string, int> aliastable; // Map that links regiters(in both standard and alias names) with integers 0 to 31.
string file_name;
vector<string> v;        // Vector that stores lines of given RISC-V assembly code.
vector<int> breakpoints; // vector that stores line numbers where breakpoints are set.
int cache_enable = 0;
int program_counter = 0;
int dot_count = 0; // Variable that counts number of dots like .data .dword .text ......
int program_completion = 0;
long long int x[32] = {0};        // Array of registers x0 to x31 that stores memory address.
unsigned char mem[0x40000] = {0}; // Array of memory.

struct s_int // Defining struct s_int that has "string" & "long long int"
{
    string a;
    long long int line;
};
stack<s_int> v_si; // Create a stack of data type struct s_int

int log(long long int x)
{
    int i = 1;
    int c = 0;
    while (i != x)
    {
        i = i * 2;
        c++;
    }
    return c;
}
struct block
{
    long long int tag;
    char *m;
    int valid_bit = 0;
    int dirty_bit = 0;
    int rank = 0;
};
class cache
{
public:
    int capacity;
    int block_size;
    int assoc;
    int r_policy;
    int w_policy;
    block **p;
    int accesses;
    int hits;
    int in;

    cache(int x1, int x2, int x3, int x4, int x5)
    {
        if (x3 != 0)
        {
            capacity = x1;
            ;
            block_size = x2; // in bytes
            assoc = x3;
            r_policy = x4;
            w_policy = x5;
            in = x1 / (x3 * x2);
            p = new block *[in];
            for (int i = 0; i < in; i++)
            {
                p[i] = new block[x3];
            }
            for (int i = 0; i < in; i++)
            {
                for (int j = 0; j < x3; j++)
                {
                    p[i][j].m = new char[x2];
                    p[i][j].valid_bit = 0;
                }
            }
        }
        else
        {
            capacity = x1;
            block_size = x2; // in bytes
            assoc = x1 / x2;
            r_policy = x4;
            w_policy = x5;
            in = 1;
            p = new block *[in];
            for (int i = 0; i < in; i++)
            {
                p[i] = new block[x3];
            }
            for (int i = 0; i < in; i++)
            {
                for (int j = 0; j < assoc; j++)
                {
                    p[i][j].m = new char[x2];
                    p[i][j].valid_bit = 0;
                }
            }
        }
    }
    void c_read(long long memo, long long &u_data, int size)
    { // we'll deal with sign later in load store

        long long mem_tag = (memo >> log(block_size)) >> log(in);
        int index = (memo >> log(block_size)) & (~(-1 << log(in)));
        int start = memo & (~(-1 << log(block_size)));
        int stop = start + size - 1;
        if (stop >= block_size)
        {
            cout << "data split over two blocks\n";
            return;
        }
        int hit = 0;
        accesses++;
        u_data = 0;
        block tt;
        if (r_policy == 0)
        {

            for (int i = 0; i < assoc; i++)
            {
                if (p[index][i].tag == mem_tag && p[index][i].valid_bit)
                {
                    hits++;
                    hit = 1;
                    tt = p[index][i];
                    int count = 0;
                    for (int t = start; t <= stop; t++)
                    {
                        u_data += (p[index][i].m[t] << (8 * count));
                        count++;
                    }
                }
            }
            if (!hit)
            {
                // search for any invalid block in particular index
                int invp = -1;
                for (int i = 0; i < assoc; i++)
                {
                    if (p[index][i].valid_bit == 0)
                    {
                        invp = i;
                        break;
                    }
                }
                if (invp != -1)
                {
                    p[index][invp].valid_bit = 1;
                    for (int i = 0; i < block_size; i++)
                    {
                        p[index][invp].m[i] = mem[(mem_tag << (log(in)) << (log(block_size))) + index * block_size + i - 0x10000];
                    }
                    p[index][invp].tag = mem_tag;

                    for (int i = 0; i < size; i++)
                    {
                        u_data += (mem[memo + i - 0x10000] << (8 * i)); // write allocate is always being used
                    }
                    p[index][invp].dirty_bit = 0; // we can deal irrespective of rank
                }
                else
                { // replace
                    long long ta = (memo >> log(in)) >> log(block_size);
                    int tbri = rand() % assoc; // to be replaced index
                    block xy = p[index][tbri];
                    // dirty bit replacement
                    if (xy.dirty_bit)
                    {
                        for (int i = 0; i < block_size; i++)
                        {
                            mem[xy.tag << (log(in)) << (log(block_size)) + index * block_size + i - 0x10000] = xy.m[i];
                        }
                    }
                    for (int i = 0; i < block_size; i++)
                    {
                        p[index][tbri].m[i] = mem[(mem_tag << (log(in)) << (log(block_size))) + index * block_size + i - 0x10000];
                    }
                    p[index][tbri].tag = (memo >> log(in)) >> log(block_size);
                    ;
                    p[index][tbri].dirty_bit = 0;

                    for (int i = 0; i < size; i++)
                    {
                        u_data += (mem[memo + i - 0x10000] << (8 * i));
                    }
                }
            }
        }
        else if (r_policy == 1)
        {
            for (int i = 0; i < assoc; i++)
            {
                if (p[index][i].tag == mem_tag && p[index][i].valid_bit)
                {
                    hits++;
                    hit = 1;
                    tt = p[index][i];
                    int count = 0;
                    for (int t = start; t <= stop; t++)
                    {
                        u_data += (p[index][i].m[t] << (8 * count));
                        count++;
                    }
                }
            }
            if (!hit)
            {
                // search for any invalid block in particular index
                int invp = -1;
                int nv = 0;
                for (int i = 0; i < assoc; i++)
                {
                    nv += (p[index][i].valid_bit);
                }
                for (int i = 0; i < assoc; i++)
                {
                    if (p[index][i].valid_bit == 0)
                    {
                        invp = i;

                        break;
                    }
                }
                if (invp != -1)
                {
                    p[index][invp].valid_bit = 1;
                    p[index][invp].rank = nv;
                    for (int i = 0; i < block_size; i++)
                    {
                        p[index][invp].m[i] = mem[(mem_tag << (log(in)) << (log(block_size))) + index * block_size + i - 0x10000];
                    }
                    p[index][invp].dirty_bit = 0;
                    p[index][invp].tag = (memo >> log(in)) >> log(block_size);
                    ;
                    u_data = 0;
                    for (int i = 0; i < size; i++)
                    {
                        u_data += (mem[memo + i - 0x10000] << (8 * i)); // write allocate is always being used
                    }
                }
                else
                { // replace
                    long long ta = (memo >> log(in)) >> log(block_size);
                    int tbri; // to be replaced index
                    for (int i = 0; i < assoc; i++)
                    {
                        if (p[index][i].rank == 0)
                        {
                            tbri = i;
                        }
                    }
                    block xy = p[index][tbri];
                    // dirty bit replacement
                    if (xy.dirty_bit)
                    {
                        for (int i = 0; i < block_size; i++)
                        {
                            mem[xy.tag << (log(in)) << (log(block_size)) + index * block_size + i - 0x10000] = xy.m[i];
                        }
                    }
                    for (int i = 0; i < assoc; i++)
                    {
                        if (p[index][i].rank != 0)
                        {
                            p[index][i].rank--;
                        }
                    }
                    for (int i = 0; i < block_size; i++)
                    {
                        p[index][tbri].m[i] = mem[(mem_tag << (log(in)) << (log(block_size))) + index * block_size + i - 0x10000];
                    }
                    p[index][tbri].tag = (memo >> log(in)) >> log(block_size);
                    ;
                    p[index][tbri].rank = assoc - 1;
                    u_data = 0;
                    for (int i = 0; i < size; i++)
                    {
                        u_data += (mem[memo + i - 0x10000] << (8 * i));
                    }
                }
            }
        }
        else if (r_policy == 2)
        {
            int hi;
            int hir;
            for (hi = 0; hi < assoc; hi++)
            {
                if (p[index][hi].tag == mem_tag && p[index][hi].valid_bit)
                {
                    hits++;
                    hit = 1;
                    tt = p[index][hi];
                    int count = 0;
                    hir = p[index][hi].rank;
                    for (int t = start; t <= stop; t++)
                    {
                        u_data += (p[index][hi].m[t] << 8 * (count));
                        count++;
                    }
                    break;
                }
            }
            if (hit)
            {
                int nv = 0;
                for (int i = 0; i < assoc; i++)
                {
                    nv += p[index][i].valid_bit;
                }
                for (int i = 0; i < assoc; i++)
                {
                    if (p[index][i].valid_bit && p[index][i].rank > hir)
                    {
                        p[index][i].rank--;
                    }
                }
                p[index][hi].rank = nv - 1;
            }
            if (!hit)
            {
                // search for any invalid block in particular index
                int nv = 0;
                for (int i = 0; i < assoc; i++)
                {
                    nv += (p[index][i].valid_bit);
                }
                int invp = -1;
                for (int i = 0; i < assoc; i++)
                {
                    if (p[index][i].valid_bit == 0)
                    {
                        invp = i;

                        break;
                    }
                }
                if (invp != -1)
                {
                    p[index][invp].valid_bit = 1;
                    p[index][invp].rank = nv;
                    for (int i = 0; i < block_size; i++)
                    {
                        p[index][invp].m[i] = mem[(mem_tag << (log(in)) << (log(block_size))) + index * block_size + i - 0x10000];
                    }
                    p[index][invp].tag = (memo >> log(in)) >> log(block_size);
                    p[index][invp].dirty_bit = 0;

                    for (int i = 0; i < size; i++)
                    {
                        u_data += (mem[memo + i - 0x10000] << (8 * i)); // write allocate is always being used
                    }
                }
                else
                { // replace
                    long long ta = (memo >> log(in)) >> log(block_size);
                    int tbri; // to be replaced index
                    for (int i = 0; i < assoc; i++)
                    {
                        if (p[index][i].rank == 0)
                        {
                            tbri = i;
                        }
                    }
                    block xy = p[index][tbri];
                    // dirty bit replacement
                    if (xy.dirty_bit)
                    {
                        for (int i = 0; i < block_size; i++)
                        {
                            mem[((xy.tag << (log(in))) << (log(block_size))) + index * block_size + i - 0x10000] = xy.m[i];
                        }
                    }
                    for (int i = 0; i < assoc; i++)
                    {
                        if (p[index][i].rank != 0)
                        {
                            p[index][i].rank--;
                        }
                    }
                    for (int i = 0; i < block_size; i++)
                    {
                        p[index][tbri].m[i] = mem[(mem_tag << (log(in)) << (log(block_size))) + index * block_size + i - 0x10000];
                    }
                    p[index][tbri].tag = (memo >> log(in)) >> log(block_size);
                    ;
                    p[index][tbri].rank = assoc - 1;
                    u_data = 0;
                    for (int i = 0; i < size; i++)
                    {
                        u_data += (mem[memo + i - 0x10000] << (8 * i));
                    }
                }
            }
        }
        std::ofstream file(file_name, std::ios::app); // Open in append mode
        file << "R: Address:0x" << hex << memo << ", Set: 0x" << index << ",";
        if (hit)
            file << " Hit";
        else
            file << " Miss";
        file << ", Tag: 0x" << mem_tag;
        if (!hit)
            file << ", Clean\n";
        else
        {
            if (tt.dirty_bit)
                file << ", Dirty\n";
            else
                file << ", Clean\n";
        }
    }
    void c_write(long long memo, long long &u_data, int size)
    {

        long long mem_tag = (memo >> log(block_size)) >> log(in);
        int index = (memo >> log(block_size)) & (~(-1 << log(in)));
        int start = memo & (~(-1 << log(block_size)));
        int stop = start + size - 1;
        if (stop >= block_size)
        {
            cout << "memory split between two blocks\n";
        }
        int hit = 0;
        accesses++;
        if (w_policy == 1)
        { // write through
            for (int i = 0; i < assoc; i++)
            {
                if (p[index][i].tag == mem_tag && p[index][i].valid_bit)
                {
                    hits++;
                    hit = 1;
                    int count = 0;
                    long long split = 255;
                    for (int t = start; t <= stop; t++)
                    {
                        p[index][i].m[t] = (split) & (u_data >> (8 * count));
                        count++;
                    }
                }
            }

            for (int i = 0; i < size; i++)
            {
                long long split = 255; // writing to mem also
                mem[memo + i] = (split) & (u_data >> (8 * i));
            }
        }
        if (w_policy == 2)
        {
            int hi;
            int hir; // write back
            for (hi = 0; hi < assoc; hi++)
            {
                if (p[index][hi].tag == mem_tag && p[index][hi].valid_bit)
                {
                    hits++;
                    hit = 1;
                    int count = 0;
                    long long split = 255;
                    hir = p[index][hi].rank;
                    for (int t = start; t <= stop; t++)
                    {
                        p[index][hi].m[t] = (split) & (u_data >> (8 * count));
                        count++;
                    }

                    p[index][hi].dirty_bit = 1;
                    break;
                }
            }
            if (hit)
            {
                if (r_policy == 2)
                {
                    int nv = 0;
                    for (int i = 0; i < assoc; i++)
                    {
                        nv += p[index][i].valid_bit;
                    }
                    for (int i = 0; i < assoc; i++)
                    {
                        if (p[index][i].valid_bit && p[index][i].rank > hir)
                        {
                            p[index][i].rank--;
                        }
                    }
                    p[index][hi].rank = nv - 1;
                }
            }
            if (!hit)
            {
                int invp = -1;
                for (int i = 0; i < assoc; i++)
                {
                    if (p[index][i].valid_bit == 0)
                    {
                        invp = i;
                        break;
                    }
                }
                if (invp != -1)
                { // we replace invlid block first
                    int count = 0;
                    long long split = 255; // 0x5555555cca60
                    for (int i = 0; i < block_size; i++)
                    { // 0x555500000000 <error: Cannot access memory at address 0x555500000000>
                        long long dd = (mem_tag << log(in) << (log(block_size))) + index * block_size + i - 0x10000;
                        p[index][invp].m[i] = mem[dd];
                    }
                    for (int t = start; t <= stop; t++)
                    {
                        p[index][invp].m[t] = (split) & (u_data >> (8 * count));
                        count++;
                    }
                    p[index][invp].tag = mem_tag;
                    p[index][invp].dirty_bit = 1;
                    int nv = 0;
                    for (int i = 0; i < assoc; i++)
                    {
                        nv += p[index][i].valid_bit;
                    }
                    p[index][invp].valid_bit = 1;
                    p[index][invp].rank = nv;
                }
                else
                {
                    if (r_policy != 0)
                    {
                        for (int tbri = 0; tbri < assoc; tbri++)
                        {

                            if (p[index][tbri].rank == 0 && p[index][tbri].valid_bit)
                            {
                                block xy = p[index][tbri];
                                // dirty bit  helped replacement
                                if (xy.dirty_bit)
                                {
                                    for (int i = 0; i < block_size; i++)
                                    {
                                        char t = xy.m[i];
                                        long long dd = (xy.tag << (log(in)) << (log(block_size))) + index * block_size + i - 0x10000;
                                        mem[dd] = t;
                                    }
                                }
                                int count = 0;
                                long long split = 255;
                                for (int i = 0; i < block_size; i++)
                                {
                                    p[index][tbri].m[i] = mem[(mem_tag << log(in) << (log(block_size))) + index * block_size + i - 0x10000];
                                }
                                for (int t = start; t <= stop; t++)
                                {
                                    p[index][tbri].m[t] = (split) & (u_data >> (8 * count));
                                    count++;
                                }
                                for (int i = 0; i < assoc; i++)
                                {
                                    if (i != tbri)
                                    {
                                        p[index][tbri].rank--;
                                    }
                                }
                                p[index][tbri].rank = assoc - 1;
                                p[index][tbri].tag = mem_tag;
                                p[index][tbri].dirty_bit = 1;
                                p[index][tbri].valid_bit = 1;
                                break;
                            }
                        }
                    }
                    else if (r_policy == 0)
                    {
                        int tbri = rand() % assoc;
                        block xy = p[index][tbri];

                        // dirty bit replacement
                        if (xy.dirty_bit)
                        {
                            for (int i = 0; i < block_size; i++)
                            {
                                mem[xy.tag << (log(in)) << (log(block_size)) + index * block_size + i - 0x10000] = xy.m[i];
                            }
                        }
                        int count = 0;
                        long long split = 255;
                        for (int i = 0; i < block_size; i++)
                        {
                            p[index][tbri].m[i] = mem[(mem_tag << log(in) << (log(block_size))) + index * block_size + i - 0x10000];
                        }
                        for (int t = start; t <= stop; t++)
                        {
                            p[index][tbri].m[t] = (split) & (u_data >> (8 * count));
                            count++;
                        }
                        p[index][tbri].tag = (mem_tag);
                        p[index][tbri].dirty_bit = 1;
                        p[index][tbri].valid_bit = 1;
                    }
                }
            }
        }
        std::ofstream file(file_name, std::ios::app); // Open in append mode
        file << "W: Address:0x" << hex << memo << ", Set: 0x" << index;
        if (hit)
            file << ", Hit";
        else
            file << ", Miss";
        file << ", Tag: 0x" << mem_tag;
        if ((w_policy == 2))
        {
            file << ", Dirty\n";
        }
        else
        {
            file << ", Clean\n";
        }
    }
    void print()
    {
        cout << "acc=" << accesses << endl
             << "hits=" << hits << endl;
        for (int i = 0; i < in; i++)
        {
            for (int j = 0; j < assoc; j++)
            {

                cout << i << " ) v: " << p[i][j].valid_bit;
                cout << " db: " << p[i][j].dirty_bit;
                cout << " rank: " << p[i][j].rank;
                cout << " tag: " << p[i][j].tag << " data ";
                for (int k = 0; k < block_size; k++)
                {
                    cout << (int)p[i][j].m[k] << " ";
                }
                cout << endl;
            }
        }
    }
};
cache *cp;
string hexa(long long int x) // Function that converts integer into hexadecimal strings.
{
    string a(16, '\0'); // 64 bits in 16 hexadecimal
    for (int i = 15; i >= 0; i--)
    {
        int b[4] = {0}; // 4 bits that hold hexadecimal digit
        for (int j = 3; j >= 0; j--)
        {
            b[j] = x & 1; // Extract the least significant bit and send it to b[3] and right shift x
            x = x >> 1;   // and then repeat the process, so finally every bit goes into  16 hexadecimal number or string (64 bits)
        }
        int x = 8 * b[0] + 4 * b[1] + 2 * b[2] + 1 * b[3]; // Convert 4 binary digits into single number from 0 to 15
        if (x < 10)
        {
            a[i] = '0' + x; // Converting the above single number into hexaadecimal digit
        }
        else
        {
            a[i] = 'A' + x - 10;
        }
    }
    return a;
}

string char_to_hexa(char x) // Converts 2 characters long (number) into hexadecimal
{
    string a(2, '\0');
    for (int i = 1; i >= 0; i--)
    {
        int b[4] = {0};
        for (int j = 3; j >= 0; j--)
        {
            b[j] = x & 1; //
            x = x >> 1;
        }
        int x = 8 * b[0] + 4 * b[1] + 2 * b[2] + 1 * b[3];
        if (x < 10)
        {
            a[i] = '0' + x;
        }
        else
        {
            a[i] = 'A' + x - 10;
        }
    }
    return a;
}

int ifrf(string s) // Function that selects the R-format instruction to check which the instruction in the input file.
{
    if (s == "add" || s == "sub" || s == "and" || s == "or" || s == "xor" || s == "sll" || s == "srl" || s == "sra")
        return 1;
    else
        return 0;
}

int ifif(string s) // Function that selects the I-format instruction to check which the instruction in the input file.
{
    if (s == "addi" || s == "andi" || s == "ori" || s == "xori" || s == "slli" || s == "srli" || s == "srai")
        return 1;
    else
        return 0;
}

int iflf(string s) // Function that selects the I-format(Load type) instruction to check which the instruction in the input file.
{
    if (s == "ld" || s == "lw" || s == "lh" || s == "lb" || s == "lwu" || s == "lhu" || s == "lbu")
        return 1;
    else
        return 0;
}

int ifsf(string s) // Function that selects the S-format instruction to check which the instruction in the input file.
{
    if (s == "sd" || s == "sw" || s == "sh" || s == "sb")
        return 1;
    else
        return 0;
}

void execute(string a) // Function that executes the given RISC-V instruction in string a.
{
    istringstream iss1(a); // To separate spaces and parse the individual components  in string 'a' and store in iss1.
    string ins, rd, rs1, rs2;
    iss1 >> ins; // Extract ins(i.e., add sub and ...) from iss1.
    long long int epc = program_counter + 1;
    if (!v_si.empty())
        v_si.top().line = epc;

    if (ifrf(ins)) // Check for R-format
    {
        iss1 >> rd;     // Extract rd from iss1
        rd.pop_back();  // Removes comma',' from rd.
        iss1 >> rs1;    // Extract rs1 from iss1
        rs1.pop_back(); // Removes comma',' from rs1.
        iss1 >> rs2;    // Extract rs2 from iss1
        int rdi = aliastable[rd];
        int rs1i = aliastable[rs1]; // Map the corresponding integer values of rd,rs1,rs2(Linked in aliastable) into rdi,rs1i,rs2i variables
        int rs2i = aliastable[rs2];

        if (ins == "add")
        {
            x[rdi] = x[rs1i] + x[rs2i]; // Add values of rs1 rs2 and store in rd.
        }
        else if (ins == "sub")
        {
            x[rdi] = x[rs1i] - x[rs2i]; // Sub values of rs1 rs2 and store in rd.
        }
        else if (ins == "and")
        {
            x[rdi] = x[rs1i] & x[rs2i]; // Performs bitwise AND of rs1 rs2, stores in rd.
        }
        else if (ins == "or")
        {
            x[rdi] = x[rs1i] | x[rs2i]; // Performs bitwise OR of rs1 rs2, stores in rd
        }
        else if (ins == "xor")
        {
            x[rdi] = x[rs1i] ^ x[rs2i]; // Performs bitwise XOR of rs1 rs2, stores in rd
        }
        else if (ins == "sll") // Performs logical shift left, also checks for the range, if out of range throws and error.
        {
            if (x[rs2i] > 63 || x[rs2i] < 0)
            {
                cout << "error\n";
                return;
            }
            x[rdi] = x[rs1i] << x[rs2i]; // Shift rs1i left by rs2i bits store in rdi.
        }
        else if (ins == "srl") // Performs logical shift right, also checks for the range, if out of range throws and error.
        {
            if (x[rs2i] > 63 || x[rs2i] < 0)
            {
                cout << "error\n";
                program_counter++; // Since this return from the function PC should be updated.
                return;
            }
            if (x[rs1i] >= 0 || x[rs2i] == 0)
                x[rdi] = x[rs1i] >> x[rs2i]; // If rs1i is positive then normal shift
            else                             // Otherwise
            {
                x[rdi] = x[rs1i] >> x[rs2i]; // Shift right by rs2i bits, in C++ it puts 1's instead of 0's for negative numbers.
                long long int t = 1LL << (64 - x[rs2i]);
                t -= 1;              // So basically here finally t contains  rs2i no of 0's at the start and remaining 1's.
                x[rdi] = x[rdi] & t; // Perform AND to get srl.
            }
        }
        else if (ins == "sra")
        {
            if (x[rs2i] > 63 || x[rs2i] < 0) // Check for range.
            {
                cout << "error\n";
                program_counter++;
                return;
            }
            x[rdi] = x[rs1i] >> x[rs2i]; // Every thing is according to sra in C++ shifts.
        }

        x[0] = 0;             // x0 should always be = 0.
        program_counter += 1; // Since 1 step is executed increase PC by 1.
    }

    // For Branch instructions.
    else if (ins == "bne")
    {
        iss1 >> rs1; // These steps are similar to in R-format
        rs1.pop_back();
        iss1 >> rs2;
        rs2.pop_back();
        int rs1i = aliastable[rs1];
        int rs2i = aliastable[rs2];
        string label;
        iss1 >> label; // Extarct label from instruction for branch target.
        if (x[rs1i] != x[rs2i])
            program_counter = label_line[label]; // Check not equal condition and change the PC to label_line[label]
        else
            program_counter++;
    }

    else if (ins == "beq")
    {
        iss1 >> rs1;
        rs1.pop_back();
        iss1 >> rs2;
        rs2.pop_back();
        int rs1i = aliastable[rs1];
        int rs2i = aliastable[rs2];
        string label;
        iss1 >> label; // Extarct label from instruction for branch target.
        if (x[rs1i] == x[rs2i])
            program_counter = label_line[label]; // Check equal condition and change the PC to label_line[label]
        else
            program_counter++;
    }

    else if (ins == "blt")
    {
        iss1 >> rs1;
        rs1.pop_back();
        iss1 >> rs2;
        rs2.pop_back();
        int rs1i = aliastable[rs1];
        int rs2i = aliastable[rs2];
        string label;
        iss1 >> label; // Extarct label from instruction for branch target.
        if (x[rs1i] < x[rs2i])
            program_counter = label_line[label]; // Check less than condition and change the PC to label_line[label]
        else
            program_counter++;
    }

    else if (ins == "bltu")
    {
        iss1 >> rs1;
        rs1.pop_back();
        iss1 >> rs2;
        rs2.pop_back();
        int rs1i = aliastable[rs1];
        int rs2i = aliastable[rs2];
        string label;
        iss1 >> label; // Extarct label from instruction for branch target.
        if (static_cast<unsigned int>(x[rs1i]) < static_cast<unsigned int>(x[rs2i]))
        {
            program_counter = label_line[label];
        }
        else
            program_counter++;
    }

    else if (ins == "bge")
    {
        iss1 >> rs1;
        rs1.pop_back();
        iss1 >> rs2;
        rs2.pop_back();
        int rs1i = aliastable[rs1];
        int rs2i = aliastable[rs2];
        string label;
        iss1 >> label; // Extarct label from instruction for branch target.
        if (x[rs1i] >= x[rs2i])
            program_counter = label_line[label]; // Check greater than or equal to condition and change the PC to label_line[label]
        else
            program_counter++;
    }

    else if (ins == "bgeu")
    {
        iss1 >> rs1;
        rs1.pop_back();
        iss1 >> rs2;
        rs2.pop_back();
        int rs1i = aliastable[rs1];
        int rs2i = aliastable[rs2];
        string label;
        iss1 >> label;                    // Extarct label from instruction for branch target.
        if (x[rs1i] >= 0 && x[rs2i] >= 0) // Check for cases and implement the condition
        {
            if (x[rs1i] >= x[rs2i])
                program_counter = label_line[label];
            else
                program_counter++;
        }
        else if (x[rs1i] < 0 && x[rs2i] < 0)
        {
            if (x[rs1i] <= x[rs2i])
                program_counter = label_line[label];
            else
                program_counter++;
        }
        else if (x[rs2i] > 0)
            program_counter = label_line[label];
        else
            program_counter++;
    }

    else if (ifsf(ins)) // For S-Format Instructions.
    {
        iss1 >> rs2;
        rs2.pop_back();
        string d; // Temporary string variable that stores immediate and rs1.
        iss1 >> d;
        int i = 0;        // Loop counter in separating string d.
        string imm;       // immediate string variable.
        for (i = 0;; i++) // Loop when we don't know the upper limit
        {
            if (d[i] == '(')
                break; // Separate's imm from "d" and send to imm string
            else
                imm.push_back(d[i]);
        }
        i++;
        for (; i < d.size() - 1; i++)
            rs1.push_back(d[i]); // Last element in d is ')' so d.size() - 1, and push remaining.
        int immi = stoi(imm);    // Convert imm in string format to integer format.
        if (!(immi >= -2048 && immi < 2048))
        {
            program_counter++;
            cout << "Error:immediate out of range\n";
            return;
        }
        int rs1i = aliastable[rs1]; // assign corresponding values of registers with it linked integers in alaistable.
        int rs2i = aliastable[rs2];
        long long int addr = x[rs1i] + immi;      // Calculating address
        if (!(addr >= 0x10000 && addr < 0x50000)) // Here we are
        {
            cout << "Address can't be accessed \n";
            program_counter++;
            return;
        }
        else
        {
            if (cache_enable == 0)
            {
                if (ins == "sd") // For store double word instruction.
                {
                    long long int x1 = addr - 0x10000; // Adjust address to memory map.
                    long long int t = x[rs2i];         // send the value in rs2 to t
                    for (int i = 0; i < 8; i++)        // Loop to store rs2 in mem.
                    {
                        int val = 0;
                        for (int j = 0; j < 8; j++)
                        {
                            val += ((1 << j) * (t & 1)); // Taking each bit and converting into
                            t = t >> 1;                  // Shifts 8 times for 1 time and total 64 times
                        }
                        mem[x1 + i] = val; // Each time it stores 8 bits or 2 hex digits
                    }
                    program_counter++;
                }
                if (ins == "sw")
                {
                    long long int x1 = addr - 0x10000; //
                    long long int t = x[rs2i];
                    for (int i = 0; i < 4; i++)
                    {
                        int val = 0;
                        for (int j = 0; j < 8; j++)
                        {
                            val += ((1 << j) * (t & 1));
                            t = t >> 1;
                        }
                        mem[x1] = val;
                        x1++;
                    }
                    program_counter++;
                }
                if (ins == "sh")
                {
                    long long int x1 = addr - 0x10000;
                    long long int t = x[rs2i];
                    for (int i = 0; i < 2; i++)
                    {
                        int val = 0;
                        for (int j = 0; j < 8; j++)
                        {
                            val += ((1 << j) * (t & 1));
                            t = t >> 1;
                        }
                        mem[x1] = val;
                        x1++;
                    }
                    program_counter++;
                }
                if (ins == "sb")
                {
                    long long int x1 = addr - 0x10000;
                    long long int t = x[rs2i];
                    for (int i = 0; i < 1; i++)
                    {
                        int val = 0;
                        for (int j = 0; j < 8; j++)
                        {
                            val += ((1 << j) * (t & 1));
                            t = t >> 1;
                        }
                        mem[x1] = val;
                        x1++;
                    }
                    program_counter++;
                }
            }
            else
            {
                if (ins == "sd") // For store double word instruction.
                {
                    cp->c_write(addr, x[rs2i], 8);
                    program_counter++;
                }
                if (ins == "sw")
                {
                    cp->c_write(addr, x[rs2i], 4);
                    program_counter++;
                }
                if (ins == "sh")
                {
                    cp->c_write(addr, x[rs2i], 2);
                    program_counter++;
                }
                if (ins == "sb")
                {
                    cp->c_write(addr, x[rs2i], 1);
                    program_counter++;
                }
            }
        }
    }

    else if (iflf(ins)) // For I-Format Instructions of load type
    {
        iss1 >> rs2;
        rs2.pop_back();
        string d;
        iss1 >> d;
        int i = 0;
        string imm;
        for (i = 0;; i++)
        {
            if (d[i] == '(')
                break;
            else
                imm.push_back(d[i]);
        }
        i++;
        for (; i < d.size() - 1; i++)
            rs1.push_back(d[i]);
        int immi = stoi(imm);
        if (!(immi >= -2048 && immi < 2048))
        {
            program_counter++;
            cout << "Error:immediate out of range\n";
            return;
        }
        int rs1i = aliastable[rs1];
        int rs2i = aliastable[rs2];

        long long int addr = x[rs1i] + immi;
        x[rs2i] = 0;
        if (!(addr >= 0x10000 && addr < 0x50000))
        {
            cout << "Address can't be accessed " << addr << "\n";
            program_counter++;
            return;
        }
        else
        {
            if (cache_enable == 0)
            {
                if (ins == "ld")
                {
                    long long int x1 = addr - 0x10000;
                    for (int i = 0; i < 8; i++)
                    {
                        x[rs2i] += (1 << (8 * i)) * mem[x1];
                        x1++;
                    }
                    program_counter++;
                }
                if (ins == "lwu")
                {
                    long long int x1 = addr - 0x10000;
                    for (int i = 0; i < 4; i++)
                    {
                        x[rs2i] += (1 << (8 * i)) * mem[x1];
                        x1++;
                    }
                    program_counter++;
                }
                if (ins == "lhu")
                {
                    long long int x1 = addr - 0x10000;
                    for (int i = 0; i < 2; i++)
                    {
                        x[rs2i] += (1 << (8 * i)) * mem[x1];
                        x1++;
                    }
                    program_counter++;
                }
                if (ins == "lbu")
                {
                    long long int x1 = addr - 0x10000;
                    for (int i = 0; i < 1; i++)
                    {
                        x[rs2i] += (1 << (8 * i)) * mem[x1];
                        x1++;
                    }
                    program_counter++;
                }
                if (ins == "lw")
                {
                    long long int x1 = addr - 0x10000;
                    for (int i = 0; i < 4; i++)
                    {
                        x[rs2i] += (1 << (8 * i)) * mem[x1];
                        x1++;
                    }
                    if (mem[addr - 0x10000 + 3] >= 128)
                        x[rs2i] = x[rs2i] | ((-1LL) << 32);
                    program_counter++;
                }
                if (ins == "lh")
                {
                    long long int x1 = addr - 0x10000;
                    for (int i = 0; i < 2; i++)
                    {
                        x[rs2i] += ((1LL << (8 * i)) * mem[x1]);
                        x1++;
                    }
                    if (mem[addr - 0x10000 + 1] >= 128)
                        x[rs2i] = x[rs2i] | (-1LL << 16);
                    program_counter++;
                }
                if (ins == "lb")
                {
                    long long int x1 = addr - 0x10000;

                    x[rs2i] += mem[x1];
                    x1++;
                    if (mem[x1] > 127)
                    {
                        x[rs2i] << 56;
                        x[rs2i] >> 56;
                    }
                    program_counter++;
                }
            }
            else
            {
                if (ins == "ld")
                {
                    long long u_data = 0;
                    cp->c_read(addr, u_data, 8);

                    x[rs2i] = u_data;

                    program_counter++;
                }
                if (ins == "lwu")
                {

                    long long u_data = 0;
                    cp->c_read(addr, u_data, 4);

                    x[rs2i] = u_data;
                    program_counter++;
                }
                if (ins == "lhu")
                {
                    long long u_data = 0;
                    cp->c_read(addr, u_data, 2);

                    x[rs2i] = u_data;
                    program_counter++;
                }
                if (ins == "lbu")
                {
                    long long u_data = 0;
                    cp->c_read(addr, u_data, 1);

                    x[rs2i] = u_data;
                    program_counter++;
                }
                if (ins == "lw")
                {
                    long long u_data;
                    cp->c_read(addr, u_data, 4);
                    int sign = u_data >> 31;
                    if (sign == 1)
                    {
                        u_data = u_data | (-1ll << 32);
                    }

                    x[rs2i] = u_data;
                    program_counter++;
                }
                if (ins == "lh")
                {
                    long long u_data;
                    cp->c_read(addr, u_data, 2);
                    int sign = u_data >> 15;
                    if (sign == 1)
                    {
                        u_data = u_data | (-1ll << 16);
                    }
                    x[rs2i] = u_data;
                    program_counter++;
                }
                if (ins == "lb")
                {
                    long long u_data;
                    cp->c_read(addr, u_data, 1);
                    int sign = u_data >> 7;
                    if (sign == 1)
                    {
                        u_data = u_data | (-1ll << 8);
                    }

                    x[rs2i] = u_data;
                    program_counter++;
                }
            }
        }
    }

    else if (ins == "lui") // For lui instructions
    {
        iss1 >> rd;
        rd.pop_back();
        string imm;
        iss1 >> imm;
        long long int w = stoll(imm, nullptr, 16);
        w = w << 12;
        int r = aliastable[rd];
        x[r] = w;
        program_counter++;
    }

    else if (ifif(ins)) // For I-format instructions
    {
        string imm;
        iss1 >> rd;
        rd.pop_back();
        iss1 >> rs1;
        rs1.pop_back();
        iss1 >> imm;
        int rdi = aliastable[rd];
        int rs1i = aliastable[rs1];
        int immi = stoi(imm);
        if (immi >= 2048 || immi < -2048)
        {
            cout << "Immediate outof bound\n";
            program_counter++;
            return;
        }

        if (ins == "addi")
        {
            x[rdi] = x[rs1i] + immi;
        }

        else if (ins == "andi")
        {
            x[rdi] = x[rs1i] & immi;
        }

        else if (ins == "ori")
        {
            x[rdi] = x[rs1i] | immi;
        }

        else if (ins == "xori")
        {
            x[rdi] = x[rs1i] ^ immi;
        }

        else if (ins == "slli")
        {
            if (immi > 63 || immi < 0)
            {
                cout << "error in immediate\n";
                return;
            }
            x[rdi] = x[rs1i] << immi;
        }

        else if (ins == "srli")
        {
            if (immi > 63 || immi < 0)
            {
                cout << "error in immediate\n";
                program_counter++;
                return;
            }
            if (x[rs1i] >= 0 || immi == 0)
                x[rdi] = x[rs1i] >> immi;
            else
            {
                x[rdi] = x[rs1i] >> immi;
                long long int t = 1LL << (64 - immi);
                t -= 1;
                x[rdi] = x[rdi] & t;
            }
        }
        else if (ins == "srai")
        {
            if (immi > 63 || immi < 0)
            {
                cout << "error in immediate\n";
                program_counter++;
                return;
            }
            x[rdi] = x[rs1i] >> immi;
        }

        x[0] = 0;
        program_counter += 1;
    }

    else if (ins == "jal")
    {
        iss1 >> rd;
        rd.pop_back();
        string imm;
        iss1 >> imm;
        v_si.push({imm, epc});
        long long int w = label_line[imm];
        int r = aliastable[rd];
        x[r] = program_counter * 4 + 4; // x[r] stores next lines pc
        program_counter = w;
    }

    else if (ins == "jalr")
    {
        iss1 >> rs2;
        rs2.pop_back();
        string d;
        iss1 >> d;
        int i = 0;
        string imm;
        for (i = 0;; i++)
        {
            if (d[i] == '(')
                break;
            else
                imm.push_back(d[i]);
        }
        i++;
        for (; i < d.size() - 1; i++)
            rs1.push_back(d[i]);
        int immi = stoi(imm);
        if (!(immi >= -2048 && immi < 2048))
        {
            program_counter++;
            cout << "Error:immediate out of range\n";
            return;
        }
        int rs1i = aliastable[rs1];
        int rs2i = aliastable[rs2];
        x[rs2i] = program_counter * 4 + 4;
        long long int pctbe = (x[rs1i] + immi) / 4;
        if (pctbe >= v.size() || pctbe < 0)
        {
            program_counter++;
            cout << "Error : no instruction at program counter that is to be jumped\n";
            return;
        }
        else
        {
            x[rs2i] = program_counter * 4 + 4;
            program_counter = pctbe;
        }
        if (!v_si.empty())
            v_si.pop();
    }
    x[0] = 0;
}

int main()
{
    aliastable["x0"] = 0;
    aliastable["x1"] = 1;
    aliastable["x2"] = 2;
    aliastable["x3"] = 3;
    aliastable["x4"] = 4;
    aliastable["x5"] = 5;
    aliastable["x6"] = 6;
    aliastable["x7"] = 7;
    aliastable["x8"] = 8;
    aliastable["x9"] = 9;
    aliastable["x10"] = 10;
    aliastable["x11"] = 11;
    aliastable["x12"] = 12;
    aliastable["x13"] = 13;
    aliastable["x14"] = 14;
    aliastable["x15"] = 15;
    aliastable["x16"] = 16;
    aliastable["x17"] = 17;
    aliastable["x18"] = 18;
    aliastable["x19"] = 19;
    aliastable["x20"] = 20;
    aliastable["x21"] = 21;
    aliastable["x22"] = 22;
    aliastable["x23"] = 23;
    aliastable["x24"] = 24;
    aliastable["x25"] = 25;
    aliastable["x26"] = 26;
    aliastable["x27"] = 27;
    aliastable["x28"] = 28;
    aliastable["x29"] = 29;
    aliastable["x30"] = 30;
    aliastable["x31"] = 31;

    aliastable["zero"] = 0;
    aliastable["ra"] = 1;
    aliastable["sp"] = 2;
    aliastable["gp"] = 3;
    aliastable["tp"] = 4;
    aliastable["t0"] = 5;
    aliastable["t1"] = 6;
    aliastable["t2"] = 7;
    aliastable["s0"] = 8;
    aliastable["fp"] = 8;
    aliastable["s1"] = 9;
    aliastable["a0"] = 10;
    aliastable["a1"] = 11;
    aliastable["a2"] = 12;
    aliastable["a3"] = 13;
    aliastable["a4"] = 14;
    aliastable["a5"] = 15;
    aliastable["a6"] = 16;
    aliastable["a7"] = 17;
    aliastable["s2"] = 18;
    aliastable["s3"] = 19;
    aliastable["s4"] = 20;
    aliastable["s5"] = 21;
    aliastable["s6"] = 22;
    aliastable["s7"] = 23;
    aliastable["s8"] = 24;
    aliastable["s9"] = 25;
    aliastable["s10"] = 26;
    aliastable["s11"] = 27;
    aliastable["t3"] = 28;
    aliastable["t4"] = 29;
    aliastable["t5"] = 30;
    aliastable["t6"] = 31;

    while (1) // Runs in an infinite loop
    {
        string a; // It is the command that user enters in the terminal.
        getline(cin, a);
        istringstream iss(a); // separtes the blank spaces
        string inst;
        iss >> inst; // Extract the command from iss into inst.
        srand(static_cast<unsigned int>(time(0)));
        if (inst == "p")
        {
            cp->print();
            cout << endl;
        }
        if (inst == "cache_sim")
        {

            string x;
            iss >> x;
            if (x == "enable")
            {

                iss >> x;
                if (cache_enable == 0 && (program_completion == 1 || v.size() == 0))
                {
                    cache_enable = 1;

                    ifstream f(x);

                    string s;
                    getline(f, s);
                    int cache_capacity = stoi(s);
                    getline(f, s);
                    int block_size = stoi(s);
                    getline(f, s);
                    int asso = stoi(s);
                    getline(f, s);

                    int r_p; //(FIFO or LRU or RANDOM)
                    if (s == "RANDOM")
                    {
                        r_p = 0;
                    }
                    if (s == "LRU")
                    {
                        r_p = 2;
                    }
                    if (s == "FIFO")
                    {
                        r_p = 1;
                    }
                    getline(f, s);
                    int w_p;
                    if (s == "WB")
                    {
                        w_p = 2;
                    }
                    if (s == "WT")
                    {
                        w_p = 1;
                    }
                    cp = new cache(cache_capacity, block_size, asso, r_p, w_p);
                }
                else
                {
                    cout << "cache can't be accesed at the current state of execution\n";
                }
            }
            if (x == "disable")
            {
                if (cache_enable == 1 && (program_completion == 1 || v.size() == 0))
                {
                    for (int i = 0; i < cp->in; i++)
                    {
                        for (int j = 0; j < cp->assoc; j++)
                        {
                            delete[] cp->p[i][j].m;
                        }
                    }
                    for (int i = 0; i < cp->in; i++)
                    {
                        delete[] cp->p[i];
                    }
                    delete cp->p;
                    delete cp;
                    cp = 0;
                    cache_enable = 0;
                }
                else
                {
                    cout << "cache can not be disabled at current state\n";
                }
            }

            if (x == "status")
            {
                if (cache_enable)
                {
                    cout << "Cache enabled:\n";
                    cout << "Cache Size: " << cp->capacity << endl
                         << "Block Size: " << cp->block_size << endl
                         << "Associativity: " << cp->assoc << endl
                         << "Replacement Policy: ";
                    if (cp->r_policy == 0)
                    {
                        cout << "RAND";
                    }
                    if (cp->r_policy == 1)
                    {
                        cout << "FIFO";
                    }
                    if (cp->r_policy == 2)
                        cout << "LRU";
                    cout << endl
                         << "Write Back Policy: ";
                    if (cp->w_policy == 1)
                    {
                        cout << "WT\n";
                    }
                    if (cp->w_policy == 2)
                        cout << "WB\n";
                }
                else
                {
                    cout << "Cache in disabled state\n";
                }
            }

            if (x == "invalidate")
            {
                if (cache_enable)
                {
                    for (int i = 0; i < cp->in; i++)
                    {

                        for (int j = 0; j < cp->assoc; j++)
                        {
                            if (cp->p[i][j].dirty_bit)
                            {
                                for (int k = 0; k < cp->block_size; k++)
                                {
                                    char t = cp->p[i][j].m[k];
                                    long long dd = (cp->p[i][j].tag << (log(cp->in)) << (log(cp->block_size))) + i * cp->block_size + k - 0x10000;
                                    mem[dd] = t;
                                }
                            }
                        }
                    }
                    for (int i = 0; i < cp->in; i++)
                    {

                        for (int j = 0; j < cp->assoc; j++)
                        {
                            cp->p[i][j].valid_bit = 0;
                        }
                    }
                }
                else
                {
                    cout << "no cache present to invalidate\n";
                }
            }
            if (x == "dump")
            {
                string y;
                iss >> y;
                ofstream ff(y);
                if (cache_enable)
                {
                    for (int i = 0; i < cp->in; i++)
                    {
                        for (int j = 0; j < cp->assoc; j++)
                        {
                            if (cp->p[i][j].valid_bit)
                            {
                                ff << "Set: 0x" << hex << i << ",Tag: 0x" << j << ", ";
                                if (cp->p[i][j].dirty_bit)
                                {
                                    ff << "Dirty\n";
                                }
                                else
                                    ff << "Clean\n";
                            }
                        }
                    }
                }
                else
                {
                    cout << "no cache present to invalidate\n";
                }
            }
            if (x == "stats")
            {
                if (cache_enable)
                {
                    if (cp->accesses)
                    {
                        cout << "D-cache statistics: Accesses=" << cp->accesses << ", Hit=" << cp->hits << ", Miss=" << (cp->accesses - cp->hits) << ", Hit Rate=" << ((float)cp->hits / cp->accesses) << endl;
                    }
                    else
                    {
                        cout << "Cache not yet accesed \n";
                    }
                }
                else
                    cout << "no cache present to show stats\n";
            }
        }
        if (inst == "load") // For "load" command
        {
            v.resize(0);        // Clears the content in vector, because values should be fresh when loading another file.
            label_line.clear(); // Clears the links of prevoiusly mapped things.
            program_counter = 0;
            breakpoints.resize(0); // Clears previously stored breakpoint information
            dot_count = 0;
            for (int i = 0; i < 32; i++)
            {
                x[i] = 0;
            }
            while (!v_si.empty())
                v_si.pop();         // Empty's the stack v_si, cause previously some data is stored
            v_si.push({"main", 0}); // Push the string main and 0 into stack
            string fn;              // extracts the file name from the command we entered before and sends to fn.
            iss >> fn;
            for (int i = 0; i < fn.size(); i++)
            {
                if (fn[i] == '.')
                    break;
                else
                {
                    file_name.push_back(fn[i]);
                }
            }
            file_name = file_name + ".output";
            ofstream file(file_name, std::ios::trunc);
            if (!file.is_open())
            {
                std::cerr << "Error opening file to empty it." << std::endl;
                return 1;
            }
            file.close(); // Close the file after emptying it

            ifstream f(fn);          // Opens the file fn into stream f where we can read later.
            int l = 1;               // Variable that counts the line number in file
            string lc;               // temporarily stores each line of the file when that line is executing.
            ifstream f2(fn);         // Define another stream where we store only lines starting with '.'
            string x33;              // x33 temporary string that stores lines with '.'
            while (getline(f2, x33)) // Counting no of dots in the file.
            {
                if (x33[0] != '.')
                    break;
                dot_count++;
            }
            while (getline(f, lc)) // goes through each line in the file fn.
            {
                int i = 0;
                int r = 0;
                for (; i < lc.size(); i++) // Checks whether label is there or not and also stores the label name
                {
                    if (lc[i] == ':')
                    {
                        r = 1;
                        break;
                    }
                }
                string label; // if line contains the label it gets stored in this string.
                if (r)
                {
                    for (int j = 0; j < i; j++)
                        label.push_back(lc[j]);            // Push the label name into 'string label'.
                    label_line[label] = l - 1 - dot_count; // mapping the label with its corresponding line number - dot_counts.
                }

                l++; // Line counter
            }
            ifstream f1(fn); // Reopen the file to process actual instructions & data in the file
            string x1;       // Here lines get stored in x1.
            int gl = 0;
            int mn = 0;
            int k = 1; // Line counter for
            while (getline(f1, x1))
            {
                if (k == 1)
                {
                    if (x1[0] != '.')
                        gl = 1; // If it is normal instruction then set gl to 1.
                }
                if (!gl) // If not (i.e., it may be .data or .dword .... or .text)
                {
                    if (x1[0] == '.')
                    {
                        istringstream iss2(x1); // We need more than one, beacuse the content in iss completes after one time use
                        istringstream iss3(x1);
                        string q1;
                        iss2 >> q1;
                        iss3 >> q1;
                        if (q1 == ".data")
                            continue;           // If it is .data then go to the next line in the file.
                        else if (q1 == ".text") // If it is .text go to next line in the file also make sure gl = 1,
                        {
                            gl = 1; // Because after .text we get normal instructions so set gl to 1.
                            continue;
                        }
                        else
                        {
                            string hh; // Data value in the respective type
                            int count = 0;
                            while (iss2 >> hh)
                                count++; // Counting number of data values present in the instruction.
                            int shc = 0;
                            if (q1 == ".dword")
                                shc = 8; // Size in bytes for their corresponding data type.
                            if (q1 == ".word")
                                shc = 4;
                            if (q1 == ".half")
                                shc = 2;
                            if (q1 == ".byte")
                                shc = 1;
                            for (int i = 0; i < count; i++) // Processing each data value in . section
                            {
                                iss3 >> hh;
                                if (i != count - 1)
                                    hh.pop_back(); // Remove comma if it is not the last one.
                                long long int y = stoll(hh);
                                for (int i = 0; i < shc; i++) // Split the value into bytes & store into memory
                                {
                                    int val = 0;
                                    for (int j = 0; j < 8; j++)
                                    {
                                        val += ((1 << j) * (y & 1));
                                        y = y >> 1;
                                    }
                                    mem[mn + i] = val; // Storing.
                                }
                                mn += shc; // Move to next memory address.
                            }
                        }
                    }
                }
                if (gl) // If it is normal instruction.
                {

                    int q = 0; // Used for searching label in instructions
                    for (; q < x1.size(); q++)
                    {
                        if (x1[q] == ':')
                        {
                            q = 0;
                            break;
                        }
                    }
                    int remc = 0; // Variable to store position where label ends in instruction
                    if (!q)       // If label found
                    {
                        for (int i = 0; i < x1.size(); i++) // Check where the first space comes.
                        {
                            if (x1[i] == ' ')
                            {
                                remc = i + 1; // Note down the position before actual instructiion starts
                                break;
                            }
                        }
                        x1.erase(0, remc); // Remove the label and space from the instruction
                    }
                    v.push_back(x1); // Push back the instruction into vector v.
                }
                k++; // which counts the number of prcessed instructions.
            }
        }

        else if (inst == "run") // For the "run" command
        {

            if (program_counter != v.size()) // When program is not executed compeltely
            {                                // go through instructions until a barekpoint or program end is found.

                while (program_counter < v.size() && (((find(breakpoints.begin(), breakpoints.end(), program_counter)) == breakpoints.end())))
                { // print Instruction and program counter

                    cout << "Executed: " << v[program_counter] << "; PC = 0x" << hexa(4 * program_counter) << endl;
                    execute(v[program_counter]); // calling the function execute
                                                 // increase instruction counter
                }
                if (program_counter != v.size())
                    cout << "Execution stopped at breakpoint\n"; // when execution stopped at braek point.

                else // Program is completed now and PC is changed to 1 for future.
                {
                    program_completion = 1;
                    cout << "Program has been completely executed\n";
                }
                if (cache_enable)
                    cout << "D-cache statistics: Accesses=" << cp->accesses << ", Hit=" << cp->hits << ", Miss=" << (cp->accesses - cp->hits) << ", Hit Rate=" << ((float)cp->hits / cp->accesses) << endl;
                else
                    cout << "no cache present to show stats\n";
            }
            else // Program already completed
            {
                program_completion = 1;
                cout << "Program has already been completely executed\n";
                if (cache_enable)
                    cout << "D-cache statistics: Accesses=" << cp->accesses << ", Hit=" << cp->hits << ", Miss=" << (cp->accesses - cp->hits) << ", Hit Rate=" << ((float)cp->hits / cp->accesses) << endl;
                else
                    cout << "no cache present to show stats\n";
            }
            cout << endl;
        }

        else if (inst == "regs") // For the "regs" command.
        {
            cout << "Registers\n";
            for (int i = 0; i < 32; i++)
                cout << "x" << i << " = 0x" << hexa(x[i]) << endl; // Print values in all 32 registers in hexadecimal format
            cout << endl;
        }

        else if (inst == "exit") // For the "exit" command.
        {
            cout << "Exited the simulator\n";
            cout << endl; // print the exit message and come out of loop and terminate the program.
            break;
        }

        else if (inst == "step") // For the "step" command.
        {
            if (program_counter < v.size()) // checking whether there are instructions left to execute or not.
            {
                cout << "Executed: " << v[program_counter] << " 0x" << hexa(4 * program_counter) << endl; // Print current instruction and PC
                execute(v[program_counter]);                                                              // Execute the instruction.
            }
            else
                cout << "Nothing to step\n";
            cout << endl;
        }

        else if (inst == "break") // For the "break" commnd.
        {
            int b;
            iss >> b;                                    // read line number from the command
            breakpoints.push_back(b - 1 - dot_count);    // Push it into break point vector
            for (int i = 0; i < breakpoints.size(); i++) // print all the break points
            {
                cout << "Breakpoint at " << breakpoints[i] + 1 + dot_count << " ";
            }
            cout << endl;
            cout << endl;
        }

        else if (inst == "mem") // For the "mem" command
        {
            int x12;
            long long int x11;
            iss >> hex >> x11;
            iss >> dec >> x12;
            ;
            for (long long int i = 0; i < x12; i++)
            {
                cout << "Memory[0x" << hexa(x11 + i) << "] = 0x" << char_to_hexa(mem[x11 + i - 0x10000]) << endl;
            }
            cout << endl;
        }

        else if (inst == "del") // For the "del" command
        {
            string r;
            iss >> r;
            if (r == "break")
            {
                int b;
                iss >> b;
                breakpoints.erase(remove(breakpoints.begin(), breakpoints.end(), b - 1 - dot_count), breakpoints.end());
                if (breakpoints.size())
                    cout << "Breakpoints at ";
                for (int i = 0; i < breakpoints.size(); i++)
                {
                    cout << "" << breakpoints[i] + 1 + dot_count << " ";
                }
                cout << endl;
            }
            cout << endl;
        }

        else if (inst == "show-stack") // For the "show-stack" command
        {
            string r;
            cout << "Call Stack:\n";
            stack<s_int> s = v_si;
            vector<s_int> z;
            while (s.empty() != 1)
            {
                z.push_back(s.top());
                s.pop();
            }
            for (int i = z.size() - 1; i >= 0; i--)
            {
                if (program_completion == 1)
                {
                    cout << "Execution Call Stack: Execution complete\n";
                }
                else
                    cout << z[i].a << ":" << z[i].line << endl;
            }
        }
    }
    return 0;
}