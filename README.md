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

### Linear Regression

#### Train a Linear Regression model:
```
a.exe -trm -lr world_happiness_data 2020a.csv 2020_lr_model 0 25
```

#### Test a Linear Regression model:

Here we use the same model name as before, but update our data to a test set, ***2020b.csv***. We also remove the lambda parameter.

```
a.exe -tem -lr world_happiness_data 2020b.csv 2020_lr_model 0
```

### Logistic Regression

#### Train a Logistic Regression model:

Here ***2020a*** identifies the prefix for a properly initialized Logistic Regression dataset, where there is a file called ***2020a.csv*** and another file called ***2020a_threshold.csv***. The 'threshold file' contains one row with the same amount of columns as there are rows in ***2020a.csv***, where each column holds a value (0 or 1) representing correct/incorrect or happy/unhappy in our case.

```
a.exe -trm -lor -trm -lor world_happiness_data 2020a 2020_lor_model 25
```

#### Test a Logistic Regression model:
```
a.exe -trm -lor -trm -lor world_happiness_data 2020b 2020_lor_model
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