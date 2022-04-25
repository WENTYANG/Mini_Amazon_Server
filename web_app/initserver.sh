#!/bin/bash
python3 manage.py makemigrations user
python3 manage.py makemigrations amazon
python3 manage.py flush
python3 manage.py migrate
res="$?"
while [ "$res" != "0" ]
do
    sleep 3;
    python3 manage.py migrate
    res="$?"
done

