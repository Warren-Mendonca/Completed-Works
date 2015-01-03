The submission contains the Report and Code.

  . Please extract the code folder
  . This folder contains `train_cfs.m, train_gd.m, test_cfs.m, test_gd.m, project1.m`
  . This folder also contains mat files like `project1_data.mat, W_cfs.mat, W_gd.mat, mu_cfs.mat, mu_gd.mat, s_cfs.mat, s_gd.mat`
  . To run please execute the project1.m file

Functions

1. train_cfs.m
      - Output : [Erms, weight mu sigma]
      	Returns weight learnt. mu matrix and sigma matrix
2. test_cfs.m
   - Input : [weight mu sigma] 
   	 Takes in weight matrix, mu matrix and
     sigma matrix from train_cfs function 

   - Output : [Erms]
    	Returns the Erms predicted

3. train_gd.m
   - Input : [input target]
   * Takes in input matrix and the target matrix

   - Output : [Erms weight mu sigma]
   	Returns Erms weight learnt. mu matrix and sigma matrix

4. test_gd.m
   - Input : [weight mu sigma] 
   	 Takes in weight matrix, mu matrix and
     sigma matrix from train_gd function 

   - Output : [Erms]
    	Returns the Erms predicted

5. project1.m
   - Input : no input

   - Output : Final values
