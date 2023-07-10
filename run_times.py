import subprocess


sizes = [10,100,1000,10_000,100_000,1_000_000]
reps = 5 
p = [2,4,8,12]

file = open('tiempos.csv','w')

file.write('procesos,q,longitud,tiempo_tot,tiempo_comm,tiempo_ejec\n')

for size in sizes:

    print(size)
    subprocess.run(f'mpic++ parem.cpp -o exe.out -Dinput_path=\\"./test_automatas/n_{size}/accept.txt\\"',
                   shell=True)
    
    for p_ in p:

        total_time = 0
        total_comm_time = 0
        total_exe_time = 0

        for _ in range(reps):
            time,comm_time,exe_time = subprocess.check_output(f'mpirun -np {p_} ./exe.out',shell=True).decode().split(',')

            total_time += float(time)
            total_comm_time += float(comm_time)
            total_exe_time+= float(exe_time)

        total_time /= reps
        total_comm_time /= reps
        total_exe_time /= reps

        file.write(f'{p_},{size},{size},{total_time},{total_comm_time},{total_exe_time}\n')

file.close()




