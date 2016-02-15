# Reverse Polish Notation Calculator
#
# Copyright (C) 2007
# Center for Perceptual Systems
# University of Texas at Austin
#
# jsp Mon Feb 26 17:51:04 CST 2007

MAJOR_VERSION=`grep MAJOR_VERSION version.h | cut -d' ' -f5 -`
MINOR_VERSION=`grep MINOR_VERSION version.h | cut -d' ' -f5 -`
RELEASE=1
VERSION=$(MAJOR_VERSION).$(MINOR_VERSION).$(RELEASE)

TARGET=rpn

include ../Makefile.app

INCLUDEPATH=../horny_toad
DEPENDPATH=../horny_toad
EXTRA_SOURCES=../horny_toad/argv.cc
LIBS=

check: all
	echo "help quit" | ./rpn --basic
	echo "help quit" | ./rpn --hp35
	echo "help quit" | ./rpn --super
