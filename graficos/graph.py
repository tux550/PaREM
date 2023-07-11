import pandas as pd
import matplotlib.pyplot as plt

t_par = pd.read_csv("../tests/tiempos_par.csv") #../tests/
t_seq = pd.read_csv("../tests/tiempos_seq.csv") #../tests/


qs = t_seq.q.unique()
ns = t_seq.longitud.unique()
ps = [2,4,8,12]#t_par.procesos.unique()

print("q",qs)
print("n",ns)
print("p",ps)


#Colors
seq_color = "red"
par_colors = ['black', 'blue', 'brown', 'green']


show = False
def show_plt():
    if show:
        plt.show()

for q in qs:
    # Extract
    sequential = t_seq[t_seq.q == q][["longitud","tiempo_ejec"]]
    parallels = []
    for p in ps:
        parallel_p =  t_par[t_par.q == q]
        parallel_p = parallel_p[parallel_p.procesos == p]
        parallel_p = parallel_p[["longitud","tiempo_tot","tiempo_comm","tiempo_ejec"]]
        # ADD SPEEDUP
        tmp = parallel_p[["longitud","tiempo_tot"]].merge(sequential[["longitud","tiempo_ejec"]], on=["longitud",], how='left')
        tmp["speedup"] = tmp["tiempo_ejec"]/tmp["tiempo_tot"]
        tmp = tmp [["longitud","speedup"]]
        parallel_p = parallel_p.merge(tmp[["longitud","speedup"]], on=["longitud",], how='left')
        # ADD EFICENCY
        parallel_p["efficency"] = parallel_p["speedup"]/p
        #Append
        parallels.append( (p, parallel_p) )
    #Plot
    plt.title(f"Tiempos (s) Totales q={q}")
    plt.plot(sequential.longitud, sequential.tiempo_ejec, marker="o", color=seq_color, label="Sequential")
    for i, (p, parallel_p) in enumerate( parallels ):
        plt.plot(parallel_p.longitud, parallel_p.tiempo_tot,marker="o", color=par_colors[i],label=f"MPI:p={p}")
    plt.legend()
    plt.xlabel("Longitud de cadena $n$")
    plt.ylabel(f"$T_{{Total}}$ (s)")
    show_plt()
    plt.savefig(f'tiempos_totales_q{q}.png')
    plt.clf()

    plt.title(f"Tiempos (s) Ejecución q={q}")
    plt.plot(sequential.longitud, sequential.tiempo_ejec, marker="o", color=seq_color, label="Sequential")
    for i, (p, parallel_p) in enumerate(parallels):
        plt.plot(parallel_p.longitud, parallel_p.tiempo_ejec, marker="o", color=par_colors[i],label=f"MPI:p={p}")
    plt.legend()
    plt.xlabel("Longitud de cadena $n$")
    plt.ylabel(f"$T_{{Comp}}$ (s)")
    show_plt()
    plt.savefig(f'tiempos_ejecucion_q{q}.png')
    plt.clf()
    
    plt.title(f"Tiempos (s) Comunicación q={q}")
    for i, (p, parallel_p) in enumerate(parallels):
        plt.plot(parallel_p.longitud, parallel_p.tiempo_comm, marker="o", color=par_colors[i],label=f"MPI:p={p}")
    plt.legend()
    plt.xlabel("Longitud de cadena $n$")
    plt.ylabel(f"$T_{{Comm}}$ (s)")
    show_plt()
    plt.savefig(f'tiempos_comunicacion_q{q}.png')
    plt.clf()

    plt.figure(figsize=(8, 6))
    plt.title(f"Tiempos (s) en Paralelo q={q}")
    for i,(p, parallel_p) in enumerate(parallels):
        plt.plot(parallel_p.longitud, parallel_p.tiempo_tot, marker="o", color=par_colors[i],label=f"MPI:p={p}")
        plt.plot(parallel_p.longitud, parallel_p.tiempo_ejec,"--", marker="o", color=par_colors[i],label=f"MPI:p={p} $T_{{Comp}}$") # Dashed para tiempo de ejecucion
        plt.plot(parallel_p.longitud, parallel_p.tiempo_comm,":", marker="o", color=par_colors[i],label=f"MPI:p={p} $T_{{Comm}}$") # Dotted para tiempo de comm
    plt.legend()
    plt.xlabel("Longitud de cadena $n$")
    plt.ylabel("Tiempo (s)")
    show_plt()
    plt.savefig(f'tiempos_paralelo_q{q}.png')
    plt.clf()

    # Plot speed up
    plt.title(f"Speedup q={q}")
    for i, (p, parallel_p) in enumerate( parallels ):
        plt.plot(parallel_p.longitud, parallel_p.speedup,marker="o", color=par_colors[i],label=f"MPI:p={p}")
    plt.legend()
    plt.xlabel("Longitud de cadena $n$")
    plt.ylabel("Speedup")
    show_plt()
    plt.savefig(f'speedup_q{q}.png')
    plt.clf()

    # Plot efficency
    plt.title(f"Efficency q={q}")
    for i, (p, parallel_p) in enumerate( parallels ):
        plt.plot(parallel_p.longitud, parallel_p.speedup,marker="o", color=par_colors[i],label=f"MPI:p={p}")
    plt.legend()
    plt.xlabel("Longitud de cadena $n$")
    plt.ylabel("Eficiencia")
    show_plt()
    plt.savefig(f'efficency_q{q}.png')
    plt.clf()
