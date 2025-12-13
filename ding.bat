@echo off
REM Run the regression model on all data sets from 2015 -> 2020
SETLOCAL EnableDelayedExpansion

CALL .\debug.bat

IF %ERRORLEVEL% NEQ 0 (
    ECHO Error: Build
    EXIT /B %ERRORLEVEL%
)

FOR /L %%q IN (0, 1, 100) DO (
    FOR /L %%i IN (2015, 1, 2020) DO (

        REM Train with a data
        .\build\Debug\regression_model.exe -trm -lr world_happiness_data %%ia.csv %%i_test 0 %%q

        REM Test with b data
        .\build\Debug\regression_model.exe -tem -lr world_happiness_data %%ib.csv %%i_test 0 %%q

        IF %ERRORLEVEL% NEQ 0 (
            ECHO Error: Run
            EXIT /B %ERRORLEVEL%
        )
    )
)

ENDLOCAL