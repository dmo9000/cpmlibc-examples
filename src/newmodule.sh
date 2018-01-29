#!/bin/sh

MODNAME=$1

[ -z "$MODNAME" ] && exit 1

MODNAME_LOWER=`echo $MODNAME | tr "[:upper:]" "[:lower:]"`
MODNAME_UPPER=`echo $MODNAME | tr "[:lower:]" "[:upper:]"`

cp -rfp stub $MODNAME_LOWER
sed -i "s/stub/$MODNAME_LOWER/g" $MODNAME_LOWER/*
sed -i "s/STUB/$MODNAME_UPPER/g" $MODNAME_LOWER/*
mv $MODNAME_LOWER/stub.c $MODNAME_LOWER/$MODNAME_LOWER.c
mv $MODNAME_LOWER/stub.mk $MODNAME_LOWER/$MODNAME_LOWER.mk

