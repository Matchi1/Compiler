#!/bin/bash

echo "Compile the compilator"
make
echo -e "\n\n"

#Initialisation des variables de chemins
exec="bin/tpcc"
repertoire="test"
good=$repertoire"/good"
semerr=$repertoire"/sem-err"
synerr=$repertoire"/syn-err"
warn=$repertoire"/warn"
resultat="resultat.log"
rm -f $resultat #Suppression anciens résultats

#Tests des positifs
for i in $(ls $good)
do
	echo "./$exec < "$i
    ./$exec < $good/$i
	# renvoie le résultat dans le fichier 'resultat' sous la
	# forme 'nom de fichier' 'résultat de l'analyse du fichier (0 ou 1)'
    echo "Résultat attendus pour "$i": 0, résultat obtenu: "$? >> $resultat
done

#Tests des positifs
for i in $(ls $semerr)
do
	echo "./$exec < "$i
    ./$exec < $semerr/$i
	# renvoie le résultat dans le fichier 'resultat' sous la
	# forme 'nom de fichier' 'résultat de l'analyse du fichier (0 ou 1)'
    echo "Résultat attendus pour "$i": 0, résultat obtenu: "$? >> $resultat
done

#Tests des positifs
for i in $(ls $synerr)
do
	echo "./$exec < "$i
    ./$exec < $synerr/$i
	# renvoie le résultat dans le fichier 'resultat' sous la
	# forme 'nom de fichier' 'résultat de l'analyse du fichier (0 ou 1)'
    echo "Résultat attendus pour "$i": 0, résultat obtenu: "$? >> $resultat
done
echo "Les Résultats des tests sont disponible dans le fichier $resultat\n"

#Tests des positifs
for i in $(ls $warn)
do
	echo "./$exec < "$i
    ./$exec < $warn/$i
	# renvoie le résultat dans le fichier 'resultat' sous la
	# forme 'nom de fichier' 'résultat de l'analyse du fichier (0 ou 1)'
    echo "Résultat attendus pour "$i": 0, résultat obtenu: "$? >> $resultat
done
