from glob import glob
from os.path import split

# print(glob("graficos/*.png"))

rutas = glob("graficos/*.png")

cad = '''\\begin{{figure}}[H]
    \centerline{{
        \includegraphics[width=0.4\\textwidth]{{{ruta}}}
    }}
    \caption{{{capcion}}}
    \label{{grafico:{etiqueta}}}
\end{{figure}}\n'''

with open("latex.tex", 'w') as f:
    for ruta in rutas:
        _, tail = split(ruta)
        tail = tail.split('.')[0]
        f.write(cad.format(ruta = ruta, capcion="Ingresar Capción", etiqueta=tail))
    pass