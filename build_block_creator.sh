#!/bin/bash
#-----------------------------------------------------------------------------#
main()
{
    echo "Primero creamos la carpeta out si no existe"
    mkdir -p out
    echo "Ahora buildearemos block creator"
    cd "block_creator/Debug"
    make
    echo "Buildeado!. Ahora lo copiamos a la carpeta de out"
    cp block_creator ../../out
    echo "Listo. Finalmente un clean"
    make clean
    cd "../.."
    echo "Script finalizado!"
}

#-----------------------------------------------------------------------------#
main
