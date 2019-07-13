#!/bin/bash
#-----------------------------------------------------------------------------#
main()
{
    echo "Primero creamos la carpeta out si no existe"
    mkdir -p out
    echo "Ahora buildearemos la lib"
    cd "lib/Debug"
    make
    echo "Lib creada. Siguiente paso, buildear KERNEL"
    cd "../../kernel/Debug"
    make
    echo "Kernel creado, sacando binario y limpiando"
    cp kernel "../../out"
    make clean
    echo "Proximo paso, crear filesystem"
    cd "../../filesystem/Debug"
    make
    echo "Filesystem creado, sacando binario y limpiando"
    cp filesystem "../../out"
    make clean
    echo "Paso final, crear memoria"
    cd "../../memoria/Debug"
    make
    echo "Memoria creada, sacando binario y limpiando"
    cp memoria "../../out"
    make clean
    echo "Limpiando lib"
    cd "../../lib/Debug"
    make clean
    cd "../.."
    echo "Done!"
}

#-----------------------------------------------------------------------------#
main
