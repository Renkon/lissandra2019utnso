#!/bin/bash
#-----------------------------------------------------------------------------#
main()
{
    if [ "$#" -ne 4 ]; then
        echo "Illegal number of parameters"
    fi

    blocks="${1}"
    blockSize="${2}"
    magicNumber="${3}"
    mountPoint="${4}"

    echo "Se creara metadata de filesystem con los siguientes datos"
    echo " - ${blocks} de tamaño ${blockSize} - MagicNumber ${magicNumber}"
    echo " - MountPoint: ${mountPoint}"

    mkdir -p "${mountPoint}"
    mkdir -p "${mountPoint}/Bloques"
    mkdir -p "${mountPoint}/Metadata"
    mkdir -p "${mountPoint}/Tables"

    echo "Carpetas creadas.. ejecutando binario"
    ./out/block_creator "${blocks}" "${blockSize}" "${magicNumber}" "${mountPoint}"
    echo "Se ejecuto el script y se deberia haber creado la carpeta"    
    echo "Si ven que fallo el binario, seguramente es porque no lo buildearon!!!!"
}

#-----------------------------------------------------------------------------#
main "$@"
