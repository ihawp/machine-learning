@echo off
REM Run the regression model on all data sets from 2015 -> 2020
SETLOCAL EnableDelayedExpansion

ECHO Linear Regression Started (ding.bat)

FOR /L %%q IN (0, 1, 100) DO (
    FOR /L %%i IN (2015, 1, 2020) DO (

        REM Train with a data
        .\build\Debug\a.exe -trm -lr world_happiness_data %%ia.csv %%i_test 0 %%q >nul 2>&1

        REM Test with b data
        .\build\Debug\a.exe -tem -lr world_happiness_data %%ib.csv %%i_test 0 %%q >nul 2>&1

        IF %ERRORLEVEL% NEQ 0 (
            ECHO Error: Run %%i %%q
        )
    )
)

ECHO Linear Regression Finished (ding.bat)

ENDLOCAL