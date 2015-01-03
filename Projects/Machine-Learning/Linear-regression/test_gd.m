
% This function takes as input the input matrix of the test data along with
% the mu, sigma and weight matrix calculated in the training phase of
% stochastic gradient  solution model. It returns the error for the final
% data

function Erms = test_gd( mu, sigma, wml )

filename='project1_data.mat';
load(filename,'data');
N=69623;
N1=round((10/100)*N);

data2=cell2mat(data);

data1=data2(62662:N,2:47);

t=data2(62662:N,1:1);

%lambda=1;
edw=0;
eww=0;
j=6;
Ew=0;
Erms=0;
num_iterations=100;
%for j=1:20
  %for h=1:20
 
   k=round(N1/j);
   
    lambda=14;
    s=3;
    wml=zeros(6,1);
    alpha=7.812500e-03;
   for i=1:j 
    %samples=datasample(data1,k);
    %mu=mean(samples);
    %Erms=0;
    
    %clear phi;
    for z=1:N1
        phi(z,i)=exp(-(1/2)*(data1(z,:)-mu(i,:))*inv(sigma)*transpose(data1(z,:)-mu(i,:)));
        
    end
    
   end

    % Logic 2 where j weights are generated for j columns at a time
   %for l=1:num_iterations
    for i=1:N1
         wml=wml+transpose(alpha*(t(i)-phi(i,:)*wml)*phi(i,:));
    end
    wml;
    edw=0;
    eww=0;
    Ew=0;
    temp=0;
    Erms=0;
    for b=1:N1

     edw=((t(b)-phi(b,:)*(wml))^2)+edw;
    end
    
    edw=0.5*edw;
    
    
    Erms=sqrt(2*edw/N1);
   
%    alp(l)=alpha;
%    if (temp<Erms);
%     Erms=temp;
%     fprintf('NEW LOWEST!');
%    else
%       alpha=0.5*alpha;
%    end
   
    
    fprintf('j=%d, alpha =%d lambda=%d s=%d Erms=%d\n',j,alpha,lambda,s,Erms)
   %end
%   alp
  %end
%end