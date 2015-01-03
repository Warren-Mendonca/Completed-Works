% This program takes input weight vectors from the training program and
% uses them along with the test data to produce output classification
% labels for the data. The output is saved in Y.

load('data2.mat');

[NTest, col] = size(train_input);
train_input =[ones(NTest,1) train_input];

X = train_input;
Y = [];


for i = 1: 1 : NTest
       XTest = X(i,:);
       YTest = softmax(Wt*XTest');
       [num,idx] = max(YTest(:));
       [xf,yf] = ind2sub(size(YTest),idx);
       Y = vertcat(Y, xf-1);

end
