import argparse

parser = argparse.ArgumentParser()
add_arg = parser.add_argument

add_arg('--l', nargs='*', default=None, type=int,
        help='Neurons per hidden layers, should be specified if there is at\
        least one hidden layer.')
add_arg('--n', default=None, type=str,
        help='Model name, should be specified.')
add_arg('--r', default=0, type=int,
        help='Create a new model if 0, else reload model specified above and\
        ignore \'args.lr, args.l and args.w\' parameters\
        (to be derived from saved model), default=0.')
add_arg('--b', default=32, type=int,
        help='Batch size, default=32.')
add_arg('--lr', default=0.001, type=float,
        help='Learning rate, default=0.001')
add_arg('--w', default=0.0, type=float,
        help='Weight decay, default=0.0')
add_arg('--e', default=100, type=int,
        help='Epochs, default=5.')
add_arg('--i', default=5, type=int,
        help='Log interval, default=5.')
add_arg('--v', default=0, type=int,
        help='Validate at the end of each epoch if 1, default=0')
add_arg('--s', default=0, type=int,
        help='Save for every n\'th epoch, 0 disables, default=0')
add_arg('--d', default='cuda', type=str,
        help='Device (cuda or cpu), default=\'cuda\'.')
add_arg('--bl', default=1, type=int, # TODO non conventional
        help='For training set, generate only epoch loss for each epoch (0),\
                also closes printing loss for each batch seperately. default=1.')

args = parser.parse_args()

if __name__ == '__main__':

    assert(args.b > 0)
    assert(args.e > 0)
    assert(args.i > 0)
    assert(args.v >= 0)
    assert(args.s >= 0)
    assert(args.n is not None)

    if args.r == 0:
        assert(args.l is None or len(args.l) < 4)
        assert(args.lr > 0 and args.lr < 1)
        assert(args.w >= 0)

    import torch
    import torch.nn as nn
    import torch.optim as optim
    import torch.utils.data as utils
    import numpy as np

    if not args.l:
        args.l = []

    batch_size = args.b
    nlayers = len(args.l)
    lr = args.lr
    log_interval = args.i
    nepochs = args.e
    do_validate = True if args.v == 1 else False
    save_every = args.s
    do_reload = True if args.r == 1 else False
    on_cuda = True if args.d == 'cuda' else False
    weight_decay = args.w
    do_log_batch_loss = True if args.bl == 1 else False

    to_args = None

    if on_cuda:
        to_args = {'device': torch.device('cuda'), 'dtype': torch.double}
    else:
        to_args = {'device': torch.device('cpu'), 'dtype': torch.double}

    fname = 'mdl-' + format(args.n)

    # Neural net to be included will use these
    nneurons = [512] + args.l + [1]

    # Workaround TODO
    mse_train_x = mse_train_y = None

    if on_cuda:
        mse_train_x = torch.from_numpy(np.ndarray.astype(np.load('train.npy'),
            dtype=np.dtype('d'))).cuda()
        mse_train_y = torch.from_numpy(np.ndarray.astype(np.reshape(
            np.load('train_gt.npy'), newshape=(-1, 1)), dtype=np.dtype('d'))).cuda()
    else:
        mse_train_x = torch.from_numpy(np.ndarray.astype(np.load('train.npy'),
            dtype=np.dtype('d')))
        mse_train_y = torch.from_numpy(np.ndarray.astype(np.reshape(
            np.load('train_gt.npy'), newshape=(-1, 1)), dtype=np.dtype('d')))

    train_x = torch.from_numpy(np.ndarray.astype(np.load('train.npy'),
        dtype=np.dtype('d')))
    train_y = torch.from_numpy(np.ndarray.astype(np.reshape(np.load('train_gt.npy'),
        newshape=(-1, 1)), dtype=np.dtype('d')))

    valid_x = valid_y = None

    if do_validate:
        if on_cuda:
            valid_x = torch.from_numpy(np.ndarray.astype(np.load('valid.npy'),
                dtype=np.dtype('d'))).cuda()
            valid_y = torch.from_numpy(np.ndarray.astype(np.reshape(
                np.load('valid_gt.npy'), newshape=(-1, 1)),
                dtype=np.dtype('d'))).cuda()
        else:
            valid_x = torch.from_numpy(np.ndarray.astype(np.load('valid.npy'),
                dtype=np.dtype('d')))
            valid_y = torch.from_numpy(np.ndarray.astype(np.reshape(
                np.load('valid_gt.npy'), newshape=(-1, 1)),
                dtype=np.dtype('d')))

    train_dataset = utils.TensorDataset(train_x, train_y)
    loader = utils.DataLoader(dataset=train_dataset, batch_size=batch_size, shuffle=True,
            num_workers=1, pin_memory=True)

    model = None
    state = None
    completed_epochs = 0

    import nets
    
    if do_reload:
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
    
    optimizer = optim.RMSprop(model.parameters(), lr=lr, weight_decay=weight_decay)

    if do_reload:
        lr = state['lr']
        weight_decay = state['decay']
        optimizer = optim.RMSprop(model.parameters(), lr=lr, weight_decay=weight_decay)

        model.load_state_dict(state['model_state'])
        optimizer.load_state_dict(state['optimizer'])
        completed_epochs += state['epoch']

    else:
        torch.manual_seed(7)
        torch.cuda.manual_seed(7)
  
    options = 'a' if do_reload else 'w'

    with open('models/' + fname + '.log', options) as fp:

        fp.write('\t'.join(
            ['batch-size', 'nlayers', 'layers', 'learning-rate', 'log-int', 'nepochs']))
        fp.write('\n')        
        fp.write('\t'.join(map(lambda x: str(x),
            [batch_size, nlayers, nneurons, lr, log_interval, nepochs])))
        fp.write('\n\n')

        entry = "Epoch 0\n"

        print(entry[:-1])
                
        fp.write(entry)

        model.eval()

        with torch.no_grad():
            
            if do_validate:
    
                val_output = model(valid_x)
                
                val_lossF = torch.nn.MSELoss()
                val_loss = val_lossF(val_output, valid_y)

                entry = 'Validation Loss: {}\n'.format(val_loss)

                print(entry[:-1])

                fp.write(entry)

                zero_one_loss = np.mean(np.vectorize(
                    lambda x: 1 if abs(x) > 10 else 0)
                    (val_output.cpu().numpy() - valid_y.cpu().numpy()))

                entry = 'Zero-One Accuracy: {}\n'.format(1-zero_one_loss)

                print(entry[:-1])

                fp.write(entry)


            if not do_log_batch_loss:

                train_output = model(mse_train_x)

                train_lossF = torch.nn.MSELoss()
                train_loss = train_lossF(train_output, mse_train_y)

                entry = 'Train Loss: {}\n'.format(train_loss)

                print(entry[:-1])

                fp.write(entry)


        for epoch in range(1, nepochs+1):

            entry = "Epoch {}\n".format(completed_epochs+epoch)

            print(entry[:-1])
                    
            fp.write(entry)
    
            model.train()

            for batch_idx, (batch_x, batch_y) in enumerate(loader):

                batch_x, batch_y = batch_x.to(**to_args), batch_y.to(**to_args)

                # clear gradients
                optimizer.zero_grad()

                batch_output = model(batch_x)
        
                # not minibatch loss
                lossF = torch.nn.MSELoss()
                loss = lossF(batch_output, batch_y)
                loss.backward()
                
                optimizer.step()

                if batch_idx % log_interval == 0 and do_log_batch_loss:
                 
                    entry = '[{}/{} ({:.0f}%)]\tLoss: {:.6f}\n'.format(
                        batch_idx * len(batch_x), len(loader.dataset),
                        100. * batch_idx / len(loader), loss.item())

                    print(entry[:-1])
                    
                    fp.write(entry)

            if save_every > 0 and epoch % save_every == 0:
                state = {
                    'epoch': completed_epochs+epoch,
                    'model_state': model.state_dict(),
                    'optimizer': optimizer.state_dict(),
                    'lr': lr,
                    'nneurons': nneurons,
                    'decay': weight_decay
                }

                torch.save(state, 'models/' + fname + 'ep{}.dat'.format(
                    completed_epochs+epoch))

            model.eval()

            with torch.no_grad():
                
                if do_validate:
        
                    val_output = model(valid_x)
                    
                    val_lossF = torch.nn.MSELoss()
                    val_loss = val_lossF(val_output, valid_y)

                    entry = 'Validation Loss: {}\n'.format(val_loss)

                    print(entry[:-1])

                    fp.write(entry)

                    zero_one_loss = np.mean(np.vectorize(
                        lambda x: 1 if abs(x) > 10 else 0)
                        (val_output.cpu().numpy() - valid_y.cpu().numpy()))

                    entry = 'Zero-One Accuracy: {}\n'.format(1-zero_one_loss)

                    print(entry[:-1])

                    fp.write(entry)


                if not do_log_batch_loss:

                    train_output = model(mse_train_x)

                    train_lossF = torch.nn.MSELoss()
                    train_loss = train_lossF(train_output, mse_train_y)
 
                    entry = 'Train Loss: {}\n'.format(train_loss)

                    print(entry[:-1])

                    fp.write(entry)
            
    state = {
        'epoch': completed_epochs+nepochs,
        'model_state': model.state_dict(),
        'optimizer': optimizer.state_dict(),
        'lr': lr,
        'nneurons': nneurons,
        'decay': weight_decay
    }

    torch.save(state, 'models/' + fname + '.dat')

    print('All set.')

