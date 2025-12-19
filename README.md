# Machine Learning with C++

Currently, the code contains a minor CLI for training and testing mlpack's Linear Regression and Logistic Regression classes.

There is example data located in ***/world_happiness_data***.

## Prepare Your Data

Assuming your data is already in CSV format, you can use the ***csv.py*** file to clean your CSV file(s).

The CLI expects CSV data for Linear and Logistic Regression models.

Checkout the example data in ***/world_happiness_data*** for more context.

## Use the CLI

Any command is a type of action followed by a type of model, and then the respective required parameters.

### Actions:
- Train a model with ***-trm***.
- Test a model with ***-tem***.

### Models:
- Use a Linear Regression model with ***-lr***.
- Use a Logistic Regression model with ***-lor***.

### Parameters:

#### Linear Regression:
1. <folder>: A folder containing your CSV files.
2. <filename>: Your CSV file.
3. <model_name>: The name of your model. (When you save a model it uses this name for the .bin file).
4. <important_column: integer>: The index of the dependent variable for your Linear Regression model (the 'correct' answers) inside your CSV file.
5. *<lambda: integer>: Only required when training.

#### Logistic Regression:
1. <folder>: A folder containing your CSV files.
2. <filename>: Your CSV file.
3. <model_name>: The name of your model. (When you save a model it uses this name for the .bin file).
4. *<lambda: integer>: Only required when training.

## Examples:

### Train a Linear Regression model
```
usr/bin/you> a.exe -trm -lr world_happiness_data 2020a.csv 2020a_test 0 25
```

### Citations
```
Conrad Sanderson and Ryan Curtin.
Armadillo: An Efficient Framework for Numerical Linear Algebra.
International Conference on Computer and Automation Engineering, 2025.

Conrad Sanderson and Ryan Curtin.
Practical Sparse Matrices in C++ with Hybrid Storage and Template-Based Expression Optimisation.
Mathematical and Computational Applications, Vol. 24, No. 3, 2019.
```