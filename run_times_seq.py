import subprocess

states =[10,100,1000]
sizes = [10,100,1000,10_000,100_000,1_000_000,10_000_000]
reps = 5 
seq_exe = "rem/rem.exe"

file = open('tiempos_seq.csv','w')
file.write('q,longitud,tiempo_ejec\n') # Headers

for n_state in states:
    for size in sizes:
        total_exe_time = 0
        automata_filename = f"./generate_automatas/{n_state}q_{size}c.txt"
        print(f"Running: {size},{n_state}")
        print(f"File: {automata_filename}")
        for _ in range(reps):
            exe_time = subprocess.check_output(f'{seq_exe} {automata_filename}',shell=True).decode()
            total_exe_time+= float(exe_time)
        total_exe_time /= reps
        file.write(f'{n_state},{size},{total_exe_time}\n')
file.close()




