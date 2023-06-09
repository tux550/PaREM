#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <cstring>

#include<mpi.h>

using namespace std;

using count_t = long;
using elem_t = long;
using elem_set = set<elem_t>;
using row_t = vector<elem_t>;
using table_t = vector<row_t>;

void input_table(table_t &table, elem_set &accept_states) {
    unsigned long alphabet_size;
    unsigned long transitions_n;
    unsigned long accept_n;
    elem_t tmp;
    // GET DIMENSIONS
    cin >> transitions_n >> alphabet_size >> accept_n;
    // GET TABLE
    for (auto t=0; t<transitions_n; t++){
        row_t tmp_row{};
        for (auto a=0; a<alphabet_size; a++){
            cin >> tmp;
            tmp_row.push_back(tmp);
        }
        table.push_back(tmp_row);
    }
    // GET ACCEPT STATES
    for (auto s=0; s<accept_n; s++) {
        cin >> tmp;
        accept_states.insert(tmp);
    }
    // Return
    return;
}

void input_str(char* &str, unsigned long &str_len) {
    // Accept string
    string tmp_str;
    cin >> tmp_str;
    // Reserve space
    str_len = tmp_str.length();
    str= new char[str_len + 1]; //+1 for null terminator
    // Copy to char array
    strcpy(str, tmp_str.c_str());
    // Return
    return;
}

elem_t char2elem(char c) {
    // TODO: modificar con ASCII para version general. este es para el input de ejemplo
    switch (c)
    {
    case 'p':
        return 0;
    case 'a':
        return 1;
    case 'r':
        return 2;
    case 'e':
        return 3;
    case 'l':
        return 4;
    default:
        return -1;
    }
}

elem_t rem(char* str, unsigned long str_len, elem_t q, const table_t &table) {
    for (unsigned long i=0;i<str_len; i++) {
        char c = str[i];
        q = table[q][char2elem(c)];
    }
    return q;
}

bool parem(char* str, unsigned long str_len, const table_t &table,const elem_set &accept_states, unsigned long p, unsigned long rank) {
    unsigned long start_position;
    unsigned long final_position;
    unsigned long shift;
    unsigned long total_scatter_len;
    unsigned long part_scatter_len;
    char* pi_input;
    unsigned long pi_input_len;
    char pi_prev;

    vector<row_t> partial_result;

    // SPLIT & COMMUNICATE

    // Broadcast str_len
    MPI_Bcast(&str_len, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    // > Get shift and scatter len
    shift = (str_len % p);
    total_scatter_len = str_len - shift;
    part_scatter_len = total_scatter_len/p;
    // > Reserve
    if (rank==0) {pi_input_len = shift+part_scatter_len;}
    else {pi_input_len = part_scatter_len;}
    pi_input = new char[part_scatter_len+1];
    // Scatter
    MPI_Scatter(str+shift,part_scatter_len,MPI_CHAR,pi_input,part_scatter_len,MPI_CHAR,0,MPI_COMM_WORLD);
    pi_input[pi_input_len] = '\0';
    // Edge case: copy extra data (n%p != 0) to Rank0
    if (rank == 0 && shift != 0) {
        strncpy(pi_input, str, pi_input_len);
    }

    MPI_Status estado[1];
    // Send
    if (rank != p-1) {
        // SEND pi_input[pi_input_len-1] to rank+1
        MPI_Send(pi_input+(pi_input_len-1),1,MPI_CHAR,rank+1,0,MPI_COMM_WORLD);
    }
    // Recv
    if (rank != 0) {
        // RECV save to pi_prev
        MPI_Recv(&pi_prev,1,MPI_CHAR,rank-1,0,MPI_COMM_WORLD, estado);
    }

    // PRINT
    printf("rank: %ld pi_input: %s\n", rank, pi_input);
    printf("rank: %ld pi_prev: %c\n", rank, pi_prev);

    // TODO: Broadcast table


    // PARALIZACION PENDIENTE
    if (rank == 0){    
        // CALCULATE INITIAL STATES
        elem_set R{};
        if (rank == 0) {
            R.insert(0);
        } else{
            // - Calc S
            elem_set S{};
            for (elem_t q=0; q<table.size(); q++) {
                auto next_q = table[q][char2elem(pi_input[0])];
                if (next_q != -1) {
                    S.insert(q);
                }
            }
            // - Calc L
            elem_set L{};
            for (elem_t q=0; q<table.size(); q++) {
                auto next_q = table[q][char2elem(pi_prev)];
                if (next_q != -1) {
                    L.insert(next_q);
                }
            }
            // - Intersection(L,S)
            std::set_intersection(
                S.begin(), S.end(),
                L.begin(), L.end(),
                std::inserter(R, R.begin())
            );
        }

        // CALCULATE REM
        row_t transition(table.size(),-1);
        
        printf("R %ld---", rank);
        for (auto &q : R) {
            auto next_q = rem(pi_input,pi_input_len,q,table);
            transition[q] = next_q;
            printf("R %ld q=%ld -> next_q=%ld", rank, q, next_q);
        }
        cout << endl;


        // REDUCE (Send transition)
        partial_result.push_back(transition);
        // REDUCE
        // Note: se puede reducir de forma binaria
        // result[i] =  [ partial_result(i+1)[q] if q!=-1 else -1 for q in partial_result(i) ]
        elem_t q = 0;
        for (auto &t : partial_result) {
            q = t[q];
            if (q == -1) {break;}
        }
        if (accept_states.find(q) != accept_states.end()) {
            return true;
        }
        else {
            return false;
        }
    }
}

int main(int argc,char **argv) {
    int NoOfProcess, ProcessNo;
    MPI_Init(&argc, &argv);    
    MPI_Comm_size(MPI_COMM_WORLD, &NoOfProcess);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcessNo);

    table_t transition_table;
    elem_set accept_states;
    char* str;
    unsigned long str_len;
    bool res;
    if (ProcessNo == 0) {
        // GET TABLE FROM STD INPUT
        input_table(transition_table, accept_states);
        // GET INPUT STR FROM STD INPUT
        input_str(str, str_len);
        // PRINT FOR DEBUGGING
        cout << "[INPUTS]" << endl;
        cout << "Transition Table:" << endl;
        for (auto &row : transition_table) {
            for (auto &e : row) {
                cout << e << " "; 
            }
            cout << endl;
        }
        cout << "Acceptance states:" << endl;
        for (auto &e : accept_states) {
            cout << e << " "; 
        }
        cout << endl;
        cout << "String:" << endl;
        cout << str << endl;
        cout << "[-----]" << endl;
    }

    // PaREM
    res = parem(str, str_len, transition_table, accept_states, NoOfProcess, ProcessNo);

    if (ProcessNo == 0){
        cout << "RESULT:" << res << endl;
        cout << "END" << endl;        
    }

    MPI_Finalize();
    return 0;
}