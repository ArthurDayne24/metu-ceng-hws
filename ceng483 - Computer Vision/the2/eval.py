import argparse

parser = argparse.ArgumentParser()
add_arg = parser.add_argument

add_arg('--n', default=None, type=str, help='Model name, should be specified.')
add_arg('--self', default=0, type=int, help='Evaluate self portrait in test mode.')
add_arg('--tv', default='validate', type=str, help='\'test\' or \'validate mode\',\
        default=\'validate\'')

args = parser.parse_args()

if __name__ == '__main__':

    assert(args.n is not None)
    assert(args.tv in ['validate', 'test'])

    import torch
    import numpy as np

    to_args = {'device': torch.device('cpu'), 'dtype': torch.double}

    tv_mode = args.tv
    self_mode = True if args.self == 1 else False

    fname = 'mdl-' + format(args.n)

    tensor_x = tensor_y = None

    if tv_mode == 'validate':

        tensor_x = torch.from_numpy(np.ndarray.astype(np.load('valid.npy'),
            dtype=np.dtype('d'))) # pytorch valid

        tensor_y = np.load('valid_gt.npy') # np array

    elif tv_mode == 'test':

        npy_fname = 'test.npy' if not self_mode else 'self.npy'
        tensor_x = torch.from_numpy(np.ndarray.astype(np.load(npy_fname),
            dtype=np.dtype('d'))) # pytorch tensor

    model = None    
    state = None

    import nets
    
    state = torch.load('models/' + fname + '.dat')
    nneurons = state['nneurons']
    nlayers = len(nneurons) - 2

    if nlayers == 0:
        model = nets.oneLayerNet(nneurons).to(**to_args)
    elif nlayers == 1:
        model = nets.twoLayerNet(nneurons).to(**to_args)
    elif nlayers == 2:
        model = nets.threeLayerNet(nneurons).to(**to_args)
    elif nlayers == 3:
        model = nets.fourLayerNet(nneurons).to(**to_args)

    model.load_state_dict(state['model_state'])

    model.eval()

    with torch.no_grad():
        
        output = np.reshape(model(tensor_x).numpy(), newshape=(-1))

        if tv_mode == 'validate':
            zero_one_loss = np.mean(np.vectorize(
                lambda x: 1 if abs(x) > 10 else 0)(output - tensor_y))

            entry = 'Zero-One Accuracy: {}\n'.format(1-zero_one_loss)

            print(entry[:-1])

            np.save('eval/' + fname + '-estim-valid.npy', output)
            
            with open('eval/' + fname + '-estim-valid.log', 'w') as fp:
                fp.write(entry)

        elif tv_mode == 'test':
            np.save('eval/' + fname + 'estim-test.npy', output)

