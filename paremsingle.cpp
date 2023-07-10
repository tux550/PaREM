#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <cstring>





using namespace std;

using count_t = long;
using dim_t = unsigned long;
using len_t = unsigned long;
using elem_t = long;
using elem_set = set<elem_t>;

// GLOBAL VARIABLES
elem_t*  transition_table;
dim_t    alphabet_size;
dim_t    transitions_n;
elem_set accept_states;
string   input_str;
len_t    str_len;
// PROCESS VARIABLES
len_t shift;
len_t total_scatter_len;
len_t part_scatter_len;
char* pi_input;
len_t pi_input_len;
char pi_prev;
// RESULT VARIABLES
elem_t* partial_transitions;
elem_t* total_transitions;
elem_t* inoutvec_val_copy;

// #define GATHEREAR
#define REDUCIR

// Operacion Reduce Binario

// TRANSITION TABLE FUNCTIONS
void set_table(dim_t row, dim_t col, elem_t val) {
    transition_table[row*alphabet_size + col] = val;
}
elem_t get_table(dim_t row, dim_t col) {
    return transition_table[row*alphabet_size + col];
}

// INPUT TABLE
void input_table() {
    // Tmp vars
    count_t accept_n;
    elem_t  tmp;
    // GET DIMENSIONS
    cin >> transitions_n >> alphabet_size >> accept_n;
    // RESERVE TABLE
    transition_table = new elem_t[transitions_n*alphabet_size];
    // GET TABLE
    for (auto t=0; t<transitions_n; t++){
        for (auto a=0; a<alphabet_size; a++){
            cin >> tmp;
            set_table(t,a,tmp);
        }
    }
    // GET ACCEPT STATES
    for (auto s=0; s<accept_n; s++) {
        cin >> tmp;
        accept_states.insert(tmp);
    }

    // Return
    return;
}

// INPUT STR
void read_input_str() {
    // Tmp vars
    // Accept string
    cin >> input_str;
    return;
}

elem_t char2elem(char c) {
    return (elem_t) (c-'a');
}


elem_t rem(elem_t q) {
    for (unsigned long i=0;i < input_str.size(); i++) {
        if (q==-1){return -1;}
        char c = input_str[i];
        q = get_table(q,char2elem(c));
    }
    return q;
}

bool run_rem() {

    // CALCULATE REM

    elem_t result_state = rem(0);

    if (accept_states.find(result_state) != accept_states.end()) {
        return true;
    }
    else {
        return false;
    }


}

int main(int argc,char **argv) {
    // freopen("./test_automatas/n_1000000/accept.txt", "r", stdin);
    // freopen("./generate_automatas/result.txt", "r", stdin);
    // freopen("./generate_automatas/result.txt", "r", stdin);
    freopen(input_path, "r", stdin);
    // freopen("output", "w", stdout);
 
    bool res;

    // GET TABLE FROM STD INPUT
    input_table();
    // GET INPUT STR FROM STD INPUT
    read_input_str();

    // PaREM
    auto t1 = std::chrono::high_resolution_clock::now();
    res = run_rem();
    auto t2 = std::chrono::high_resolution_clock::now();

    // cout << "RESULT:" << res << endl;
    // cout << "END" << endl;        
    cout<< (std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count())/1000000.0<<endl;
    

    return 0;
}