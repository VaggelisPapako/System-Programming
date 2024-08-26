#!/bin/bash

quit=0
echo "partiesFile:$1 numLines:$2"
if [ -r "$1" ] && [ -s "$1" ]
then 
    echo partiesFile eligible
else
    echo partiesFile not eligible to be read
    quit=1
fi

case $2 in
    ''|*[!0-9]*) 
    echo "not good input for numlines" 
    ;;
    *) 
    echo good input for numlines
    ;;
esac

let parties=$(wc -l < $1)
echo $parties

if [ "$quit" -eq "1" ]
then
    echo quitting
    exit 1
fi

touch inputFile
rm inputFile
touch inputFile

declare -a partiesArray

mapfile -t partiesArray < $1

for ((j=0;j<$2;j++))
do

    #pick a random party
    rand=$((RANDOM % $parties + 1))
    party=${partiesArray[$rand]//[$'\t\r\n ']}
    #create random letter name-surname
    nameLetters=$((RANDOM % 12 + 1))
    if [ "$nameLetters" -lt "3" ]
    then
        nameLetters=3
    fi
    surnameLetters=$((RANDOM % 12 + 1))
    if [ "$surnameLetters" -lt "3" ]
    then
        surnameLetters=3
    fi
    name=$(cat /dev/urandom | tr -dc 'a-zA' | fold -w $nameLetters | head -n 1)
    last=$(cat /dev/urandom | tr -dc 'a-zA' | fold -w $surnameLetters | head -n 1)
    person="$name $last $party"
    echo $person >> inputFile
done