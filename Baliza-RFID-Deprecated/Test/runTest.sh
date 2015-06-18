#!/bin/bash
#Script que ejecuta el test
#Antes de ejecutar el test, como se requiere interacción por parte del usuario con el lector RFID, el usuario debe ser consciente de a que distancia debe pasar la tarjeta como mínimo para que el lector la pueda reconocer correctamente. 

if [ "$#" != 1 ]
then
    echo "Número de parámetros incorrecto"
    echo "Ejemplo: ./runTest.sh ttyUSB0"
fi

if [ -f ./test = 0]
then
   make
   chmod +x ./test
fi

./test "$1"
