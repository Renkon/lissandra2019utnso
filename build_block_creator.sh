#!/bin/bash
#-----------------------------------------------------------------------------#
main()
{
    echo "Buildearemos block creator"
    cd "block_creator/Debug"
    make
    cd "../.."
    echo "Hecho!"
}

#-----------------------------------------------------------------------------#
main