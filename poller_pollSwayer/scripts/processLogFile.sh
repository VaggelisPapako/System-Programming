#!/bin/bash

quit=0
echo "poll-log:$1"
if [ -r "$1" ] && [ -s "$1" ]; then
    echo poll-log eligible
else
    echo poll-log not eligible to be read
    exit
fi
touch pollerResultsFile
rm pollerResultsFile
touch pollerResultsFile
declare -a votesArray
declare -A party_counts  # Associative array to store party vote counts
# new
declare -A voted_voters  # Associative array to track voted voters

mapfile -t votesArray < "$1" # Used -t to remove newlines

for vote in "${votesArray[@]}"; do
    name=$(echo "$vote" | awk '{print $1}')
    surname=$(echo "$vote" | awk '{print $2}')
    party=$(echo "$vote" | awk '{print $3}')

    # Check if voter has already voted, if yes, skip counting
    voter_id="$name $surname"
    if [[ -v voted_voters["$voter_id"] ]]; then # Check if this voter has been assigned a value (meaning has already voted)
        echo "$voter_id has already voted"
        continue
    else
        voted_voters["$voter_id"]=1
    fi    

    # Increment party vote count
    if [[ -n "$party" ]]; then
        ((party_counts["$party"]++))
    fi
done

for party in "${!party_counts[@]}"; do
    echo "Party: $party | Votes: ${party_counts["$party"]}"
    echo "Party: $party | Votes: ${party_counts["$party"]}" >> pollerResultsFile
done