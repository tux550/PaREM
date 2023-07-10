import subprocess


states = [10,100,1000]
sizes = [100_000,500_000,1_000_000,5_000_000,10_000_000,50_000_000]
#[10,100,1000,10_000,100_000,1_000_000,10_000_000]
reps = 5 
p = [1,2,3,4,5,6,7,8,9,10,11,12]
par_exe = "../rem/parem.exe"

file = open('tiempos_par.csv','w')

file.write('procesos,q,longitud,tiempo_tot,tiempo_comm,tiempo_ejec\n')

for n_state in states:
    for size in sizes:
        automata_filename = f"../generate_automatas/{n_state}q_{size}c.txt"
        print(f"Running: {size},{n_state}")
        print(f"File: {automata_filename}")
        for p_ in p:
            print(f"P:{p_}")
            if p_ > size:
                continue
            total_time = 0
            total_comm_time = 0
            total_exe_time = 0
            cmd = f'mpirun -np {p_} {par_exe} {automata_filename}'
            print(cmd)
            for _ in range(reps):
                time,comm_time,exe_time = subprocess.check_output(cmd,shell=True).decode().split(',')
                total_time      += float(time)
                total_comm_time += float(comm_time)
                total_exe_time  += float(exe_time)
            total_time      /= reps
            total_comm_time /= reps
            total_exe_time  /= reps

            file.write(f'{p_},{n_state},{size},{total_time},{total_comm_time},{total_exe_time}\n')



file.close()




