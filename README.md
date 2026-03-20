# ssdd2
Sistemas Distribuidos 2

## Autoría:
Jose Barrio, Juan Mayoral.
G.81, Mar-Abr 2026

## Uso

* Compilado:
Primero, realice: make clean
Luego:            make
Finalmente, para probar los tests con concurrencia:
    ./servidor_mq & sleep 2 ; ./cliente_mq ; 
    sleep 1 ; ./cliente_mq
    
    o
    ./servidor_mq & sleep 2 ; ./cliente_mq ; 
    ./cliente_mq

