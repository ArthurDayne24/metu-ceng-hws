import argparse

parser = argparse.ArgumentParser()
add_arg = parser.add_argument

add_arg('--b', default=8, type=int, help='Batch size, default=8.')
add_arg('--lr', default=0.0001, type=float, help='Learning rate,\
        default=0.0001')
add_arg('--e', default=20, type=int, help='Epochs, default=20.')
add_arg('--w', default=0.0001, type=float, help='Weight decay,\
        default=0.0001')
add_arg('--r', default=0, type=int,
        help='Create a new model if 0, else reload model specified above and\
        ignore \'args.lr, args.b and args.w\' parameters\
        (to be derived from saved model), default=0.')
add_arg('--n', default=None, type=str, help='Model name, should be specified.')
add_arg('--c', default=1, type=int, help='On cuda (1) or not (1), default=0.')
add_arg('--s', default=0, type=int, help='Save for each n epochs, 0 disables, default=0.')

args = parser.parse_args()

def upsample(image):
    return rescale(image, 4, mode='constant', order=3)

def cvt2rgb(image):
    return color.lab2rgb(image)

if __name__ == '__main__':

    assert(args.r in [0, 1])
    assert(args.e > 0)
    assert(args.n is not None)
    assert(args.c in [0, 1])
    assert(args.s >= 0)

    if args.r == 0:
        assert(args.b > 0)
        assert(args.w >= 0)
        assert(args.lr > 0 and args.lr < 1)

    from math import ceil
    import torch
    import torch.nn as nn
    import torch.optim as optim
    import torch.utils.data as utils
    import net
    from skimage import color
    from skimage.transform import rescale
    import numpy as np

    batch_size = args.b
    lr = args.lr
    nepochs = args.e
    weight_decay = args.w
    do_reload = True if args.r else False
    on_cuda = True if args.c else False
    save_every = args.s
    completed_epochs = 0

    TRAIN_OUT_FILE_L  = 'train_l_scaled.npy'
    TRAIN_OUT_FILE_AB = 'train_ab_scaled.npy'
    VALID_LOW_OUT_FILE_L  = 'valid_l_scaled.npy'
    VALID_LOW_OUT_FILE_AB = 'valid_ab_scaled.npy'
    VALID_HIGH_OUT_FILE = 'valid_gt.npy'

    to_args = {'device': torch.device('cuda' if on_cuda else 'cpu'),
            'dtype': torch.double}

    fname = 'mdl-' + format(args.n)

    train_x = train_y = valid_x = valid_y = None

    train_x = torch.from_numpy(np.ndarray.astype(np.load(TRAIN_OUT_FILE_L),
        dtype=np.dtype('d')))
    train_y = torch.from_numpy(np.ndarray.astype(np.load(TRAIN_OUT_FILE_AB),
        dtype=np.dtype('d')))

    valid_real = np.load(VALID_HIGH_OUT_FILE)

    if on_cuda:
        valid_x = torch.from_numpy(np.ndarray.astype(np.load(VALID_LOW_OUT_FILE_L),
            dtype=np.dtype('d'))).cuda()
        valid_y = torch.from_numpy(np.ndarray.astype(np.load(VALID_LOW_OUT_FILE_AB),
            dtype=np.dtype('d'))).cuda()

    else:
        valid_x = torch.from_numpy(np.ndarray.astype(np.load(VALID_LOW_OUT_FILE_L),
            dtype=np.dtype('d')))
        valid_y = torch.from_numpy(np.ndarray.astype(np.load(VALID_LOW_OUT_FILE_AB),
            dtype=np.dtype('d')))

    train_dataset = utils.TensorDataset(train_x, train_y)
    loader = utils.DataLoader(dataset=train_dataset, batch_size=batch_size, shuffle=True,
            num_workers=1, pin_memory=True)

    model = net.Net().to(**to_args)
    optimizer = optim.RMSprop(model.parameters(), lr=lr, weight_decay=weight_decay)

    if do_reload:
        state = torch.load('models/' + fname + '.dat')

        lr = state['lr']
        weight_decay = state['decay']
        batch_size = state['b_size']
        completed_epochs = state['completed_epochs']
        
        optimizer = optim.RMSprop(model.parameters(), lr=lr, weight_decay=weight_decay)
        optimizer.load_state_dict(state['optimizer'])
        
        model.load_state_dict(state['model_state'])

    torch.manual_seed(7)
    
    if on_cuda:
        torch.cuda.manual_seed_all(7)
  
    iters_per_epoch = ceil(800 / batch_size)

    with open('models/' + fname + '.log', 'w' if not do_reload else 'a') as fp:

        print("--------------------- " + fname + " ---------------------")

        if not do_reload:
            fp.write('\t'.join(
                ['batch-size', 'learning-rate', 'nepochs', 'wdecay']))
            fp.write('\n')        
            fp.write('\t'.join(map(lambda x: str(x),
                [batch_size, lr, nepochs, weight_decay])))
            fp.write('\n\n')

        for epoch in range(completed_epochs, completed_epochs+nepochs+1):

            entry = "Epoch {}\n".format(epoch)

            print(entry[:-1])
                    
            fp.write(entry)
   
            model.train()

            for batch_idx, (batch_x, batch_y) in enumerate(loader):

                batch_x, batch_y = batch_x.to(**to_args), batch_y.to(**to_args)

                # clear gradients
                optimizer.zero_grad()

                batch_output = model(batch_x)
        
                lossF = torch.nn.MSELoss()
                loss = lossF(batch_output, batch_y)

                if epoch != completed_epochs:
                    loss.backward()
                    
                    optimizer.step()

                if (batch_idx+1) == iters_per_epoch:
                 
                    entry = 'Training loss: {}\n'.format(loss.item())

                    print(entry[:-1])
                    
                    fp.write(entry)

            if epoch != completed_epochs or epoch == 0:

                model.eval()

                with torch.no_grad():
                    
                    val_output = model(valid_x)
                    
                    val_lossF = torch.nn.MSELoss()
                    val_loss = val_lossF(val_output, valid_y)

                    entry = 'Validation Loss: {}\n'.format(val_loss)

                    print(entry[:-1])

                    fp.write(entry)

                    AB = L = None

                    if on_cuda:
                        AB = val_output.cpu().numpy()
                        L = valid_x.cpu().numpy()

                    else:
                        AB = val_output.numpy()
                        L = L.numpy()

                    L = np.moveaxis(L, 1, -1) * 100
                    
                    AB = np.moveaxis(AB, 1, -1) * 128

                    AB = np.asarray([upsample(img) for img in AB], dtype=np.double)

                    LAB = np.concatenate((L, AB), 3)

                    RGB = np.asarray([(cvt2rgb(img) * 255).reshape(-1)
                        for img in LAB], dtype=np.int64)

                    acc = 0
                    
                    for i in range(100):
                        cur_acc = (np.abs(valid_real[i] - RGB[i]) < 12).sum() / \
                            (256 * 256 * 3)
                       
                        acc += cur_acc

                    acc /= 100.0
                    
                    entry = 'Accuracy: {}\n'.format(acc)

                    print(entry[:-1])
                    
                    fp.write(entry)

                if save_every and epoch % save_every == 0:

                    state = {
                        'model_state': model.state_dict(),
                        'optimizer': optimizer.state_dict(),
                        'lr': lr,
                        'b_size' : batch_size,
                        'decay': weight_decay,
                        'completed_epochs': epoch
                    }

                    torch.save(state, 'models/' + fname + 'ep-{}.dat'.format(epoch))

    state = {
        'model_state': model.state_dict(),
        'optimizer': optimizer.state_dict(),
        'lr': lr,
        'b_size' : batch_size,
        'decay': weight_decay,
        'completed_epochs': completed_epochs+nepochs
    }

    torch.save(state, 'models/' + fname + '.dat')

    print('All set.')

