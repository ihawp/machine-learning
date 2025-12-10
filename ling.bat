@echo off
REM for logistic_regression instead of ding.bat which is for linear regression

CALL .\debug.bat

IF %ERRORLEVEL% NEQ 0 (
    ECHO Error: Build
    EXIT /B %ERRORLEVEL%
)

FOR /L %%q IN (0, 1, 100) DO (
    FOR /L %%i IN (2015, 1, 2020) DO (
        
        .\build\Debug\regression_model.exe -lor world_happiness_data %%i %%i_test %%q
    
        IF %ERRORLEVEL% NEQ 0 (
            ECHO Error: Run
            EXIT /B %ERRORLEVEL%
        )
    )
)