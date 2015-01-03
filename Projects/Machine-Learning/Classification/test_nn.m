%This function take the input from the trained weights hich we have
%obtained in the training phase. This is then used to proveide us with
%classification labels for the test ata. The output is saved in Y.

load('data2.mat');

Y = [];
[NTest, cols] = size(train_input);
phi =[ones(NTest,1) train_input];

for i = 1: 1 : NTest
       phitest = phi(i,:);
       size(Wb);
       size(phitest);
       YTest=softmax(Wb*logsig(Wa*phitest'));
       [num,idx] = max(YTest(:));
       [x1,x2] = ind2sub(size(YTest),idx);
       Y = vertcat(Y,x1-1);
end