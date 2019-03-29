import argparse

parser = argparse.ArgumentParser()
add_arg = parser.add_argument

add_arg('--n', default=None, type=str, help='Model name, should be specified.')
add_arg('--tv', default='validate', type=str, help='\'test\' or \'validate mode\',\
        default=\'validate\'')
add_arg('--c', default=1, type=int, help='On cuda (1) or not (1), default=1.')

args = parser.parse_args()

def upsample(image):
    return rescale(image, 4, mode='constant', order=3)

def cvt2rgb(image):
    return color.lab2rgb(image)

if __name__ == '__main__':

    assert(args.n is not None)
    assert(args.tv in ['validate', 'test'])
    assert(args.c in [0, 1])

    import torch
    import numpy as np
    from skimage import color
    from skimage.transform import rescale
    import net
    
    VALID_OUT_FILE_L  = 'valid_l_scaled.npy'
    TEST_OUT_FILE = 'test.npy'

    on_cuda = True if args.c else False
    tv_mode = args.tv
    fname = 'mdl-' + format(args.n)

    to_args = {'device': torch.device('cuda' if on_cuda else 'cpu'),
            'dtype': torch.double}

    L = None

    if on_cuda:
        L = torch.from_numpy(np.ndarray.astype(np.load(VALID_OUT_FILE_L
            if tv_mode == 'validate' else TEST_OUT_FILE),
            dtype=np.dtype('d'))).cuda()
    else:
        L = torch.from_numpy(np.ndarray.astype(np.load(VALID_OUT_FILE_L
            if tv_mode == 'validate' else TEST_OUT_FILE),
            dtype=np.dtype('d')))

    model = None    
    state = None
    
    state = torch.load('models/' + fname + '.dat')

    model = net.Net().to(**to_args)

    model.load_state_dict(state['model_state'])

    model.eval()

    with torch.no_grad():

        print("Ready.")

        AB = None

        if on_cuda:
            AB = model(L).cpu().numpy()
            L = L.cpu().numpy()

        else:
            AB = model(L).numpy()
            L = L.numpy()

        print("Get AB.")
      
        L = np.moveaxis(L, 1, -1) * 100
        
        AB = np.moveaxis(AB, 1, -1) * 128

        AB = np.asarray([upsample(img) for img in AB], dtype=np.double)

        LAB = np.concatenate((L, AB), 3)

        print("Reshaped.")

        RGB = np.asarray([cvt2rgb(img) * 255 for img in LAB], dtype=np.double)

        print("RGB'd.")

        if tv_mode == 'validate':
            np.save('estimations_validation.npy', RGB)

        elif tv_mode == 'test':
            np.save('estimations_test.npy', RGB)

        print("Done.")

