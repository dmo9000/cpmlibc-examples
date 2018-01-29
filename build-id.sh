#!/bin/sh

SEQ=0000

THIS_BUILD_ID=`printf "%s%04d" \`date "+%Y%m%d"\` ${SEQ}`

if [ -r build-id ]; then
    LAST_BUILD_ID=`cat build-id`
    LAST_DATE=`echo $LAST_BUILD_ID | cut -b1-8`
    THIS_DATE=`echo $THIS_BUILD_ID | cut -b1-8`
    if [ $LAST_BUILD_ID -lt $THIS_BUILD_ID ]; then
        # if the old build number is already lower than our new one, a new day has sprung!
        # we can safely just bump the build id and write a new header file
        echo "$THIS_BUILD_ID" > build-id
        echo "#define BUILD_ID \"$THIS_BUILD_ID\"" > build-id.h
        exit 0
        fi
    while [ $THIS_BUILD_ID -le $LAST_BUILD_ID ]; do
        # THIS_BUILD_ID needs to be higher than the old one before we can write it 
        # this should really just roll forward to the highest numbered build for the day
        let THIS_BUILD_ID=THIS_BUILD_ID+1   
        done 
    # write it out 
    echo "$THIS_BUILD_ID" > build-id
    echo "#define BUILD_ID \"$THIS_BUILD_ID\"" > build-id.h
    exit 0
   else
    # build-id file does not exist, create and seed it
    echo "$THIS_BUILD_ID" > build-id
    echo "#define BUILD_ID \"$THIS_BUILD_ID\"" > build-id.h
   fi   

