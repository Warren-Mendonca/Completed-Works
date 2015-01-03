% Used to train the model over  the training set. Output for this is a set
% of trained weights which we use as inputs for the test set. The data is
% trained using the gradient descent method.

load('data.mat');

[N, kinit] = size(train_input);
Tstart = train_label(:,1:1);

T = [];

for i = 1:1:N
j = Tstart(i);
Kend = [zeros(1,j) ones(1,1) zeros(1, 9-j)];
T = vertcat(T, Kend);
end

phi = train_input(:,1:end);
train = 0.02;

phi = [ones(N,1) phi];

Nrns = 513;
Kend = 10; 
Nodes = 60;

Wa=rand(Nodes,Nrns)*2;
Wb=rand(Kend,Nodes)*2;

Wa=ones(Nodes,Nrns)-Wa;
Wb=ones(Kend,Nodes)-Wb;


for j = 1 : 1 : N
    i = round(rand(1)*N);
    
    if i==0
        i = i+1;
    end
    a1j = Wa*phi(i,:)';
    bij = logsig(a1j);
    
    a1k = Wb*bij;
    b1k = softmax(a1k);
    
    step1 = b1k'-T(i,:);
    gradient2 = step1'*bij';
    step2 = (bij.*(ones(Nodes,1)-bij)).*(Wb'*step1');
    
    gradient1 = step2*phi(i,:);
    
    Wa=Wa-train*gradient1;
    Wb=Wb-train*gradient2;
end

test_nn;