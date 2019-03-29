#!/usr/bin/env bash
scp -rp -P 26534  commons.py onurtirt@pc2.instageni.rnoc.gatech.edu:~/ &
scp -rp -P 26530  commons.py onurtirt@pc2.instageni.rnoc.gatech.edu:~/ &
scp -rp -P 26531  commons.py onurtirt@pc2.instageni.rnoc.gatech.edu:~/ &

wait
