import random

alphabet_size = 24
string_size = 1_000_000
n = 100
accept_states =  1
automaton_to_write = [[-1 for j in range(alphabet_size)] for i in range(n)]
accept_states = [0]

automaton_to_write[0][1] = 0 
automaton_to_write[0][0] = 1


file = open('result.txt','w')
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

