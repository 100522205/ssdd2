import subprocess
import time # para el timeout 10s 
import sys

cmd=["./cliente"]
timeout = 10 # tiempo máximo

def borra_procesos(pr):
    """
    solucionar problema de max recursos
    NOTA: no soluciona, solo parcialmente
    """

    for p in pr:
        try:
            p.terminate()
        except Exception:
            pass

def test(k):
    """
    funcion para ejecutar k clientes. Solo triunfa si todas terminan
    en menos de 10s sin fallar
    """

    pr=[]

    for i in range(k):
        try:
            p=subprocess.Popen(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            pr.append(p)
        except OSError as e: # parcheamos bug por falta de recuros del so
            print("Límite del SO")
            borra_procesos(pr)
            return False

    # ahora que están ejecutando, esperamos

    ahora = time.time()
    exito = True
    for p in pr:
        if not exito: # si 1 falla no espramos
            break
        t = time.time()-ahora
        tr = max(0, timeout-t)

        try:
            cod = p.wait(timeout=tr)
            if cod != 0:
                exito = False
        except subprocess.TimeoutExpired: # mas del tiempo
            exito = False

    # limpiar antes de terminar
    borra_procesos(pr)
    return exito
    
def main():
    print("PRUEBAS DE CARGA")
    print("MAXIMOS CLIENTES EN 10 segundos")

    k=1
    k_bueno = 0
    k_malo=0

    print("Crecimiento exponencial")
    encontrado_abajo = 0
    while not encontrado_abajo:
        print("Probando con k = "+str(k))
        sys.stdout.flush() # print ANTES

        if test(k): # funcionó
            print("Éxito con K = "+str(k))
            k_bueno=k
            k*=2
            time.sleep(0.5) 
        else:
            print("Fallo")
            k_malo=k
            time.sleep(0.5) 
            encontrado_abajo=1

    print("Busqueda binaria")

    lmin = k_bueno+1
    lmax = k_malo-1
    mejor = k_bueno

    while lmin<=lmax: # busqueda binaria del mayor exitoso
        mitad=(lmax+lmin)//2
        print("Probando con K = "+ str(mitad))
        sys.stdout.flush()

        if test(mitad):
            print("Éxito con K = "+ str(mitad))
            mejor = mitad
            lmin = mitad+1
        else:
            print("Fallo")
            lmax=mitad-1
        time.sleep(0.5)

    print("Resultado final: Máximo en "+ str(mejor) + " ejecuciones concurrentes de cliente")


if __name__ == '__main__':
    main()

