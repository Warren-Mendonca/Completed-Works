clc;
clear;
train_input = [];
train_label =[];
for k=0:9
    temp = strcat('C:/Users/Warren/Desktop/Course Material/Machine Learning/MATLAB/PA final/wmendonc/features_test/', num2str(k), '.txt');
    fileID = fopen(temp);
    format = [repmat('%f ', [1 512])];
    full = textscan(fileID, format);
    temp_input = cell2mat(full(1:end,1:end));
    height = size(temp_input, 1);
    temp_target = ones(height,1).*k;
%     temp_input =  [temp_mat target_mat];
    fclose(fileID);
    train_input = [train_input; temp_input];
    train_label = [train_label; temp_target];
end

height = size(train_label, 1);
train_target = zeros(height, 10);
for i = 1:height
    train_target(i,train_label(i,1) + 1) = 1;
end

save('data3.mat','train_input', 'train_target','train_label', '-append');
train_input = [];
train_label =[];

for k=0:9
    temp = strcat('C:/Users/Warren/Desktop/Course Material/Machine Learning/MATLAB/PA final/wmendonc/features_train/', num2str(k), '.txt');
    fileID = fopen(temp);
    format = [repmat('%f ', [1 512])];
    full = textscan(fileID, format);
    temp_input = cell2mat(full(1:end,1:end));
    height = size(temp_input, 1);
    temp_target = ones(height,1).*k;
%     temp_input =  [temp_mat target_mat];
    fclose(fileID);
    train_input = [train_input; temp_input];
    train_label = [train_label; temp_target];
end

height = size(train_label, 1);
train_target = zeros(height, 10);
for i = 1:height
    train_target(i,train_label(i,1) + 1) = 1;
end

save('data3.mat','train_input', 'train_target','train_label', '-append');
