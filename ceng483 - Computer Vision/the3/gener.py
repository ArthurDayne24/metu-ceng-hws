from skimage import io, color
import numpy as np

def read_image(filename):
    img = io.imread(filename)
    if len(img.shape) == 2:
        img = np.stack([img, img, img], 2)
    return img

# Take GRAY return L
def cvt2L(image):
    Lab = color.rgb2lab(image)
    return Lab[:, :, 0]

# Take GRAY return AB
def cvt2AB(image):
    Lab = color.rgb2lab(image)
    return Lab[:, :, 1:]

if __name__ == '__main__':

    print("Start.")

    TRAIN_IMGNAMES_FILE = 'train.txt'
    VALID_IMGNAMES_FILE = 'valid.txt'
    TEST_IMGNAMES_FILE = 'test.txt'
    GRAYS_DIR = 'gray/'
    COLOR_LOW_DIR = 'color_64/'
    COLOR_HIGH_DIR = 'color_256/'
    TEST_DIR = 'test_gray/'
    TRAIN_OUT_FILE_L  = 'train_l_scaled.npy'
    TRAIN_OUT_FILE_AB = 'train_ab_scaled.npy'
    VALID_LOW_OUT_FILE_L  = 'valid_l_scaled.npy'
    VALID_LOW_OUT_FILE_AB = 'valid_ab_scaled.npy'    
    VALID_HIGH_OUT_FILE = 'valid_gt.npy'
    TEST_OUT_FILE = 'test.npy'

    imgnames = None

    with open(TRAIN_IMGNAMES_FILE, 'r') as imgname_file:

        imgnames = imgname_file.read().split('\n')[:-1]




    imgs = np.asarray([cvt2L(read_image(GRAYS_DIR+imgname)) for imgname in imgnames],
            dtype=np.double)

    imgs = np.reshape(imgs, (-1, 1, 256, 256))
    
    imgs = np.divide(imgs, 100)

    np.save(TRAIN_OUT_FILE_L, imgs)
    



    imgs = np.asarray([cvt2AB(read_image(COLOR_LOW_DIR+imgname)) for imgname in imgnames],
            dtype=np.double)

    imgs = np.moveaxis(imgs, 3, 1)

    imgs = np.divide(imgs, 128)

    np.save(TRAIN_OUT_FILE_AB, imgs)




    with open(VALID_IMGNAMES_FILE, 'r') as imgname_file:

        imgnames = imgname_file.read().split('\n')[:-1]




    imgs = np.asarray([cvt2L(read_image(GRAYS_DIR+imgname)) for imgname in imgnames],
            dtype=np.double)

    imgs = np.reshape(imgs, (-1, 1, 256, 256))
    
    imgs = np.divide(imgs, 100)

    np.save(VALID_LOW_OUT_FILE_L, imgs)




    imgs = np.asarray([cvt2AB(read_image(COLOR_LOW_DIR+imgname)) for imgname in imgnames],
            dtype=np.double)

    imgs = np.moveaxis(imgs, 3, 1)
    
    imgs = np.divide(imgs, 128)

    np.save(VALID_LOW_OUT_FILE_AB, imgs)




    imgs = np.asarray([read_image(
        COLOR_HIGH_DIR + imgname).reshape(-1).astype(np.int64)
        for imgname in imgnames], dtype=np.int64)

    np.save(VALID_HIGH_OUT_FILE, imgs)





    with open(TEST_IMGNAMES_FILE, 'r') as imgname_file:

        imgnames = imgname_file.read().split('\n')[:-1]


    imgs = np.asarray([cvt2L(read_image(TEST_DIR+imgname)) for imgname in imgnames],
            dtype=np.double)

    imgs = np.reshape(imgs, (-1, 1, 256, 256))
    
    imgs = np.divide(imgs, 100)

    np.save(TEST_OUT_FILE, imgs)




    print("Finish.")
    
