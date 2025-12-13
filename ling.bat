@echo off
REM for logistic_regression instead of ding.bat which is for linear regression

CALL .\debug.bat

IF %ERRORLEVEL% NEQ 0 (
    ECHO Error: Build
    EXIT /B %ERRORLEVEL%
)

FOR /L %%q IN (0, 1, 100) DO (
    FOR /L %%i IN (2015, 1, 2020) DO (

        REM Train with a data
        .\build\Debug\regression_model.exe -trm -lor world_happiness_data %%ia %%i_test %%q
        
        REM Test with b data
        .\build\Debug\regression_model.exe -tem -lor world_happiness_data %%ib %%i_test %%q
    
        IF %ERRORLEVEL% NEQ 0 (
            ECHO Error: Run
            EXIT /B %ERRORLEVEL%
        )
    )
)