#reduce
import torch
import torch.nn as nn
import torch.nn.functional as F

class oneLayerNet(nn.Module):

    def __init__(self, nneurons):
        super(oneLayerNet, self).__init__()
        self.fc1 = nn.Linear(nneurons[0], nneurons[1], bias=True)
        
        torch.nn.init.xavier_uniform_(self.fc1.weight)
        
        # if bn
        #self.bn1 = nn.BatchNorm1d(nneurons[0])

    def forward(self, x):
        # if bn
        #x = self.fc1(self.bn1(x))
        
        x = self.fc1(x)

        return x

class twoLayerNet(nn.Module):

    def __init__(self, nneurons):
        super(twoLayerNet, self).__init__()
        self.fc1 = nn.Linear(nneurons[0], nneurons[1], bias=True)
        self.fc2 = nn.Linear(nneurons[1], nneurons[2], bias=True)
        
        torch.nn.init.xavier_uniform_(self.fc1.weight)
        torch.nn.init.xavier_uniform_(self.fc2.weight)

        # if bn
        #self.bn1 = nn.BatchNorm1d(nneurons[0])
        #self.bn2 = nn.BatchNorm1d(nneurons[1])

    def forward(self, x):
        # if bn
        #x = F.relu(self.fc1(self.bn1(x)))
        #x = self.fc2(self.bn2(x))

        x = F.relu(self.fc1(x))
        x = self.fc2(x)

        return x

class threeLayerNet(nn.Module):

    def __init__(self, nneurons):
        super(threeLayerNet, self).__init__()
        self.fc1 = nn.Linear(nneurons[0], nneurons[1], bias=True)
        self.fc2 = nn.Linear(nneurons[1], nneurons[2], bias=True)
        self.fc3 = nn.Linear(nneurons[2], nneurons[3], bias=True)

        # if bn
        #self.bn1 = nn.BatchNorm1d(nneurons[0])
        #self.bn2 = nn.BatchNorm1d(nneurons[1])
        #self.bn3 = nn.BatchNorm1d(nneurons[2])

        torch.nn.init.xavier_uniform_(self.fc1.weight)
        torch.nn.init.xavier_uniform_(self.fc2.weight)
        torch.nn.init.xavier_uniform_(self.fc3.weight)

    def forward(self, x):
        # if bn
        #x = F.relu(self.fc1(self.bn1(x)))
        #x = F.relu(self.fc2(self.bn2(x)))
        #x = self.fc3(self.bn3(x))

        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = self.fc3(x)
        return x

class fourLayerNet(nn.Module):

    def __init__(self, nneurons):
        super(fourLayerNet, self).__init__()
        self.fc1 = nn.Linear(nneurons[0], nneurons[1], bias=True)
        self.fc2 = nn.Linear(nneurons[1], nneurons[2], bias=True)
        self.fc3 = nn.Linear(nneurons[2], nneurons[3], bias=True)
        self.fc4 = nn.Linear(nneurons[3], nneurons[4], bias=True)

        # if bn
        #self.bn1 = nn.BatchNorm1d(nneurons[0])
        #self.bn2 = nn.BatchNorm1d(nneurons[1])
        #self.bn3 = nn.BatchNorm1d(nneurons[2])
        #self.bn4 = nn.BatchNorm1d(nneurons[3])

        torch.nn.init.xavier_uniform_(self.fc1.weight)
        torch.nn.init.xavier_uniform_(self.fc2.weight)
        torch.nn.init.xavier_uniform_(self.fc3.weight)
        torch.nn.init.xavier_uniform_(self.fc4.weight)

    def forward(self, x):
        # if bn
        #x = F.relu(self.fc1(self.bn1(x)))
        #x = F.relu(self.fc2(self.bn2(x)))
        #x = F.relu(self.fc3(self.bn3(x)))
        #x = self.fc4(self.bn4(x))

        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = F.relu(self.fc3(x))
        x = self.fc4(x)
        return x

