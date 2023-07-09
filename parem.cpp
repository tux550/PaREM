#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <cstring>
#include<mpi/mpi.h>




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
char*    str;
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

// #define GATHEREAR
#define REDUCIR

// Operacion Reduce Binario
MPI_Op MPI_BIN_CONN;

void conexion_binaria(void *invec, void *inoutvec, int *len, MPI_Datatype* datatype){
    elem_t *invec_val = (elem_t *) invec;
    elem_t *inoutvec_val = (elem_t *) inoutvec;

    elem_t *inoutvec_val_copy = new elem_t[*len];

    for(auto i = 0; i<*len; i++){
        inoutvec_val_copy[i] = inoutvec_val[i];
    }

    for(auto i = 0; i<*len; i++){
        if(invec_val[i] == -1)
            inoutvec_val[i] = -1;
        else
            inoutvec_val[i] = inoutvec_val_copy[invec_val[i]];
    }

    delete []inoutvec_val_copy;
}


// TRANSITION TABLE FUNCTIONS
void set_table(dim_t row, dim_t col, elem_t val) {
    transition_table[row*alphabet_size + col] = val;
}
elem_t get_table(dim_t row, dim_t col) {
    return transition_table[row*alphabet_size + col];
}

// INPUT TABLE
void input_table(int rank, int size) {
    // Tmp vars
    count_t accept_n;
    elem_t  tmp;
    // GET DIMENSIONS
    if (rank == 0) {
        cin >> transitions_n >> alphabet_size >> accept_n;
        cout<<transitions_n << alphabet_size <<accept_n<<endl;
    }
    // COMMUNICATE DIMENSIONS
    MPI_Bcast(&transitions_n, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&alphabet_size, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    // RESERVE TABLE
    transition_table = new elem_t[transitions_n*alphabet_size];
    // GET TABLE
    if (rank == 0) {
        for (auto t=0; t<transitions_n; t++){
            for (auto a=0; a<alphabet_size; a++){
                cin >> tmp;
                set_table(t,a,tmp);
            }
        }
    }
    // COMMUNICATE TABLE
    MPI_Bcast(transition_table, transitions_n*alphabet_size, MPI_LONG, 0, MPI_COMM_WORLD);
    // GET ACCEPT STATES
    if (rank == 0) {
        for (auto s=0; s<accept_n; s++) {
            cin >> tmp;
            accept_states.insert(tmp);
        }
    }
    // RESERVE SPACE FOR RESULT VARIABLES
    total_transitions = new elem_t[transitions_n*size];
    partial_transitions = new elem_t[transitions_n];
    // Return
    return;
}

// INPUT STR
void input_str(int rank, int size) {
    // Tmp vars
    string tmp_str;

    // ACCEPT STRING
    if (rank == 0) {
        // Accept string
        cin >> tmp_str;
        // Reserve space
        str_len = tmp_str.length();
        str = new char[str_len + 1]; //+1 for null terminator
        // Copy to char array
        strcpy(str, tmp_str.c_str());
    }

    // BROADCAST STR_LEN
    MPI_Bcast(&str_len, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

    // CALCULATE SHIFT AND SCATTER LENGTH
    shift             = (str_len % size);    // Remaining
    total_scatter_len = str_len - shift;  // Total Scatter Length
    part_scatter_len  = total_scatter_len/size; // Each process
    
    // RESERVE STRING SPACE
    if (rank==0) {pi_input_len = shift+part_scatter_len;}
    else {pi_input_len = part_scatter_len;}
    pi_input = new char[part_scatter_len+1];

    // SCATTER STR
    MPI_Scatter(str+shift,part_scatter_len,MPI_CHAR,pi_input,part_scatter_len,MPI_CHAR,0,MPI_COMM_WORLD);
    pi_input[pi_input_len] = '\0';

    // EDGE CASE: copy extra data (n%p != 0) to Rank0
    if (rank == 0 && shift != 0) {
        strncpy(pi_input, str, pi_input_len);
    }

    // COMMUNICATE BOUNDARIES
    // Send
    if (rank != size-1) {
        // SEND pi_input[pi_input_len-1] to rank+1
        MPI_Send(pi_input+(pi_input_len-1),1,MPI_CHAR,rank+1,0,MPI_COMM_WORLD);
    }
    // Recv
    if (rank != 0) {
        // RECV save to pi_prev
        MPI_Recv(&pi_prev,1,MPI_CHAR,rank-1,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // PRINT
    //printf("rank: %d pi_input: %s\n", rank, pi_input);
    //printf("rank: %d pi_prev: %c\n", rank, pi_prev);



    // Return
    return;
}

elem_t char2elem(char c) {
    return (elem_t) (c-'a');
}


elem_t rem(elem_t q) {
    for (unsigned long i=0;i<pi_input_len; i++) {
        if (q==-1){return -1;}
        char c = pi_input[i];
        q = get_table(q,char2elem(c));
    }
    return q;
}

bool parem(int rank, int size) {

    // CALCULATE INITIAL STATES
    elem_set R{};
    if (rank == 0) {
        R.insert(0);
    } else{
        // - Calc S
        elem_set S{};
        for (elem_t q=0; q<transitions_n; q++) {
            auto next_q = get_table(q,char2elem(pi_input[0]));
            if (next_q != -1) {
                S.insert(q);
            }
        }
        // - Calc L
        elem_set L{};
        for (elem_t q=0; q<transitions_n; q++) {
            auto next_q = get_table(q,char2elem(pi_prev));
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
    for (elem_t q=0; q<transitions_n; q++) {
        if (R.find(q) != R.end()) {
            auto next_q = rem(q);
            partial_transitions[q] = next_q;
        }
        else {
            partial_transitions[q] = -1;
        }
    }

    // GATHER o REDUCE
    // Note: se puede reducir de forma binaria
    // result[i] =  [ partial_result(i+1)[q] if q!=-1 else -1 for q in partial_result(i) ]
    #ifdef REDUCIR
    MPI_Reduce(partial_transitions, total_transitions, transitions_n, MPI_LONG, MPI_BIN_CONN, 0, MPI_COMM_WORLD);


    if (rank == 0) {
        if (accept_states.find(total_transitions[0]) != accept_states.end()) {
           return true;
        }
        else {
            return false;
        }
    }
    #endif

    #ifdef GATHEREAR
    MPI_Gather(partial_transitions,transitions_n,MPI_LONG,total_transitions,transitions_n,MPI_LONG,0,MPI_COMM_WORLD);
    if (rank == 0) {
        cout<<transitions_n<<endl;
        for(int i = 0; i<transitions_n*size; i++){
            if(i % transitions_n == 0)
            cout<<endl;
            cout<<total_transitions[i]<<" ; ";
        }
        len_t offset = 0;
        elem_t q = 0;
        for (auto i=0; i<size; i++) {
            q = total_transitions[q+offset];
            if (q == -1) {break;}
            offset += transitions_n;
        }
        if (accept_states.find(q) != accept_states.end()) {
           return true;
        }
        else {
            return false;
        }
    }
    #endif
    else {
        return false;
    }

}

int main(int argc,char **argv) {
    int NoOfProcess, ProcessNo;
    MPI_Init(&argc, &argv);    
    MPI_Comm_size(MPI_COMM_WORLD, &NoOfProcess);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcessNo);

    MPI_Op_create(conexion_binaria,0, &MPI_BIN_CONN);    

    char* str;
    unsigned long str_len;
    bool res;




    // GET TABLE FROM STD INPUT
    input_table(ProcessNo,NoOfProcess);
    // GET INPUT STR FROM STD INPUT
    input_str(ProcessNo,NoOfProcess);

    // PaREM
    res = parem(ProcessNo,NoOfProcess);

    if (ProcessNo == 0){
        cout << "RESULT:" << res << endl;
        cout << "END" << endl;        
    }

    MPI_Finalize();
    return 0;
}