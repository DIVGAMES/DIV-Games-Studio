#!/bin/bash

# Generates git changelog grouped by day
#
# optional parameters
# -a, --author       to filter by author
# -s, --since        to select start date
# -u, --until        to select end date
AGO="12 months ago"

if [ ! -z "$1" ] 
then
AGO=$1
else
echo "What's NEW"
echo "----------"
fi

array=()

git-log-by-day () {
  local NEXT=$(date +%Y-%m-%d)


#  local RED="\x1B[31m"
#  local YELLOW="\x1B[32m"
#  local BLUE="\x1B[34m"
#  local RESET="\x1B[0m"

  local SINCE=$(date +%Y-%m-%d --date="${AGO}")
  local UNTIL=$NEXT

  for i in "$@"
  do
  case $i in
    -a=*|--author=*)
    local AUTHOR="${i#*=}"
    shift
    ;;
    -s=*|--since=*)
    SINCE="${i#*=}"
    shift
    ;;
    -u=*|--until=*)
    UNTIL="${i#*=}"
    shift
    ;;
    *)
      # unknown option
    ;;
  esac
  done

#  local LOG_FORMAT=" %Cgreen*%Creset %s"
   local LOG_FORMAT=" * %s"
  
  if [ -z "$AUTHOR" ]
  then
    LOG_FORMAT="$LOG_FORMAT [%an]"
  else
    echo 
    echo -e "${BLUE}Logs filtered by author: ${AUTHOR}${RESET}"
  fi

#echo  git log --no-merges --since="${SINCE}" --until="${UNTIL}" --format="%cd" --date=short | sort -u 
#exit;
n=0
# git log --no-merges --since="${SINCE}" --until="${UNTIL}" --format="%cd" --date=short | sort -u | while read DATE ; do
 git log --no-merges 9bc29cd6b238a64904fa7fbd4b99e682cc91f664.. --format="%cd" --date=short | sort -u -r | while read DATE ; do
    array+=("${DATE}")
    echo "${n} = " ${array[$n]}
    let n+=1
#    echo ${n}
    arr=${array}
    echo ${arr}

    echo ${array[@]}
 done
# unset array[${#array[@]}-1]
echo "arse"
echo ${arr[@]}
 for DATE in ${arr[@]} ; do
 echo "arse"
 #exit
  echo ${DATE}
    local GIT_PAGER=$(git log --no-merges --format="${LOG_FORMAT}" --since="${DATE} 00:00:00" --until="${DATE} 23:59:59" --author="${AUTHOR}")

    if [ ! -z "$GIT_PAGER" ]
    then
      echo
      echo -e "${RED}[$DATE]${RESET}"
      echo -e "${GIT_PAGER}"
    fi

  done
}
git-log-by-day
