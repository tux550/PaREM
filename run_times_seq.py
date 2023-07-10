import subprocess

states =[10,100,1000]
sizes = [10,100,1000,10_000,100_000,1_000_000,10_000_000]
reps = 5 

file = open('tiempos_seq.csv','w')

file.write('q,longitud,tiempo_ejec\n')


for n_state in states:
    for size in sizes:
        print(size,n_state)
        subprocess.run(f'g++ paremsingle.cpp -o exe -Dinput_path=\\"./generate_automatas/{n_state}q_{size}c.txt\\"',
                    shell=True)
        total_exe_time = 0
        for _ in range(reps):
            exe_time = subprocess.check_output(f'./exe',shell=True).decode()

            total_exe_time+= float(exe_time)

        total_exe_time /= reps

        file.write(f'{n_state},{size},{total_exe_time}\n')



file.close()




