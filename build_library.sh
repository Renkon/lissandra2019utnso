#!/bin/bash
#-----------------------------------------------------------------------------#
main()
{
    echo "Primero creamos la carpeta out si no existe"
    mkdir -p out
    echo "Ahora buildearemos la lib"
    cd "lib/Debug"
    make
    echo "Buildeado!. Ahora lo copiamos a la carpeta de out"
    cp liblib.a ../../out
    echo "Listo. Finalmente un clean"
    make clean
    cd "../.."
    echo "Script finalizado!"
}

#-----------------------------------------------------------------------------#
main
