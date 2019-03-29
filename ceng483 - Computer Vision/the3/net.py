import torch
import torch.nn as nn

# Note: All conv. layers have dilation=0, groups = 1 and bias = True
#       All pooling layers have dilation=1, return_indices=False, ceil_mode=False
# as default

# Expects scaled input [0, 1] of L channel

class Net(nn.Module):

    def __init__(self):
        super(Net, self).__init__()

        self.relu = nn.ReLU()

        self.conv1 = nn.Conv2d(in_channels=1, out_channels=15, kernel_size=5,
                stride=1, padding=2)

        self.pool1 = nn.MaxPool2d(kernel_size=2, stride=2, padding=0)
 
        self.bn1 = nn.BatchNorm2d(15)

        self.conv2 = nn.Conv2d(in_channels=15, out_channels=24, kernel_size=3,
                stride=1, padding=1)

        self.pool2 = nn.MaxPool2d(kernel_size=2, stride=2, padding=0)

        self.bn2 = nn.BatchNorm2d(24)

        self.conv3 = nn.Conv2d(in_channels=24, out_channels=2, kernel_size=3,
                stride=1, padding=1)

        torch.nn.init.xavier_uniform_(self.conv1.weight.data)
        torch.nn.init.xavier_uniform_(self.conv2.weight.data)
        torch.nn.init.xavier_uniform_(self.conv3.weight.data)

    def forward(self, x):

        # 256 x 256 x 1

        x = self.conv1(x)
        # 256 x 256 x 15
        x = self.pool1(x)
        # 128 x 128 x 15
        x = self.bn1(x)
        # 128 x 128 x 15
        x = self.relu(x)
        # 128 x 128 x 15

        x = self.conv2(x)
        # 128 x 128 x 24
        x = self.pool2(x)
        # 64 x 64 x 24
        x = self.bn2(x)
        # 64 x 64 x 24
        x = self.relu(x)
        # 64 x 64 x 24
        
        x = self.conv3(x)
        # 64 x 64 x 2

        return x

