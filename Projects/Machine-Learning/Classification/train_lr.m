% Used to train the model over  the training set. Output for this is a set
% of trained weights which we use as inputs for the test set. The input to
% this program is a N*(D+1) matrix. N is the number of samples and D + 1 is
% the length of each training sample vector.

load('data.mat');
train = 0.001;
itr = 500;
Nn = 513;
Kn = 10;


[N, Kinit] = size(train_input);


Tstart = train_label(:,1:1);
T = [];
for k = 1:1:N
i = Tstart(k);

Kend = [zeros(1,i) ones(1,1) zeros(1, 9-i)];
T = vertcat(T, Kend);
end

X = train_input(:,1:end);
X = [ones(N,1) X];


Wt = ones(Kn,Nn);

y=(Wt*X')';
y=exp(y);
s=sum(y,2);
for k=1:19978,
    y(k,:)=y(k,:)/s(k);
end
error1=-sum(sum(train_target.*log(y)));

Y = softmax(Wt*X')';


for k = 0:1:itr
Wk = Wt - train*((Y-T)'*X);
Y = softmax(Wt*X')';
Wt = Wk;
end
Error = sum(T-Y);

y=(Wt*X')';
y=exp(y);
s=sum(y,2);
for k=1:19978,
    y(k,:)=y(k,:)/s(k);
end
error2=-sum(sum(train_target.*log(y)));

test_lr;