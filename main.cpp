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
    size_t alphabet_size;
    size_t transitions_n;
    size_t accept_n;
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

void input_str(char* &str, size_t &str_len) {
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

elem_t rem(char* str, size_t str_len, elem_t q, const table_t &table) {
    for (size_t i=0;i<str_len; i++) {
        char c = str[i];
        q = table[q][char2elem(c)];
    }
    return q;
}

bool parem(char* str, size_t str_len, const table_t &table,const elem_set &accept_states, size_t p, size_t rnk) {
    size_t start_position;
    size_t final_position;
    char* pi_input;
    size_t pi_input_len;
    char pi_prev;

    vector<row_t> partial_result;

    for (size_t rank=0;rank<p;rank++){
        // SPLIT & COMMUNICATE

        // Scatter
        // > Get start and end
        start_position = rank * (str_len/p);
        if (rank == p-1) { final_position = str_len; }
        else { final_position = (rank+1) * (str_len/p); }
        // > Copy
        pi_input_len = final_position-start_position;
        pi_input = new char[pi_input_len+1];
        strncpy(pi_input, str+start_position, pi_input_len);

        // Send/Recv
        if (rank != 0) {pi_prev=str[start_position-1];}

        // CALCULATE INITIAL STATES
        cout << "RANK" << rank << " - str:" << pi_input << endl;
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
        cout << "R: ";
        for (auto &q : R) {
            auto next_q = rem(pi_input,pi_input_len,q,table);
            transition[q] = next_q;
            cout << q << "->" << next_q << " ";
        }
        cout << endl;

        // REDUCE (Send transition)
        partial_result.push_back(transition);
    }

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

int main(int argc,char **argv) {
    int NoOfProcess, ProcessNo;
    MPI_Init(&argc, &argv);    
    MPI_Comm_size(MPI_COMM_WORLD, &NoOfProcess);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcessNo);

    table_t transition_table;
    elem_set accept_states;
    char* str;
    size_t str_len;
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
    if (ProcessNo == 0) res = parem(str, str_len, transition_table, accept_states, NoOfProcess, ProcessNo);

    if (ProcessNo == 0){
        cout << "RESULT:" << res << endl;
        cout << "END" << endl;        
    }

    MPI_Finalize();
    return 0;
}