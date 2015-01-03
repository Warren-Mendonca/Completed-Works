function [Erms, wml, mu, sigma] = train_cfs()

clear;
clc;
filename='project1_data.mat';
load(filename,'data');
N=69623;
N1=round((80/100)*N);

data2=cell2mat(data);

data1=data2(1:N1,2:47);

t=data2(1:N1,1:1);
%lambda=1;
edw=0;
eww=0;
j=14;
Ew=0;
Erms=0;

% Code commented out used for training the data set
%for j=1:20
%  for h=1:20
 
k=round(N1/j);
   
lambda=16;
s=3;
sigma=(s^2)*eye(46,46);
    
   for i=1:j 
    samples=datasample(data1,k);
    mu(j,:)=mean(samples);
    edw=0;
    eww=0;
    Ew=0;
    Erms=0;
    %clear phi wml;
    for z=1:N1
        phi(z,i)=exp(-(1/2)*(data1(z,:)-mu(j,:))*inv(sigma)*transpose(data1(z,:)-mu(j,:)));
        
    end
    
   end
   
   wml=inv(lambda*eye(j,j)+(transpose(phi)*phi))*(transpose(phi)*t);
   
   for b=1:N1

    edw=((t(b)-phi(b,:)*(wml))^2)+edw;
   end
   edw=0.5*edw;
    
   for b=1:j
      eww=wml(b)^2+eww; 
   end
   eww=0.5*eww;
   
   Ew=edw+lambda*eww;
   Erms=sqrt(2*Ew/N1); 
   fprintf('j=%d, lambda=%d s=%d Erms=%d\n',j,lambda,s,Erms)

filename='mu_cfs.mat'
save(filename,'mu');
filename='sigma_cfs.mat'
save(filename,'sigma');


   
 
%  end
%end