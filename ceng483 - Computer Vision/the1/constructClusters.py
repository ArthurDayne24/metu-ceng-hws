import argparse

parser = argparse.ArgumentParser()
add_arg = parser.add_argument

add_arg('--input', default=None, type=str,
        help='Prefix set in \'descriptor.py\' for descriptor arrays.')
add_arg('--output', default=None, type=str,
        help='Prefix of the file to output cluster info and histogram data.')
add_arg('--k', default=None, type=int,
        help='Number of clusters.')
add_arg('--batch', default=100, type=int,
        help='Batch size for minibatches. (0) for no minibatches.')
add_arg('--iters', default=100, type=int,
        help='Max iters for K-Means.')
add_arg('--justmse', default=0, type=int,
        help='If (1), calculate centroids and show MSE, then exit immediately.')

args = parser.parse_args()

import cv2
import pickle
import numpy as np

if __name__ == '__main__':

    assert args.output is not None, "--output is not specified."
    assert args.input is not None, "--input is not specified."
    assert args.k is not None, "--k is not specified."

    JUST_MSE = True if args.justmse == 1 else False
    BTC_SIZE = args.batch
    MAX_ITERS = args.iters
    N_CLUSTERS = args.k
    INPUT_PRE = "data/" + args.input 
    OUTPUT_PRE = "data/" + args.output

    print('Load descriptors\' numpy array ...')

    descriptorsArray = np.load(INPUT_PRE + '_train.npy')

    print('Done. Run K-Means.')

    kmeans = None

    if BTC_SIZE == 0:
        from sklearn.cluster import KMeans
        kmeans = KMeans(n_clusters=N_CLUSTERS, max_iter=MAX_ITERS).fit(descriptorsArray)
    else:
        from sklearn.cluster import MiniBatchKMeans
        kmeans = MiniBatchKMeans(n_clusters=N_CLUSTERS, max_iter=MAX_ITERS,
                batch_size=BTC_SIZE).fit(descriptorsArray)

    if JUST_MSE:
        #print(kmeans.inertia_ / len(descriptorsArray))
        print(kmeans.inertia_)
        exit()

    print('KMeans is done. Save it and load descriptor dictionaries.')

    with open(OUTPUT_PRE + '_train_clusters', 'wb') as fp:
        pickle.dump(kmeans, fp)

    descriptors = None

    with open(INPUT_PRE + '_train_descDict', 'rb') as fp:
       descriptors = pickle.load(fp) 

    print('Done. Construct each image\'s histogram.')

    labels = kmeans.labels_

    it = 0

    for descriptorDict in descriptors:

        descriptorDict['histogram'] = np.zeros(shape=(N_CLUSTERS,))
        ndescriptors = len(descriptorDict['descriptors'])

        for cid in labels[it: it+ndescriptors]:

            descriptorDict['histogram'][cid] += 1

        np.divide(descriptorDict['histogram'], ndescriptors)

        del descriptorDict['descriptors']
        
        it += ndescriptors
    
    print('Done. Save dictionaries with labels and log work.')

    with open(OUTPUT_PRE + '_train_labeledDict', 'wb') as fp:
       pickle.dump(descriptors, fp)

    with open(OUTPUT_PRE + '_train_clusters.log', "w") as fp:
        fp.write("K=" + str(N_CLUSTERS) + '\n')
        fp.write("MaxIters=" + str(MAX_ITERS) + '\n')
        if BTC_SIZE != 0:
            fp.write("BatchSize=" + str(BTC_SIZE) + '\n')
        else:
            fp.write("BatchSize=None\n")

    print('Done.')

