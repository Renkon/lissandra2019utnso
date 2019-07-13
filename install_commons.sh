#!/bin/bash
#-----------------------------------------------------------------------------#
repositoryUrl="https://github.com/sisoputnfrba/so-commons-library.git"

main()
{
    echo "Iniciando clone de GitHub"
    git clone "${repositoryUrl}"
    echo "Se descargaron las Commons en la carpeta"
    echo "Ahora procedere a ir a so-commons-library"
    cd "so-commons-library"
    echo "Ahora hare el make install (solicitara credenciales para sudo)"
    sudo make install
    echo "Hemos terminado la instalacion de las commons"
    cd ..
    echo "Si lees esto, es porque estas leyendo esto"
}

#-----------------------------------------------------------------------------#
main
