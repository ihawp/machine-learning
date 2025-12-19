@echo off
REM Call all of my bat files.

cls

CALL .\debug.bat

IF %ERRORLEVEL% NEQ 0 (
    ECHO Error: Build
    EXIT /B %ERRORLEVEL%
)

cls

ECHO(

ECHO Successful Build

CALL .\ling.bat

IF %ERRORLEVEL% NEQ 0 (
    ECHO Error: Logistic Regression
    EXIT /B %ERRORLEVEL%
)

CALL .\ding.bat

IF %ERRORLEVEL% NEQ 0 (
    ECHO Error: Linear Regression
    EXIT /B %ERRORLEVEL%
)