#!/bin/bash
#-----------------------------------------------------------------------------#
main()
{
    echo "Ahora procedere a ir a so-commons-library"
    cd "so-commons-library"
    echo "Ahora hare el make uninstall (solicitara credenciales para sudo)"
    sudo make uninstall
    echo "Hemos terminado la desinstalacion de las commons"
    cd ..
    echo "Ahora simplemente borraremos la carpeta, y listo"
    sudo rm -rf "so-commons-library"
    echo "Done!"
}

#-----------------------------------------------------------------------------#
main

