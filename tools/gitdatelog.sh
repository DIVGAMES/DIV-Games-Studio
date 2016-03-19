#!/bin/bash

# Generates git changelog grouped by day
#
# optional parameters
# -a, --author       to filter by author
# -s, --since        to select start date
# -u, --until        to select end date

git-log-by-day () {
  local NEXT=$(date +%Y-%m-%d)


#  local RED="\x1B[31m"
#  local YELLOW="\x1B[32m"
#  local BLUE="\x1B[34m"
#  local RESET="\x1B[0m"

  local SINCE=$(date +%Y-%m-%d --date='1 month ago')
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

# git log --no-merges --since="${SINCE}" --until="${UNTIL}" --format="%cd" --date=short | sort -u | while read DATE ; do
 git log --no-merges --since="${SINCE}" --until="${UNTIL}" --format="%cd" --date=short | sort -u -r | while read DATE ; do

    local GIT_PAGER=$(git log --no-merges --format="${LOG_FORMAT}" --since="${DATE} 00:00:00" --until="${DATE} 23:59:59" --author="${AUTHOR}")

    if [ ! -z "$GIT_PAGER" ]
    then
      echo
      echo -e "${RED}[$DATE]${RESET}"
      echo -e "${GIT_PAGER}"
    fi

  done
}
echo "What's NEW"
echo "----------"
git-log-by-day
