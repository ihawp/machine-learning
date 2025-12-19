@echo off
REM for logistic_regression instead of ding.bat which is for linear regression
SETLOCAL EnableDelayedExpansion

ECHO Logistic Regression Started (ling.bat)

FOR /L %%q IN (0, 1, 100) DO (
    FOR /L %%i IN (2015, 1, 2020) DO (

        REM Train with a data
        .\build\Debug\a.exe -trm -lor world_happiness_data %%ia %%i_test %%q >nul 2>&1
        
        REM Test with b data
        .\build\Debug\a.exe -tem -lor world_happiness_data %%ib %%i_test %%q >nul 2>&1
    
        IF %ERRORLEVEL% NEQ 0 (
            ECHO Error: Run %%i %%q
        )
    )
)

ECHO Logistic Regression Finished (ling.bat)

ENDLOCAL