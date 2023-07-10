import random
import math

def create_random_automata(alphabet_size,string_size,n,file):
    n_accept_states = math.ceil(n/100)
    states = {i for i in range(1,n)}
    accept_states = set(random.sample(range(1,n),k=n_accept_states))
    accept_states.add(0)
    open_set = {0}
    #automaton[i][0] estados a los que se puede ir (con su simbolo)
    #automaton[i][1] estados a los que se puede regresar (con su simbolo)
    automaton = [[{},{}]  for i in range(n)] 
    automaton_to_write = [[-1 for j in range(alphabet_size)] for i in range(n)]
    not_used_symbols =  [{j for j in range(alphabet_size)}  for i in range(n)] 
    alphabet = [i for i in range(alphabet_size)]

    #random dfs
    while len(states) > 0:
        # print(len(states))

        current_state = random.choice(tuple(open_set)) 
        open_set.remove(current_state)

        random.shuffle(alphabet)

        for symbol in alphabet:
            if len(states) > 0:
                selected_state = random.choice(tuple(states))
                states.remove(selected_state)
                automaton[current_state][0][selected_state] = [symbol]
                automaton[selected_state][1][current_state] = [symbol]
                automaton_to_write[current_state][symbol] = selected_state
                not_used_symbols[current_state].remove(symbol)
                # print(f'El estado {current_state} puede ir al {selected_state}')
                open_set.add(selected_state)
            else:
                break

    # for row in automaton:
        # print(row)
    # print(np.array(not_used_symbols))

    for i in range(n):

        # if i % 1000 == 0:
        # print(i)
        if len(not_used_symbols[i]) > 0:
            selected_symbol = random.choice(tuple(not_used_symbols[i]))
            # print(f'Se pudo ciclo en {i} con {selected_symbol}')
            not_used_symbols[i].remove(selected_symbol)
            automaton[i][0][i] = [selected_symbol]
            automaton[i][1][i] = [selected_symbol]
            automaton_to_write[i][selected_symbol] = i
        
        number_of_symbols = random.randint(0,len(not_used_symbols[i]))
        # number_of_symbols = len(not_used_symbols[i])

        for _ in range(number_of_symbols):
            selected_symbol = random.choice(tuple(not_used_symbols[i]))
            selected_state = random.randint(0,n-1)
            # print(f'Se puede ir de {i} hasta {selected_state} con {selected_symbol}')
            not_used_symbols[i].remove(selected_symbol)

            automaton_to_write[i][selected_symbol] = selected_state 
            if not automaton[i][0].get(selected_state):
                automaton[i][0][selected_state] = [selected_symbol]
            else:
                automaton[i][0][selected_state].append(selected_symbol) 

            if not automaton[selected_state][1].get(i):
                automaton[selected_state][1][i] = [selected_symbol]
            else:
                automaton[selected_state][1][i].append(selected_symbol)



    # for row in automaton:
        # print(row)
    # print(np.array(not_used_symbols))

    automaton_to_write[0][1] = 0 
    accept_states = list(accept_states)

    file = open(file,'w')
    file.write(f'{n} {alphabet_size} {len(accept_states)}\n')


    for row in automaton_to_write:
        for i in range(len(row)):
            if i < alphabet_size-1:
                file.write(str(row[i]) + ' ')
            else:
                file.write(str(row[i]) + '\n')

    for i in range(len(accept_states)):
        if i < len(accept_states) - 1:
            file.write(str(accept_states[i]) + ' ')
        else:
            file.write(str(accept_states[i]) + '\n')

    result_string = ''

    for i in range(string_size):
        result_string += 'b'

    file.write(result_string)

    file.close()




def generate_automata(alphabet_size,string_size,n,file):
    accept_states =  1
    automaton_to_write = [[-1 for j in range(alphabet_size)] for i in range(n)]
    accept_states = [0]

    automaton_to_write[0][1] = 0 
    automaton_to_write[0][0] = 1


    file = open(file,'w')
    file.write(f'{n} {alphabet_size} {len(accept_states)}\n')


    for row in automaton_to_write:
        for i in range(len(row)):
            if i < alphabet_size-1:
                file.write(str(row[i]) + ' ')
            else:
                file.write(str(row[i]) + '\n')

    for i in range(len(accept_states)):
        if i < len(accept_states) - 1:
            file.write(str(accept_states[i]) + ' ')
        else:
            file.write(str(accept_states[i]) + '\n')

    result_string = ''

    for i in range(string_size):
        result_string += 'b'

    file.write(result_string)

    file.close()

states = [10, 100, 1000]
string_sizes = [10,100,1_000,10_000,100_000,1_000_000,10_000_000]


for n_state in states:
    for size in string_sizes:
        create_random_automata(24,size,n_state,f'{n_state}q_{size}c.txt')