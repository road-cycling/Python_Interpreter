#/bin/bash

NC='\033[0m'
RED='\033[0;31m'
Green='\033[0;32m'

echo "Starting Test Script"
echo

cd $(dirname $0)

for file in $(ls | grep -v *.sh); do 
    NATHAN_INTERPRETER=$(timeout 2 ../statement.x $file)
    # NATHAN_INTERPRETER=$(../statement.x $file)
    if [[ $? -ne 0 ]]; then 
        echo -e "${RED}ERROR - $file FAIL on Timeout${NC}"
        continue;
    fi

    PYTHON_INTERPRETER=$(cat $file | python2.7)
    DIFF=$(diff <( echo $NATHAN_INTERPRETER ) <( echo $PYTHON_INTERPRETER ))


    if [[ -n $DIFF ]]; then  # Diff is not empty
        echo -e "${RED}Test failed on file $file${NC}"
        echo "-----------------"
        echo "Your Output"
        echo $NATHAN_INTERPRETER 
        echo "-----------------"
        echo "Expected Output"
        echo $PYTHON_INTERPRETER
        echo "----------------"
        echo "-- Diff --"
        echo $DIFF
        echo 
    elif [[ -z $DIFF ]]; then    
        echo -e "${Green}$file passes${NC}"
    fi

done

echo
echo "Finished Testing"