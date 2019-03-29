import argparse

parser = argparse.ArgumentParser()
add_arg = parser.add_argument

add_arg('--queries', default='validation_queries.dat', type=str,
        help='Path to validation queries.')
add_arg('--input', default=None, type=str,
        help='Prefix of the files set as output in constructClusters.py')

args = parser.parse_args()

import pickle
import numpy as np

if __name__ == '__main__':

    assert args.input is not None, "--input is not specified."

    INPUT = 'data/' + args.input + '_train_labeledDict'
    OUTPUT = args.input + '_result.dat'
    QUERY_DAT = args.queries

    print('Load dictionaries with histograms.')

    descriptors = None

    # keys of descriptors dictionary: 'histogram', 'fname'
    with open(INPUT, 'rb') as fp:
       descriptors = pickle.load(fp)

    print('Loading done. Now evaluate results and dump to file.')

    with open(OUTPUT, 'w') as resultFp:
        
        with open(QUERY_DAT, 'r') as queryDatFp:

            for line in queryDatFp:

                queryFname = line[:-1]

                queryImgHist = None

                for descDict in descriptors:

                    anotherFname = descDict['fname']

                    if anotherFname == queryFname:
                        queryImgHist = descDict['histogram']
                        break

                sortedEucList = sorted(
                    [(np.linalg.norm(queryImgHist-anotherDict['histogram']),
                        anotherDict['fname']) for anotherDict in descriptors],
                    key=lambda distFnamePair: distFnamePair[0])
        
                resultFp.write(queryFname + ':')

                resultFp.write(' '.join(map(lambda distFnamePair:
                    str(distFnamePair[0]) + ' ' + distFnamePair[1], sortedEucList))
                    +'\n')

    print('All set.')

