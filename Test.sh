#!/bin/bash

# Init path variables
exec="bin/tpcc"
dir="test"
good=$dir"/good"
semerr=$dir"/sem-err"
synerr=$dir"/syn-err"
warn=$dir"/warn"
result="resultat.log"
rm -f $result # Delete old result file

if test -e $exec; then
	echo "No need to compile"
else
	echo "Compiling the compilator"
	make
fi
echo -e "\n\n"

# Good tests
for i in $(ls $good)
do
	echo "./$exec < $good/$i"
    ./$exec < $good/$i
	# renvoie le résultat dans le fichier 'resultat' sous la
	# forme 'nom de fichier' 'résultat de l'analyse du fichier (0 ou 1)'
    echo "Résultat attendus pour "$i": 0, résultat obtenu: "$? >> $result
done

# Semantic error tests
for i in $(ls $semerr)
do
	echo "./$exec < $semerr/$i"
    ./$exec < $semerr/$i
	# renvoie le résultat dans le fichier 'resultat' sous la
	# forme 'nom de fichier' 'résultat de l'analyse du fichier (0 ou 1)'
    echo "Résultat attendus pour "$i": 1, résultat obtenu: "$? >> $result
done

# Syntax error tests
for i in $(ls $synerr)
do
	echo "./$exec < $synerr/$i"
    ./$exec < $synerr/$i
	# renvoie le résultat dans le fichier 'resultat' sous la
	# forme 'nom de fichier' 'résultat de l'analyse du fichier (0 ou 1)'
    echo "Résultat attendus pour "$i": 2, résultat obtenu: "$? >> $result
done

# Warning tests
for i in $(ls $warn)
do
	echo "./$exec < $warn/$i"
    ./$exec < $warn/$i
	# renvoie le résultat dans le fichier 'resultat' sous la
	# forme 'nom de fichier' 'résultat de l'analyse du fichier (0 ou 1)'
    echo "Résultat attendus pour "$i": 3, résultat obtenu: "$? >> $result
done

echo "Les Résultats des tests sont disponible dans le fichier $result"
