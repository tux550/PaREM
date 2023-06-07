#include <iostream>
#include <vector>
#include <string>


using namespace std;


using elem_t = long;
using row_t = vector<elem_t>;
using table_t = vector<row_t>;

void input_table(table_t &table, row_t &accept_states) {
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
        accept_states.push_back(tmp);
    }
    // Return
    return;
}

elem_t char2elem(char c) {
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

void parem(string str, const table_t &table,const row_t &accept_states, size_t p) {
    size_t start_position;
    size_t final_position;
    string pi_input;
    for (auto rank = 0; rank < p; rank++) {
        // SPLIT & COMMUNICATE
        start_position = rank * (str.size()/p);
        if (rank = p-1) { final_position = str.size() - 1; }
        else { final_position = (rank+1) * (str.size()/p); }
        pi_input = str.substr(start_position, final_position);
        // CALCULATE INITIAL STATES
        // - Calc S
        row_t S; // TODO: cambiar a set
        for (elem_t q=0; q<table.size(); q++) {
            auto next_q = table[q][char2elem(pi_input[0])];
            if (next_q != -1) {
                S.push_back(q);
            }
        }
        // - Calc L
        row_t L; // TODO: cambiar a set
        for (elem_t q=0; q<table.size(); q++) {
            auto next_q = table[q][char2elem(pi_input[0])];
            if (next_q != -1) {
                L.push_back(q);
            }
        }
        // TODO L S INTERSECTION
    }

}

int main() {
    table_t transition_table;
    row_t accept_states;
    input_table(transition_table, accept_states);

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
    cout << "[-----]" << endl;

    return 0;
}