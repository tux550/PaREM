import pandas as pd
import matplotlib.pyplot as plt

t_par = pd.read_csv("../tests/tiempos_par.csv") #../tests/
t_seq = pd.read_csv("../tests/tiempos_seq.csv") #../tests/


qs = t_seq.q.unique()
ns = t_seq.longitud.unique()
ps = [2,4,8,12]#t_par.procesos.unique()

print(qs,ns,ps)


#Colors
seq_color = "red"
par_colors = ['black', 'blue', 'brown', 'green']

for q in qs:
    # Extract
    sequential = t_seq[t_seq.q == q][["longitud","tiempo_ejec"]]
    parallels = []
    for p in ps:
        parallel_p =  t_par[t_par.q == q][t_par.procesos == p]
        parallel_p = parallel_p[["longitud","tiempo_tot","tiempo_comm","tiempo_ejec"]]
        parallels.append( (p, parallel_p) )
    #Plot
    plt.title("Tiempos Totales")
    plt.plot(sequential.longitud, sequential.tiempo_ejec, marker="o", color=seq_color)
    for i, (p, parallel_p) in enumerate( parallels ):
        print(parallel_p)
        plt.plot(parallel_p.longitud, parallel_p.tiempo_tot,marker="o", color=par_colors[i])
    plt.show()

    plt.title("Tiempos Ejecucción")
    plt.plot(sequential.longitud, sequential.tiempo_ejec, marker="o", color=seq_color)
    for i, (p, parallel_p) in enumerate(parallels):
        plt.plot(parallel_p.longitud, parallel_p.tiempo_ejec, marker="o", color=par_colors[i])
    plt.show()

    plt.title("Tiempos Comunicación")
    for i, (p, parallel_p) in enumerate(parallels):
        plt.plot(parallel_p.longitud, parallel_p.tiempo_comm, marker="o", color=par_colors[i])
    plt.show()


    plt.title("Tiempos en Paralelo")
    for i,(p, parallel_p) in enumerate(parallels):
        plt.plot(parallel_p.longitud, parallel_p.tiempo_tot, marker="o", color=par_colors[i])
        plt.plot(parallel_p.longitud, parallel_p.tiempo_ejec,"--", marker="o", color=par_colors[i]) # Dashed para tiempo de ejecucion
        plt.plot(parallel_p.longitud, parallel_p.tiempo_comm,":", marker="o", color=par_colors[i]) # Dotted para tiempo de comm
    plt.show()


    break
