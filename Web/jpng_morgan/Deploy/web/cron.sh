#!/bin/bash

if [ $(ls -1 /uploads/ | wc -l) -gt 5 ]; then
    rm -rf /uploads/*
fi
