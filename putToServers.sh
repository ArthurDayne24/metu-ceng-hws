#!/usr/bin/env bash
echo Starting all transmissions at once

scp -rp -P 26534 input.txt \
    onurtirt@pc2.instageni.rnoc.gatech.edu:~/ &

scp -rp -P 26531 input.txt \
    onurtirt@pc2.instageni.rnoc.gatech.edu:~/ &

scp -rp -P 26534 SourceNode.py commons.py setntp.sh \
    onurtirt@pc2.instageni.rnoc.gatech.edu:~/ &

scp -rp -P 26530 BrokerNode.py experimental_broker.sh commons.py configAtBroker.sh experimental.sh \
    onurtirt@pc2.instageni.rnoc.gatech.edu:~/ &

scp -rp -P 26532 experimental.sh experimental_router1.sh \
    onurtirt@pc2.instageni.rnoc.gatech.edu:~/ &

scp -rp -P 26533 experimental.sh experimental_router2.sh \
    onurtirt@pc2.instageni.rnoc.gatech.edu:~/ &

scp -rp -P 26531 DestinationNode.py commons.py configAtDest.sh setntp.sh experimental.sh experimental_destination.sh \
    onurtirt@pc2.instageni.rnoc.gatech.edu:~/ &

wait
