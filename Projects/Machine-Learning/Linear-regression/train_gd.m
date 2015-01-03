function [Erms, wml, mu, sigma] = train_gd()
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
j=6;
Ew=0;
Erms=10;
num_iterations=100;
%for j=1:100
  %for h=1:20
 
   k=round(N1/j);
   
    lambda=14;
    s=3;
    sigma=(s^2)*eye(46,46)
    wml=zeros(6,1);
    alpha=0.05;
   for i=1:j 
    samples=datasample(data1,k);
    mu(i,:)=mean(samples);
    %Erms=0;
    
    %clear phi;
    for z=1:N1
        phi(z,i)=exp(-(1/2)*(data1(z,:)-mu(i,:))*inv(sigma)*transpose(data1(z,:)-mu(i,:)));
        
    end
    phi;
   end

    % Logic  where j weights are generated for j columns at a time
   for l=1:num_iterations
    for i=1:N1
         wml=wml+transpose(alpha*(t(i)-phi(i,:)*wml)*phi(i,:));
    end
    wml;
    edw=0;
    eww=0;
    Ew=0;
    temp=0;
    
    for b=1:N1

     edw=((t(b)-phi(b,:)*(wml))^2)+edw;
    end
    
    edw=0.5*edw;
    
    for b=1:j
       eww=wml(b)^2+eww; 
    end
    eww=0.5*eww;
   
    Ew=edw+lambda*eww;
    
    temp=sqrt(2*Ew/N1);
    temp
    if (temp<Erms);
     Erms=temp;
     fprintf('NEW LOWEST!');
    else
       alpha=0.5*alpha;
    end
   
    fprintf('j=%d, alpha =%d lambda=%d s=%d Erms=%d\n',j,alpha,lambda,s,Erms)
filename='mu_gd.mat'
save(filename,'mu');
filename='sigma_gd.mat'
save(filename,'sigma');
filename='W_gd.mat'
save(filename,'wml');

   end
  %end
%end