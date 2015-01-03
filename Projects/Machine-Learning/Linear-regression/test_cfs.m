
% This function takes as input the input matrix of the test data along with
% the mu, sigma and weight matrix calculated in the training phase of
% closed-form solution model. It returns error for the final data set

function Erms = test_cfs( mu, sigma, wml )

filename='project1_data.mat';
load(filename,'data');
N=69623;
N1=round((10/100)*N);

data2=cell2mat(data);

data1=data2(62662:N,2:47);

t=data2(62662:N,1:1);
lambda=1;
edw=0;
eww=0;
j=14;
%s=3;
Ew=0;
Erms=0;

%for j=1:20
  %for h=1:20
%      for q=1:10
   k=round(N1/j);
   lambda=16;
    s=3;
   for i=1:j 
    samples=datasample(data1,k);
    %mu=mean(samples);
    edw=0;
    eww=0;
    Ew=0;
    Erms=0;
    %clear phi wml;
    
    for z=1:N1
        phi(z,i)=exp(-(1/2)*(data1(z,:)-mu(i,:))*inv(sigma)*transpose(data1(z,:)-mu(i,:)));
        
    end
    
   end
   
   %wml=inv(lambda*eye(j,j)+(transpose(phi)*phi))*(transpose(phi)*t);
   % We predict the ttarget matrix using the weights we have calculated in
   % the training section
   % target = zeros(test_height, 1);
   % for i = 1:N1
   % target(i,1) = phi(i,:) * wml;
   %end 
   
   for b=1:N1

    edw=((t(b)-phi(b,:)*(wml))^2)+edw;
   end
   edw=0.5*edw;
    
   %for b=1:j
   %   eww=wml(b)^2+eww; 
   %end
   %eww=0.5*eww;
   
   %Ew=edw+lambda*eww;
   Erms=sqrt(2*edw/N1); 
   %Er_v=Erms
   fprintf('Model complexity M=%d, lambda=%d s=%d Erms=%d\n',j,lambda,s,Erms)
%      end
  %end
%end
   