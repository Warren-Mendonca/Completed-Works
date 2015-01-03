

%Below code extracts the feature vectors and target vectors and saves them
%in the file project1_data.mat

%fileId=fopen('Querylevelnorm.txt');
%fmt=['%f %*s' repmat('%*d:%f',1,46) '%*[^\n]'];
%data=textscan(fileId,fmt,'collectoutput',1);
%fclose(fileId);
%celldisp(data);
%filename='project1_data.mat'
%save(filename,'data');


%------------------------For CF of maximum likelihood solution --------------------------------


% Here we call the function that calculates the Erms, mu and sigma 
% This train function returns the mean and variance used and
% weight matrix calculated

[Erms, wml, mu_cfs, sigma_cfs] = train_cfs();

% Calling the test_cfs. This function is called to test our model. This
% function takes the test target matrix and sigma, mean and weight
% generated in the training phase of the model
Erms_final_cfs = test_cfs( mu_cfs, sigma_cfs, wml );

%-------------------------------Stochastic Gradient descent part------------------------------------

% Calling the training function of the GD model with the input as the
% target and input of the training data. This function will return the
% sigma, mean and weights calculated in their respective variables.
[Erms, wml, mu_gd, sigma_gd] = train_gd();

% Calling the test_cfs. This function is called to test our CFS model. This
% function takes the test target matrix and sigma, mean and weight
% generated in the training phase of the model
Erms_final_gd = test_gd(mu_gd, sigma_gd, wml );


% ----------------------- Final output -------------------------------

fprintf('My ubit name is %s\n', 'wmendonc' );
fprintf('My student number is %d \n', 50134835);
fprintf('the model complexity M_cfs is %d\n', 14);
fprintf('the model complexity M_gd is %d\n', 6);
fprintf('the regularization parameters lambda_cfs is %4.2f\n', 16);
fprintf('the regularization parameters lambda_gd is %4.2f\n', 7.812500e-03);
fprintf('the root mean square error for the closed form solution is %4.2f\n', Erms_final_cfs);
fprintf('the root mean square error for the gradient descent method is %4.2f\n', Erms_final_gd);

