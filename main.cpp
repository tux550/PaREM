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